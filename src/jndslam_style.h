/* Copyright 2015 Rasmus Dall - rasmus@dall.dk

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#ifndef JNDSLAM_STYLE_H
#define JNDSLAM_STYLE_H

#include <vector>
#include <iostream>
#include <stdexcept>

#include <math.h>

#include "utterance.h"
#include "syllable.h"

// The possible algorithms
enum Style_Alg {SIMPLIFIED, JNDSLAM};

// Stylise all syllables in a list of utterances
void stylise(std::vector<typename utterance::utterance> &utts, Style_Alg algorithm=SIMPLIFIED);

// Calculate the mean pitch of the speaker
float calc_mean_pitch(std::vector<typename utterance::utterance> &utts);

// Convert a pitch values to its semitone difference from a reference
float f0_to_semitone(float &f0, float &mean_f0);

// Style a syllable using Simplified JNDSLAM
void style_simplified(typename syllable::syllable &syll);

// Style a syllable using JNDSLAM
void style_jndslam(typename syllable::syllable &syll);

#endif
