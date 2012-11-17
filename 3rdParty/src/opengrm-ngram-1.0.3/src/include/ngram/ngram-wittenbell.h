// ngram-wittenbell.h
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
// Witten-Bell derived class for smoothing

#ifndef NGRAM_NGRAM_WITTENBELL_H__
#define NGRAM_NGRAM_WITTENBELL_H__

#include <vector>

#include<ngram/ngram-make.h>

namespace ngram {

class NGramWittenBell : public NGramMake {
 public:
  // Construct NGramMake object, consisting of the FST and some
  // information about the states under the assumption that the FST is a model.
  // Ownership of the FST is retained by the caller.
  NGramWittenBell(StdMutableFst *infst, 
		  bool backoff, Label backoff_label = 0,
		  double norm_eps = kNormEps, bool check_consistency = false,
		  double parameter = 0.0)
    : NGramMake(infst, backoff, backoff_label, norm_eps, check_consistency),
      parameter_(parameter) {}

    // Smooth model according to 'method' and parameters.
    void MakeNGramModel() {
      NGramMake::MakeNGramModel();
    }
  
    // No discount, hence hi order mass is count
    double CalculateHiOrderMass(vector<double> *discounts, double nlog_count) {
      return nlog_count;
    }

    // Return Normalization constant for Witten Bell:
    // -log c(h) + K |{w:c(hw)>0}|
    double CalculateTotalMass(double nlog_count, StateId st) {
      double ngcount = GetFst().NumArcs(st) - 1;
      if (GetFst().Final(st).Value() != StdArc::Weight::Zero().Value())
	++ngcount;  // count </s> if p > 0
      // Count mass allocated to lower order estimates: K |{w:c(hw)>0}|
      double low_order_mass = -log(ngcount) - log(parameter_);
      return NegLogSum(nlog_count, low_order_mass);
    }

 private:
  double parameter_;  // Witten-Bell K
  DISALLOW_COPY_AND_ASSIGN(NGramWittenBell);
};


}  // namespace ngram

#endif  // NGRAM_NGRAM_WITTENBELL_H__
