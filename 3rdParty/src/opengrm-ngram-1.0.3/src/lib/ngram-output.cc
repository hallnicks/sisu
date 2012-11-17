// ngram-output.cc
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Copyright 2009-2011 Brian Roark and Google, Inc.
// Authors: roarkbr@gmail.com  (Brian Roark)
//          allauzen@google.com (Cyril Allauzen)
//          riley@google.com (Michael Riley)
//
// \file

#include <deque>

#include <fst/arcsort.h>
#include <fst/vector-fst.h>

#include <ngram/ngram-output.h>

DEFINE_string(start_symbol, "<s>", "Class label for sentence start");
DEFINE_string(end_symbol, "</s>", "Class label for sentence start");

namespace ngram {

using std::deque;

using fst::VectorFst;
using fst::StdILabelCompare;

// Print the N-gram model: each n-gram is on a line with its weight
void NGramOutput::ShowNGramModel(bool showeps, bool neglogs,
				 bool intcnts, bool ARPA) const {
  ostrm_.precision(7);
  if (ARPA) {
    ShowARPAModel();
  } else {
    string str = "";  // init n-grams from unigram state
    double start_wt;  // weight of <s> (count or prob) same as unigram </s>
    if (UnigramState() >= 0) {  // show n-grams from unigram state
      ShowNGrams(UnigramState(), str, showeps, neglogs, intcnts);
      start_wt = WeightRep(GetFst().Final(UnigramState()).Value(),
			   neglogs, intcnts);
      str = FLAGS_start_symbol;  // init n-grams from <s> state
    } else {
      start_wt = WeightRep(GetFst().Final(GetFst().Start()).Value(),
			   neglogs, intcnts);
    }
    // print <s> unigram following SRILM
    ostrm_ << FLAGS_start_symbol << '\t' << start_wt << '\n';
    ShowNGrams(GetFst().Start(), str, showeps, neglogs, intcnts);
  }
}

// Use n-gram model to calculate perplexity of input strings.
void NGramOutput::PerplexityNGramModel(vector<StdMutableFst* > *infsts,
				       int32 v, bool phimatch, 
				       string *OOV_symbol,
				       double OOV_class_size, 
				       double OOV_probability) {
  bool verbose = v > 0;
  Label OOV_label = GetOOVLabel(&OOV_probability, OOV_symbol);
  StdMutableFst*symbol_fst =
    (*infsts)[infsts->size() - 1]->InputSymbols() == 0 ? GetMutableFst() :
    (*infsts)[infsts->size() - 1];
  double logprob = 0, OOV_cost = StdArc::Weight::Zero().Value();
  int word_cnt = 0, oov_cnt = 0, words_skipped = 0;
  if (OOV_probability > 0) 
    OOV_cost = -log(OOV_probability / OOV_class_size);
  RenormUnigramForOOV(kSpecialLabel, OOV_label, 
		      OOV_class_size, OOV_probability);
  if (phimatch)
    MakePhiMatcherLM(kSpecialLabel);
  for (StateId i = 0; i < infsts->size(); ++i)
    ApplyNGramToFst((*infsts)[i], symbol_fst, phimatch, verbose, 
		    kSpecialLabel, OOV_label, OOV_cost, &logprob, 
		    &word_cnt, &oov_cnt, &words_skipped);
  ShowPerplexity(infsts->size(), word_cnt, oov_cnt, words_skipped, logprob);
}

// Print the header portion of the ARPA model format
void NGramOutput::ShowARPAHeader() const {
  vector<int> ngram_counts;
  for (int i = 0; i < HiOrder(); ++i)
    ngram_counts.push_back(0);
  for (StateId st = 0; st < GetFst().NumStates(); ++st) {
    if (StateOrder(st) == 1)  // if unigram state
      ngram_counts[0] += GetFst().NumArcs(st) + 1;  // count total arcs + <s>
    else
      ngram_counts[StateOrder(st) - 1] +=
	GetFst().NumArcs(st) - 1;  // count all arcs except for backoff arc
    if (GetFst().Final(st) != StdArc::Weight::Zero())
      ++ngram_counts[StateOrder(st) - 1];  // include </s> for all orders
  }
  ostrm_ << "\n\\data\\\n";
  for (int i = 0; i < HiOrder(); ++i)
    ostrm_ << "ngram " << i + 1 << "=" << ngram_counts[i] << '\n';
  ostrm_ << '\n';
}

// Print n-grams leaving a particular state for the ARPA model format
void NGramOutput::ShowARPANGrams(StdArc::StateId st,
				 const string &str, int order)
  const {
  if (st < 0 || StateOrder(st) > order) return;  // ignore for st < 0
  if (order == StateOrder(st) &&  // only show target order
      GetFst().Final(st) != StdArc::Weight::Zero()) {  // </s> n-gram to show
    // log_10(p)
    ostrm_ << ShowLogNewBase(GetFst().Final(st).Value(), 10) << "\t";
    if (str.size() > 0) ostrm_ << str << " ";
    ostrm_ << FLAGS_end_symbol << '\n';
  }
  for (ArcIterator<StdExpandedFst> aiter(GetFst(), st);
       !aiter.Done();
       aiter.Next()) {
    StdArc arc = aiter.Value();
    if (arc.ilabel == BackoffLabel())  // ignore backoff arc
      continue;
    // Find symbol str.
    string symbol = GetFst().InputSymbols()->Find(arc.ilabel);
    string newstr = str;  // history string
    AppendWordToNGramHistory(&newstr, symbol);  // Full n-gram
    if (order == StateOrder(st)) {  // only show target order
      ostrm_ << ShowLogNewBase(arc.weight.Value(), 10);  // log_10 n-gram prob
      ostrm_ << "\t" << newstr;
      if (StateOrder(arc.nextstate) > StateOrder(st))  // show backoff
	ostrm_ << "\t" << ShowLogNewBase(GetBackoffCost(arc.nextstate), 10);
      ostrm_ << '\n';
    }
    if (arc.ilabel != BackoffLabel() &&  // depth-first traversal
	StateOrder(arc.nextstate) > StateOrder(st))
      ShowARPANGrams(arc.nextstate, newstr, order);
  }
}

// Print the N-gram model in ARPA format
void NGramOutput::ShowARPAModel() const {
  ostrm_.precision(7);
  ShowARPAHeader();
  for (int i = 0; i < HiOrder(); ++i) {
    ostrm_ << "\\" << i + 1 << "-grams:\n";
    if (i == 0) {  // following SRILM, add <s> unigram w/ dummy weight of -99
      ostrm_ << "-99\t" << FLAGS_start_symbol << '\t';
      if (UnigramState() >= 0)  // <s> state exists, then show backoff
	ostrm_ << ShowLogNewBase(GetBackoffCost(GetFst().Start()), 10);
      ostrm_ << '\n';
    }
    if (UnigramState() >= 0) {  
      // init n-grams from <s> state
      ShowARPANGrams(GetFst().Start(), FLAGS_start_symbol, i + 1);
      // show n-grams from unigram state
      ShowARPANGrams(UnigramState(), "", i + 1);
    } else {
      // init n-grams from unigram state
      ShowARPANGrams(GetFst().Start(), "", i + 1);
    }
    ostrm_ << '\n';
  }
  ostrm_ << "\\end\\\n";
}

// Print n-grams leaving a particular state, standard output format
void NGramOutput::ShowNGrams(StdArc::StateId st, const string &str,
			     bool showeps, bool neglogs, bool intcnts) const {
  if (st < 0) return;  // ignore for st < 0
  for (ArcIterator<StdExpandedFst> aiter(GetFst(), st);
       !aiter.Done();
       aiter.Next()) {
    StdArc arc = aiter.Value();
    if (arc.ilabel == BackoffLabel() && !showeps)  // skip backoff unless reqd
      continue;
    // Find symbol str.
    string symbol = GetFst().InputSymbols()->Find(arc.ilabel);
    string newstr = str;  // history string
    AppendWordToNGramHistory(&newstr, symbol);  // Full n-gram string
    ostrm_ << newstr << "\t";  // output n-gram and its weight
    ostrm_ << WeightRep(arc.weight.Value(), neglogs, intcnts) << '\n';
    if (arc.ilabel != BackoffLabel() &&  // depth-first traversal
	StateOrder(arc.nextstate) > StateOrder(st))
      ShowNGrams(arc.nextstate, newstr, showeps, neglogs, intcnts);
  }
  if (GetFst().Final(st) != StdArc::Weight::Zero()) {  // show any </s> counts
    if (str.size() > 0)  // if history string, print it
      ostrm_ << str << " ";
    ostrm_ << FLAGS_end_symbol << '\t' << WeightRep(GetFst().Final(st).Value(),
				    neglogs, intcnts);
    ostrm_ << '\n';
  }
}

// Show string from linear fst, for verbose output of perplexities
void NGramOutput::ShowStringFst(const Fst<StdArc> &infst) const {
  StateId st = infst.Start();
  while (infst.NumArcs(st) != 0) {
    ArcIterator< Fst<StdArc> > aiter(infst, st);
    StdArc arc = aiter.Value();
    string symbol = GetFst().InputSymbols()->Find(arc.ilabel);
    if (st != infst.Start())
      ostrm_ << " ";
    ostrm_ << symbol;
    st = arc.nextstate;
  }
  ostrm_ << '\n';
}

void NGramOutput::RelabelAndSetSymbols(StdMutableFst *infst,
				       Fst<StdArc> *symbolfst) {
  for (StateId st = 0; st < infst->NumStates(); ++st) {
    for (MutableArcIterator<StdMutableFst> aiter(infst, st);
	 !aiter.Done();
	 aiter.Next()) {
      StdArc arc = aiter.Value();
      string symbol = symbolfst->InputSymbols()->Find(arc.ilabel);
      int64 key = GetFst().InputSymbols()->Find(symbol);
      if (key < 0) {
	key = GetMutableFst()->MutableInputSymbols()->AddSymbol(symbol);
	GetMutableFst()->MutableOutputSymbols()->AddSymbol(symbol);
      }
      arc.ilabel = key;
      arc.olabel = key;
      aiter.SetValue(arc);
    }
  }
  ArcSort(infst, StdILabelCompare());
  infst->SetInputSymbols(GetFst().OutputSymbols());
  infst->SetOutputSymbols(GetFst().InputSymbols());
}

// Apply n-gram model to fst.  For now, assumes linear fst, accumulates stats
// TODO(riley): note infst is deleted by this function
double NGramOutput::ApplyNGramToFst(StdMutableFst *infst,
				    Fst<StdArc> *symbolfst,
				    bool phimatch, bool verbose,
				    Label special_label, Label OOV_label,
				    double OOV_cost,
				    double *logprob, int *words,
				    int *oovs, int *words_skipped) {
  RelabelAndSetSymbols(infst, symbolfst);
  // relabel arcs and remove symbol table
  if (verbose) {
    ShowStringFst(*infst);
    ShowNGramProbHeader();
  }
  if (phimatch) {
    ComposeFst<StdArc> *cfst = FailLMCompose(*infst, special_label);
    ShowPhiPerplexity(*cfst, verbose, special_label, OOV_label,
		      logprob, words, oovs, words_skipped);
    delete cfst;
  } else {
    ShowNonPhiPerplexity(*infst, verbose, OOV_cost, OOV_label,
			 logprob, words, oovs, words_skipped);
  }
  delete infst;
  return *logprob;
}

void NGramOutput::ShowPhiPerplexity(const ComposeFst<StdArc> &cfst,
				    bool verbose,  Label special_label,
				    Label OOV_label, double *logprob, 
				    int *words, int *oovs,
				    int *words_skipped) const {
  StateId st = cfst.Start();
  int word_cnt = 0, oov_cnt = 0, skipped = 0;
  double neglogprob = 0, ngram_cost;
  string history = FLAGS_start_symbol + " ";
  while (cfst.NumArcs(st) != 0) {
    ArcIterator< Fst<StdArc> > aiter(cfst, st);
    StdArc arc = aiter.Value();
    string symbol = GetFst().InputSymbols()->Find(arc.ilabel);
    ngram_cost = ShowLogNewBase(arc.weight.Value(), 10);
    ++word_cnt;
    if (arc.olabel == special_label) {
      if (verbose)
	ShowNGramProb(symbol, history, 1, -1, -ngram_cost);
      history = "";
      ++oov_cnt;
      if (ngram_cost != -StdArc::Weight::Zero().Value()) {
	neglogprob += ngram_cost;
      } else {
	skipped++;  // no cost to OOV, word skipped for perplexity
      }
    } else {
      if (verbose)
	ShowNGramProb(symbol, history, 0, -1, -ngram_cost);
      if (arc.olabel == OOV_label)  // OOV is symbol in the model
	++oov_cnt;
      history = symbol + " ...";
      neglogprob += ngram_cost;
    }
    st = arc.nextstate;
  }
  ngram_cost = ShowLogNewBase(cfst.Final(st).Value(), 10);
  if (verbose)
    ShowNGramProb(FLAGS_end_symbol, history, 0, -1, -ngram_cost);
  neglogprob += ngram_cost;
  if (verbose) 
    ShowPerplexity(1, word_cnt, oov_cnt, skipped, neglogprob);
  (*logprob) += neglogprob;
  (*oovs) += oov_cnt;
  (*words) += word_cnt;
  (*words_skipped) += skipped;
}

void NGramOutput::ShowNonPhiPerplexity(const Fst<StdArc> &infst,
				       bool verbose, double OOV_cost, 
				       Label OOV_label, double *logprob,
				       int *words, int *oovs,
				       int *words_skipped) const {
  StateId st = infst.Start(), mst = GetFst().Start();
  int word_cnt = 0, oov_cnt = 0, skipped = 0;
  double neglogprob = 0;
  string history = FLAGS_start_symbol + " ";
  while (infst.NumArcs(st) != 0) {  // assumes linear fst (string)
    ArcIterator< Fst<StdArc> > aiter(infst, st);
    StdArc arc = aiter.Value();
    st = arc.nextstate;
    FindNextStateInModel(&mst, arc.ilabel, OOV_cost, OOV_label,
			 &neglogprob, &word_cnt, &oov_cnt, 
			 &skipped, &history, verbose);
  }
  ApplyFinalCost(mst, history, word_cnt, oov_cnt, skipped, neglogprob,
		 logprob, words, oovs, words_skipped, verbose);
}

void NGramOutput::FindNextStateInModel(StateId *mst, Label label,
				       double OOV_cost, Label OOV_label,
				       double *neglogprob, int *word_cnt,
				       int *oov_cnt, int *skipped, 
				       string *history, bool verbose) const {
  int order;
  double ngram_cost;
  string symbol = GetFst().InputSymbols()->Find(label);
  ++(*word_cnt);
  if (!FindNGramInModel(mst, &order, label, &ngram_cost)) {  // OOV
    ++(*oov_cnt);
    // Unigram state.
    ngram_cost += OOV_cost;
    ngram_cost = ShowLogNewBase(-ngram_cost, 10);
    if (OOV_cost != StdArc::Weight::Zero().Value()) {
      (*neglogprob) += ngram_cost;
    } else {
      (*skipped)++;
    }
    (*mst) = (UnigramState() >= 0) ? UnigramState() : GetFst().Start();
    if (verbose)
      ShowNGramProb(symbol, (*history), 1, -1, ngram_cost);
    (*history) = "";
  } else {
    if (label == OOV_label)
      ++(*oov_cnt);      
    ngram_cost = ShowLogNewBase(-ngram_cost, 10);
    (*neglogprob) += ngram_cost;
    if (verbose)
      ShowNGramProb(symbol, (*history), 0, order, ngram_cost);
    (*history) = symbol + " ...";
  }
}

//  Calculate and show (if verbose) </s> n-gram, and accumulate stats
void NGramOutput::ApplyFinalCost(StateId mst, string history, int word_cnt,
				 int oov_cnt, int skipped, double neglogprob,
				 double *logprob, int *words, int *oovs, 
				 int *words_skipped, bool verbose) const {
  int order;
  double ngram_cost = ShowLogNewBase(-FinalCostInModel(mst, &order), 10);
  neglogprob += ngram_cost;
  if (verbose) {
    ShowNGramProb(FLAGS_end_symbol, history, (order < 0), order, ngram_cost);
    ShowPerplexity(1, word_cnt, oov_cnt, skipped, -neglogprob);
  }
  (*logprob) -= neglogprob;
  (*words) += word_cnt;
  (*oovs) += oov_cnt;
  (*words_skipped) += skipped;
}



// Show the verbose n-gram entries with history order and neglogprob
void NGramOutput::ShowNGramProb(string symbol, string history,
				bool oov, int order,
				double ngram_cost) const {
  ostrm_ << "        p( " << symbol;
  if (history.size() == 0)
    ostrm_ << " )  ";
  else ostrm_ << " | " << history << ")";
  for (int i = symbol.size() + history.size(); i < 30; ++i)
    ostrm_ << " ";
  ostrm_ << "= ";
  if (oov)  // reporting OOV
    ostrm_ << "[OOV]    " << ngram_cost << '\n';
  else if (order < 0)
    ostrm_ << "[NGram]  " << ngram_cost << '\n';
  else  // order of the state out of which the arc came
    ostrm_ << "[" << order << "gram]  " << ngram_cost << '\n';
}

// Calculate prob of </s> and add to accum'd prob, and update total prob
double NGramOutput::SetInitRandProb(StateId hi_state,
				    StateId st, double *r) const {
  double p = 0.0, hi_neglog_sum, low_neglog_sum;
  if (hi_state >= 0) {
    CalcBONegLogSums(hi_state, &hi_neglog_sum, &low_neglog_sum);
    (*r) *= 1 - exp(-low_neglog_sum);
    if (GetFst().Final(hi_state).Value() ==
	StdArc::Weight::Zero().Value() &&
	GetFst().Final(st).Value() != StdArc::Weight::Zero().Value())
      p += exp(-GetFst().Final(st).Value());
  } else if (GetFst().Final(st).Value() !=
	     StdArc::Weight::Zero().Value()) {
    p += exp(-GetFst().Final(st).Value());
  }
  return p;
}

// Show symbol during random string generation
NGramOutput::StateId NGramOutput::ShowRandSymbol(Label lbl,
						 bool *first_printed,
						 bool show_backoff,
						 StateId st) const {
  StateId hi_state = -1;
  if (lbl >= 0) {
    // only print epsilons if flag is set
    if (show_backoff || lbl != BackoffLabel()) {
      string symbol = GetFst().InputSymbols()->Find(lbl);
      if (*first_printed) {  // no space delimiter required
	(*first_printed) = false;
      } else {
	ostrm_ << " ";
      }
      ostrm_ << symbol;
    }
    if (lbl == BackoffLabel())
      hi_state = st;
  }
  return hi_state;
}

// Find random symbol and show if necessary
NGramOutput::StateId NGramOutput::GetAndShowSymbol(StateId st, double p,
						   double r,
						   StateId *hi_state,
						   bool *first_printed,
						   bool show_backoff) const {
  StateId nextstate = -1;
  if (p > r)
    return nextstate;
  Label lbl = -1;
  Matcher<StdFst> matcher(GetFst(), MATCH_INPUT);  // for querying hi_state
  if ((*hi_state) >= 0)
    matcher.SetState(*hi_state);
  for (ArcIterator<StdExpandedFst> aiter(GetFst(), st);
       !aiter.Done() && p <= r;
       aiter.Next()) {
    StdArc arc = aiter.Value();
    if (arc.ilabel != BackoffLabel() &&
	(*hi_state) >= 0 && matcher.Find(arc.ilabel))
      continue;  // ignore if label emitted from higher order state
    p += exp(-arc.weight.Value());
    lbl = arc.ilabel;
    nextstate = arc.nextstate;
  }
  (*hi_state) = ShowRandSymbol(lbl, first_printed, show_backoff, st);
  return nextstate;
}

// Produce and output random samples from model using rand/srand
void NGramOutput::RandNGramModel(int64 samples, bool show_backoff) const {
  int seed = time(0);  // initialize random number
  srand(seed);
  for (int i = 0; i < 1000; ++i)  // a bit of burn-in
    rand(); // TODO(riley): huh?
  for (int sample = 0; sample < samples; ++sample) {
    StateId st = GetFst().Start(), hi_state = -1, nextstate;
    Label lbl;
    bool first_printed = true;
    while (st >= 0) {
      double r = rand()/(RAND_MAX + 1.0);
      double p = SetInitRandProb(hi_state, st, &r);
      nextstate = -1;
      lbl = -1;
      st = GetAndShowSymbol(st, p, r, &hi_state,
			    &first_printed, show_backoff);
    }
    ostrm_ << '\n';
  }
}

// Checks parameterization of perplexity calculation and sets OOV_label
StdArc::Label NGramOutput::GetOOVLabel(double *OOV_probability,
				       string *OOV_symbol) {
  if ((*OOV_probability) < 0.0) {
    LOG(FATAL) << "OOV_probability must be greater than 0: " << 
      (*OOV_probability);
  }
  if ((*OOV_probability) >= 1.0) {
    LOG(FATAL) << "OOV_probability must be less than 1: " << 
      (*OOV_probability);
  }
  Label OOV_label = kSpecialLabel;  // default for OOV symbol
  if (!OOV_symbol->empty()) {  // if OOV symbol provided, find symbol
    OOV_label = GetFst().InputSymbols()->Find(*OOV_symbol);
    if (OOV_label < 0) {  // not found
      LOG(ERROR) << "Provided OOV symbol (" << (*OOV_symbol)
		 << ") not in model symbol table; default used" << OOV_label;
      OOV_label = kSpecialLabel;
    } else if ((*OOV_probability) != 0) {
      LOG(FATAL) <<
	"Cannot provide unigram probability for existing OOV label";
    } else {  // look for unigram probability of found OOV symbol
      double oov_cost = GetSymbolUnigramCost(OOV_label);
      if (oov_cost == StdArc::Weight::Zero().Value()) {
	LOG(ERROR) << "Provided OOV symbol (" << (*OOV_symbol)
		   << ") has no unigram probability; default symbol used";
	OOV_label = kSpecialLabel;
      } else {
	(*OOV_probability) = exp(-oov_cost);
      }
    }
  } else if ((*OOV_probability) == 0) {
    LOG(WARNING) << "OOV probability = 0; "
		 << "OOVs will be ignored in perplexity calculation";
  }
  return OOV_label;
}

// Adds a phi loop (rho) at unigram state for OOVs
// OOV_class_size (N) and OOV_probability (p) determine weight of loop: p/N
// Rest of unigrams renormalized accordingly, by 1-p
// If OOV symbol given, all OOV symbol probs are divided by class size
// and loop at unigram state given same prob as unigram of OOV symbol
void NGramOutput::RenormUnigramForOOV(Label special_label,
				      Label OOV_label,
				      double OOV_class_size,
				      double OOV_probability) {
  StateId st = UnigramState();
  if (st < 0) st = GetFst().Start();
  double OOV_neglogprob = OOV_probability > 0.0 ? 
    -log(OOV_probability / OOV_class_size) : StdArc::Weight::Zero().Value();
  if (OOV_label < 0 && OOV_probability > 0.0) {  // default OOV with prob
    double renorm = -log(1 - OOV_probability);  // renormalization constant
    for (MutableArcIterator<StdMutableFst> aiter(GetMutableFst(), st);
	 !aiter.Done();
	 aiter.Next()) {
      StdArc arc = aiter.Value();
      arc.weight = Times(arc.weight, renorm);  // removing OOV prob mass
      aiter.SetValue(arc);
    }
    RecalcBackoff();  // recalculate backoff weights to ensure normalization
  } else if (OOV_label >= 0) {  // OOV class label in model; 
    double renorm = log(OOV_class_size);  // spread class prob around members
    for (StateId ost = 0; ost < GetFst().NumStates(); ++ost) {
      MutableArcIterator<StdMutableFst> aiter(GetMutableFst(), ost);
      if (FindMutableArc(&aiter, OOV_label)) {
	StdArc arc = aiter.Value();
	arc.weight = Times(arc.weight, renorm);
	aiter.SetValue(arc);
      }
    }
  }
  GetMutableFst()->AddArc(st, StdArc(special_label, special_label,
				     OOV_neglogprob, st));
}

// Switch backoff label to special label for phi matcher
// assumed to be order preserving (as it is with <epsilon> and -2)
void NGramOutput::MakePhiMatcherLM(Label special_label) {
  for (StateId st = 0; st < GetFst().NumStates(); ++st) {
    if (GetFst().Final(st) == StdArc::Weight::Zero())  // need backoff final
      GetBackoffFinalCost(st);
  }
  for (StateId st = 0; st < GetFst().NumStates(); ++st) {
    MutableArcIterator<StdMutableFst> aiter(GetMutableFst(), st);
    if (FindMutableArc(&aiter, BackoffLabel())) {
      StdArc arc = aiter.Value();
      arc.ilabel = arc.olabel = special_label;
      aiter.SetValue(arc);
    }
  }
  ArcSort(GetMutableFst(), StdILabelCompare());
}

}  // namespace ngram
