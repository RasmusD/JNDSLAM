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

#include "jndslam_smooth.h"

// Dummy smoothing that simply removes unvoiced segments
void smooth(std::vector<std::vector<float> > &pitchs)
{
  std::vector<std::vector<float> > new_pitchs;
  
  for (int i = 0; i < pitchs.size(); i++)
  {
    if (pitchs[i][1] == 1)
    {
      new_pitchs.push_back(pitchs[i]);
    }
  }
  
  pitchs.swap(new_pitchs);
}

// Smooth all sylls in an utt
void smooth_utt(typename utterance::utterance &utt)
{
  for (int i = 0; i < utt.sylls.size(); i++)
  {
    smooth(utt.sylls[i].pitch_values);
  }
}

// Smooth all sylls in all utts in a list of utts
void smooth_utts(std::vector<typename utterance::utterance> &utts)
{
  for (int i = 0; i < utts.size(); i++)
  {
    smooth_utt(utts[i]);
  }
}
