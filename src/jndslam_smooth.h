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

#ifndef JNDSLAM_SMOOTH_H
#define JNDSLAM_SMOOTH_H

#include <vector>
#include <array>
#include <stdexcept>
#include <algorithm>
#include <iostream>

#include <math.h>

#include "utterance.h"

// Smoothing using LOWESS
// This is closely based on the following C# implementation http://stackoverflow.com/questions/5651709/c-sharp-loess-lowess-regression
// But adapted to current use. This also explains the insistence on the use of doubles over floats which isn't really necessary.
// This modifies the current pitch values in pitchs, the smoothing span must be between 1 and 0. Current default value follows implementation
// in https://github.com/jbeliao/SLAM . It is low compared to C# implementation recommendation of 0.25 to 0.5. Its default is 0.3.
// The bigger the span the larger a smoothing window.
// Iters is the number of robustness iterations, default based on other implementations.
// Ignore_unvoiced is implementation specific and if true the distance between unvoiced sections is not taken into account when smoothing.
// Default is set to false as we wish this distance (in ms) to matter. However the original SLAM implementation in https://github.com/jbeliao/SLAM
// does not do this as far as I can see (so would be equivalent to set to true).
void smooth(std::vector<std::vector<float> > &pitchs, float smoothing_span=0.2, unsigned int iters=3, bool ignore_unvoiced=false);

void smooth_utt(typename utterance::utterance &utt);

void smooth_utts(std::vector<typename utterance::utterance> &utts);

static double tricube(double x);

#endif
