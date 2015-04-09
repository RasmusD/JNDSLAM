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

#include "jndslam_main.h"

int main(int argc, char *argv[])
{
  // Open labs and pitch dirs
  std::string lab_path = "data/lab/";
  std::string pitch_path = "data/pitch/";
  
  // Get a list of files
  std::vector<std::string> lab_dir = list_dir(lab_path);
  std::vector<std::string> pitch_dir = list_dir(pitch_path);
  
  // Prepare list of opened files
  std::vector<std::vector<std::string> > lab_files;
  std::vector<std::vector<std::string> > pitch_files;
  
  // Ready a list of utts
  std::vector<typename utterance::utterance> utts;
  
  // Match each lab file to a pitch file
  for (int i = 0; i < lab_dir.size(); i++)
  {
    struct stat s;
    if (stat((lab_path+lab_dir[i]).c_str(), &s) == 0)
    {
      // Is this a file?
      if(s.st_mode & S_IFREG)
      {
        // Open it
        lab_files.push_back(open_file(lab_path+lab_dir[i]));
        // Find the basename
        std::string basename = split_string(lab_dir[i], '.')[0];
        // Try and open the corresponding pitch file.
        std::string pitch_file_path = pitch_path+basename+".f0";
        pitch_files.push_back(open_file(pitch_file_path));
        // If we found and opened the pitch file proceed and make an utterances
        typename utterance::utterance* utt = new typename utterance::utterance(basename);
        utts.push_back(*utt);
      }
    }
  }
  
  // Add syllable info to utterance from HTS lab file
  for (int i = 0; i < utts.size(); i++)
  {
    parse_hts_lab(utts[i], lab_files[i]);
  }
  
  // Add pitch information to utterance from EST file
  for (int i = 0; i < utts.size(); i++)
  {
    parse_est(utts[i], pitch_files[i]);
  }
  
  // Smooth pitch for each segment
  // TODO Unfinishes
  smooth_utts(utts);
  
  // Stylise syllables
  stylise(utts);
  
  // Write output stylisation
  write_utts_to_file(utts);
  
  // Add style info to HTS lab and write lab
  // TODO
  // add_style_to_lab()
  // write_file()
  
  return 0;
}
