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
void stylise(std::vector<typename utterance::utterance> &utts, Style_Alg algorithm)
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
      
      if (algorithm == SIMPLIFIED)
      {
        style_simplified(*tmp_syll);
      }
      else if (algorithm == JNDSLAM)
      {
        style_jndslam(*tmp_syll);
      }
      else if (algorithm == SLAM)
      {
        style_slam(*tmp_syll);
      }
      else
      {
        throw std::invalid_argument("This should never happen! The enum is - "+std::to_string(algorithm)+" - but it is not an option!");
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
    //std::cout << "Mean pitch is " << sum/count << "\n";
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

void style_simplified(typename syllable::syllable &syll)
{
  // Apply label to start position
  float start_pitch = syll.pitch_values.front()[2];
  if (start_pitch >= 1.5)
  {
    syll.contour_start = "HIGH";
  }
  else if (start_pitch > -1.5)
  {
    syll.contour_start = "MEDIUM";
  }
  else
  {
    syll.contour_start = "LOW";
  }
  
  // Apply label to direction
  float direction_value = syll.pitch_values.back()[2] - syll.pitch_values.front()[2];
  if (direction_value >= 1.5)
  {
    syll.contour_direction = "UP";
  }
  else if (direction_value > -1.5)
  {
    syll.contour_direction = "STRAIGHT";
  }
  else
  {
    syll.contour_direction = "DOWN";
  }
  
  // Apply label to extreme
  float max = -1000;
  int max_pos = -1;
  float min = 1000;
  int min_pos = -1;
  float extreme_val = 0;
  float extreme_pos = -1;
  // Get max/min values and pos
  for (int i = 0; i < syll.pitch_values.size(); i++)
  {
    if (syll.pitch_values.at(i)[2] > max)
    {
      max = syll.pitch_values.at(i)[2];
      max_pos = i;
    }
    if (syll.pitch_values.at(i)[2] < min)
    {
      min = syll.pitch_values.at(i)[2];
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
  float beg_diff = extreme_val - syll.pitch_values.front()[2];
  float end_diff = extreme_val - syll.pitch_values.back()[2];
  
  // If an extreme is exactly at the end or beginning there is no extreme
  if (extreme_pos == 0 || extreme_pos == syll.pitch_values.size() - 1)
  {
    syll.contour_extreme = "NO_EXTREME";
  }
  else if (abs(beg_diff) < abs(end_diff)) // Else are we closer to the beginning than end?
  {
    // If the diff is above 1.5 a positive extreme exists
    if (beg_diff >= 1.5)
    {
      syll.contour_extreme = "POSITIVE";
    }
    else if (beg_diff <= -1.5) // If below -1.5 a negative
    {
      syll.contour_extreme = "NEGATIVE";
    }
    else // Else none exist
    {
      syll.contour_extreme = "NO_EXTREME";
    }
  }
  else // We are closer to the end
  {
    // If the diff is above 1.5 a positive extreme exists
    if (end_diff >= 1.5)
    {
      syll.contour_extreme = "POSITIVE";
    }
    else if (end_diff <= -1.5) // If below -1.5 a negative
    {
      syll.contour_extreme = "NEGATIVE";
    }
    else // Else none exist
    {
      syll.contour_extreme = "NO_EXTREME";
    }
  }
}

void style_jndslam(typename syllable::syllable &syll)
{
  // Apply label to start position
  syll.contour_start = semitone_to_register(syll.pitch_values.front()[2], 1.5);
  
  // Apply label to direction
  float direction_value = syll.pitch_values.back()[2] - syll.pitch_values.front()[2];
  if (direction_value >= 4.5)
  {
    syll.contour_direction = "VERY_UP";
  }
  if (direction_value >= 1.5)
  {
    syll.contour_direction = "UP";
  }
  else if (direction_value > -1.5)
  {
    syll.contour_direction = "STRAIGHT";
  }
  else if (direction_value > -4.5)
  {
    syll.contour_direction = "DOWN";
  }
  else
  {
    syll.contour_direction = "VERY_DOWN";
  }
  
  // Apply label to extreme
  float max = -1000;
  int max_pos = -1;
  float min = 1000;
  int min_pos = -1;
  float extreme_val = 0;
  float extreme_pos = -1;
  // Get max/min values and pos
  for (int i = 0; i < syll.pitch_values.size(); i++)
  {
    if (syll.pitch_values.at(i)[2] > max)
    {
      max = syll.pitch_values.at(i)[2];
      max_pos = i;
    }
    if (syll.pitch_values.at(i)[2] < min)
    {
      min = syll.pitch_values.at(i)[2];
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
  float beg_diff = extreme_val - syll.pitch_values.front()[2];
  float end_diff = extreme_val - syll.pitch_values.back()[2];
  
  
  std::string extreme = "";
  // Find position in syllable
  float pos = (float)extreme_pos / (float)syll.pitch_values.size();
  if (pos >= 0.7)
  {
    extreme += "END_";
  }
  else if (pos <= 0.3)
  {
    extreme += "BEGINNING_";
  }
  else
  {
    extreme += "MIDDLE_";
  }
  
  // If an extreme is exactly at the end or beginning there is no extreme
  if (extreme_pos == 0 || extreme_pos == syll.pitch_values.size() - 1)
  {
    extreme = "NO_EXTREME";
  }
  else if (abs(beg_diff) < abs(end_diff)) // Else are we closer to the beginning than end?
  {
    // If the diff is above 1.5 a positive extreme exists
    if (beg_diff >= 1.5)
    {
      extreme += "POSITIVE";
    }
    else if (beg_diff <= -1.5) // If below -1.5 a negative
    {
      extreme += "NEGATIVE";
    }
    else // Else none exist
    {
      extreme = "NO_EXTREME";
    }
  }
  else // We are closer to the end
  {
    // If the diff is above 1.5 a positive extreme exists
    if (end_diff >= 1.5)
    {
      extreme += "POSITIVE";
    }
    else if (end_diff <= -1.5) // If below -1.5 a negative
    {
      extreme += "NEGATIVE";
    }
    else // Else none exist
    {
      extreme = "NO_EXTREME";
    }
  }
  // Add the final contour
  syll.contour_extreme = extreme;
}

void style_slam(typename syllable::syllable &syll)
{
  // Apply label to start position
  syll.contour_start = semitone_to_register(syll.pitch_values.front()[2], 2);
  
  
  // Apply label to end position
  syll.contour_direction = semitone_to_register(syll.pitch_values.back()[2], 2);
  
  // Apply label to extreme
  float max = -1000;
  int max_pos = -1;
  float min = 1000;
  int min_pos = -1;
  float extreme_val = 0;
  float extreme_pos = -1;
  // Get max/min values and pos
  for (int i = 0; i < syll.pitch_values.size(); i++)
  {
    if (syll.pitch_values.at(i)[2] > max)
    {
      max = syll.pitch_values.at(i)[2];
      max_pos = i;
    }
    if (syll.pitch_values.at(i)[2] < min)
    {
      min = syll.pitch_values.at(i)[2];
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
  float beg_diff = extreme_val - syll.pitch_values.front()[2];
  float end_diff = extreme_val - syll.pitch_values.back()[2];
  
  std::string extreme = "";
  // Find position in syllable
  float pos = (float)extreme_pos / (float)syll.pitch_values.size();
  if (pos >= 0.7)
  {
    extreme += "END_";
  }
  else if (pos <= 0.3)
  {
    extreme += "BEGINNING_";
  }
  else
  {
    extreme += "MIDDLE_";
  }
  
  // If an extreme is exactly at the end or beginning there is no extreme
  if (extreme_pos == 0 || extreme_pos == syll.pitch_values.size() - 1)
  {
    extreme = "NO_EXTREME";
  }
  else if (abs(beg_diff) < abs(end_diff)) // Else are we closer to the beginning than end?
  {
    // If the diff is above 2 an extreme exists
    if (abs(beg_diff) >= 2)
    {
      extreme += semitone_to_register(beg_diff, 2);
    }
    else // Else none exist
    {
      extreme = "NO_EXTREME";
    }
  }
  else // We are closer to the end
  {
    // If the diff is above 1.5 a positive extreme exists
    if (abs(end_diff) >= 1.5)
    {
      extreme += semitone_to_register(end_diff, 2);
    }
    else // Else none exist
    {
      extreme = "NO_EXTREME";
    }
  }
  // Add the final contour
  syll.contour_extreme = extreme;
}

// Converts a semitone value into a 5 level register based on the split value
std::string semitone_to_register(float semitone, float split)
{
  if (semitone >= split*3)
  {
    return "VERY_HIGH";
  }
  else if (semitone >= split)
  {
    return "HIGH";
  }
  else if (semitone > -split)
  {
    return "MEDIUM";
  }
  else if (semitone > -split*3)
  {
    return "LOW";
  }
  else
  {
    return "VERY_LOW";
  }
}
