// ngram-kneserney.h
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
// 
// Kneser-Ney derived class for smoothing

#ifndef NGRAM_NGRAM_KNESERNEY_H__
#define NGRAM_NGRAM_KNESERNEY_H__

#include <vector>

#include<ngram/ngram-absolute.h>

namespace ngram {

class NGramKneserNey : public NGramAbsolute {
 public:
  // Construct NGramKneserNey object, consisting of the FST and some
  // information about the states under the assumption that the FST is a model.
  // Ownership of the FST is retained by the caller.
  NGramKneserNey(StdMutableFst *infst, 
		 bool backoff, Label backoff_label = 0,
		 double norm_eps = kNormEps, bool check_consistency = false,
		 double parameter = 0.0, int bins = -1)
    : NGramAbsolute(infst, backoff, backoff_label, norm_eps, 
		    check_consistency, parameter, bins) {}

  // Smooth model according to 'method' and parameters.
  void MakeNGramModel();

 private:
  // Update arc and final values, either initializing or incrementing
  void UpdateKneserNeyCounts(StateId st, bool increment);

  // Modify lower order counts according to Kneser Ney formula
  void AssignKneserNeyCounts();

  DISALLOW_COPY_AND_ASSIGN(NGramKneserNey);
};


}  // namespace ngram

#endif  // NGRAM_NGRAM_KNESERNEY_H__
