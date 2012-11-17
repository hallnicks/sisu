// ngram-countprune.cc
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

#include <sstream>
#include <ngram/ngram-countprune.h>

namespace ngram {

using std::stringstream;

// Reads from string while token is a numerical value
template <class A>
char NGramCountPrune::GetNextCharVal(string::const_iterator *strit, A *toget,
				     const string &count_pattern) const {
  char c = GetNextChar(strit);
  string tok;
  while (IsInNumber(c)) {
    tok += c;
    c = GetNextChar(strit);
  }
  if (tok == "")
    LOG(FATAL) << "NGramCountPrune: Count pruning parameter format error: "
	       << count_pattern;
  stringstream tok_ss(tok);
  tok_ss >> (*toget);
  return c;
}

// Derive count minimums from input count pruning string
// Expected format: "X(+):Y;Z(+):W"  X,Z are n-gram orders
// '+' optional designation for >= order and Y,W are count minimums
// ':' delimits prior to count minimum; ';' delimits fields
// example: "2:2;3+:3" signifies:
//   prune bigrams with count < 2; trigrams and above with count < 3
void NGramCountPrune::ParseCountMinimums(const string &count_pattern) {
  string:: const_iterator strit = count_pattern.begin();
  while (strit < count_pattern.end()) {
    int order;
    double count;
    char c = GetNextCharVal(&strit, &order, count_pattern);
    bool plus = false;
    if (c == '+') {
      plus = true;
      c = GetNextChar(&strit);
    }
    if (c != ':')
      LOG(FATAL) << "NGramShink: Count pruning parameter format error: "
		 << count_pattern;
    c = GetNextCharVal(&strit, &count, count_pattern);
    if (c != ';' && strit < count_pattern.end())
      LOG(FATAL) << "NGramShink: Count pruning parameter format error: "
		 << count_pattern;
    if (count <= 0) count = StdArc::Weight::Zero().Value();
    else count = log(count);
    if (order >= 0 && order <= HiOrder())
      UpdateCountMinimums(order, count, plus);
  }
}

// Updates count minimums for order, based on parsed parameter string
void NGramCountPrune::UpdateCountMinimums(int order, double count, bool plus) {
  if (order > 0 && count_minimums_[order - 1] < count)
    count_minimums_[order - 1] = count;
  if (plus) {
    for (int i = order; i < HiOrder(); ++i) {
      if (count_minimums_[i] < count)
	count_minimums_[i] = count;
    }
  }
}

}  // namespace ngram
