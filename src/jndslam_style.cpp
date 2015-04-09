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

#include "jndslam_style.h"

// Stylise all syllables in a list of utterances
// Note that we assume unvoiced segments have already been removed
void stylise(std::vector<typename utterance::utterance> &utts)
{
  // Get the mean f0 of the speaker
  float mean_pitch = calc_mean_pitch(utts);
  // For each utterance
  for (int i = 0; i < utts.size(); i++)
  {
    typename utterance::utterance *tmp_utt = &utts[i];
    // For each syllable in the utterance
    for (int j = 0; j < tmp_utt->sylls.size(); j++)
    {
      typename syllable::syllable *tmp_syll = &tmp_utt->sylls[j];
      
      // If the syllable has less than 3 voiced frames we can assign unvoiced and continue to next
      if (tmp_syll->pitch_values.size() <= 3)
      {
        tmp_syll->contour_start = "UNVOICED_START";
        tmp_syll->contour_direction = "UNVOICED_DIRECTION";
        tmp_syll->contour_extreme = "UNVOICED_EXTREME";
        continue;
      }
      
      // For each pitch value in the syllable
      // Convert f0 values to semitones around the mean
      for (int z = 0; z < tmp_syll->pitch_values.size(); z++)
      {
        if (tmp_syll->pitch_values[z][1] == 1)
        {
          //std::cout << "Prior " << tmp_syll->pitch_values[z][2];
          tmp_syll->pitch_values[z][2] = f0_to_semitone(tmp_syll->pitch_values[z][2], mean_pitch);
          //std::cout << " post " << tmp_syll->pitch_values[z][2];
          //std::cout << " mean " << mean_pitch << "\n";
        }
      }
      
      // Apply label to start position
      float start_pitch = tmp_syll->pitch_values.front()[2];
      if (start_pitch >= 1.5)
      {
        tmp_syll->contour_start = "HIGH";
      }
      else if (start_pitch > -1.5)
      {
        tmp_syll->contour_start = "MEDIUM";
      }
      else
      {
        tmp_syll->contour_start = "LOW";
      }
      
      // Apply label to direction
      float direction_value = tmp_syll->pitch_values.back()[2] - tmp_syll->pitch_values.front()[2];
      if (direction_value >= 1.5)
      {
        tmp_syll->contour_direction = "UP";
      }
      else if (direction_value > -1.5)
      {
        tmp_syll->contour_direction = "STRAIGHT";
      }
      else
      {
        tmp_syll->contour_direction = "DOWN";
      }
      
      // Apply label to extreme
      float max = -1000;
      int max_pos = -1;
      float min = 1000;
      int min_pos = -1;
      float extreme_val = 0;
      float extreme_pos = -1;
      // Get max/min values and pos
      for (int i = 0; i < tmp_syll->pitch_values.size(); i++)
      {
        if (tmp_syll->pitch_values.at(i)[2] > max)
        {
          max = tmp_syll->pitch_values.at(i)[2];
          max_pos = i;
        }
        if (tmp_syll->pitch_values.at(i)[2] < min)
        {
          min = tmp_syll->pitch_values.at(i)[2];
          min_pos = i;
        }
      }
      // Find largest of max/min
      if (abs(min) > max)
      {
        extreme_val = min;
        extreme_pos = min_pos;
      }
      else
      {
        extreme_val = max;
        extreme_pos = max_pos;
      }
      
      // Find if extreme is closer to beginning or end
      float beg_diff = extreme_val - tmp_syll->pitch_values.front()[2];
      float end_diff = extreme_val - tmp_syll->pitch_values.back()[2];
      
      // If an extreme is exactly at the end or beginning there is no extreme
      if (extreme_pos == 0 || extreme_pos == tmp_syll->pitch_values.size() - 1)
      {
        tmp_syll->contour_extreme = "NOEXTREME";
      }
      else if (abs(beg_diff) < abs(end_diff)) // Else are we closer to the beginning than end?
      {
        // If the diff is above 1.5 a positive extreme exists
        if (beg_diff >= 1.5)
        {
          tmp_syll->contour_extreme = "POSITIVE";
        }
        else if (beg_diff <= -1.5) // If below -1.5 a negative
        {
          tmp_syll->contour_extreme = "NEGATIVE";
        }
        else // Else none exist
        {
          tmp_syll->contour_extreme = "NOEXTREME";
        }
      }
      else // We are closer to the end
      {
        // If the diff is above 1.5 a positive extreme exists
        if (end_diff >= 1.5)
        {
          tmp_syll->contour_extreme = "POSITIVE";
        }
        else if (end_diff <= -1.5) // If below -1.5 a negative
        {
          tmp_syll->contour_extreme = "NEGATIVE";
        }
        else // Else none exist
        {
          tmp_syll->contour_extreme = "NOEXTREME";
        }
      }
    }
  }
}

// Calculate the mean pitch of the speaker
float calc_mean_pitch(std::vector<typename utterance::utterance> &utts)
{
  // How many pitch values are we adding together?
  int count = 0;
  // The sum of all pitch values
  float sum = 0;
  
  // For each utterance
  for (int i = 0; i < utts.size(); i++)
  {
    typename utterance::utterance *tmp_utt = &utts[i];
    // For each syllable in the utterance
    for (int j = 0; j < tmp_utt->sylls.size(); j++)
    {
      typename syllable::syllable *tmp_syll = &tmp_utt->sylls[j];
      // For each pitch value in the syllable
      if (tmp_syll->pitch_values.size() > 0)
      {
        for (int z = 0; z < tmp_syll->pitch_values.size(); z++)
        {
          if (tmp_syll->pitch_values[z][1] == 1)
          {
            sum = sum + tmp_syll->pitch_values[z][2];
            count += 1;
          }
        }
      }
    }
  }
  
  if (count != 0)
  {
    std::cout << "Mean pitch is " << sum/count << "\n";
    return sum/count;
  }
  else
  {
    return 0;
  }
}

float f0_to_semitone(float &f0, float &mean_f0)
{
  float semitones = 12*log2(f0/mean_f0);
  return semitones;
}
