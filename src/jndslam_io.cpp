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

#include "jndslam_io.h"

// Check if a file exists. Returns true if it does.
bool file_exists(std::string &filename)
{
  struct stat buffer;
  if (stat(filename.c_str(), &buffer) != -1)
  {
    return true;
  }
  return false;
}


// List the contents of a directory
std::vector<std::string> list_dir(std::string dir)
{
  std::vector<std::string> files;
  DIR *dr;
  struct dirent *dirp;
  if((dr  = opendir(dir.c_str())) == NULL)
  {
    std::cerr << "Error(" << errno << ") opening " << dir << "\n";
  }

  while ((dirp = readdir(dr)) != NULL)
  {
    // Check for files ending with ~
    std::string tmp = std::string(dirp->d_name);
    if (tmp.back() == '~')
    {
      continue;
    }
    else
    {
      files.push_back(tmp);
    }
  }
  closedir(dr);
  return files;
}

// Open a file line by line
std::vector<std::string> open_file(std::string filepath)
{
  //Lets first check if the file exists at all. If not we die.
  if (!file_exists(filepath))
  {
    throw std::invalid_argument("File does not exist: "+filepath);
  }
  std::ifstream f;
  f.open(filepath.c_str(), std::ifstream::in);
  std::string str;
  
  std::vector<std::string> out_vec;
  
  while (f.good())
  {
    std::getline(f, str);
    out_vec.push_back(str);
  }
  
  return out_vec;
}

// Parse an EST file and add the pitch information to all sylls in an utt.
void parse_est(typename utterance::utterance &utt, std::vector<std::string> &line_list)
{
  // For string splitting
  std::vector<std::string> tmp_vec;
  // For keeping time, voicing and pitch values
  std::vector<std::vector<float> > time_vec;
  std::vector<float> tmp_time_vec;
  
  // Go through each line and find timing
  // We start at int = 7 as we wish to ignore the header lines (the first 7 lines)
  for (int i = 7; i < line_list.size(); i++)
  {
    // Split it in times, voicing and pitch value
    tmp_vec = split_string(line_list[i], ' ',false);
    // Make sure we have enough values
    if (tmp_vec.size() == 3)
    {
      tmp_time_vec.clear();
      for (int j = 0; j < 3; j++)
      {
        tmp_time_vec.push_back(std::atof(tmp_vec[j].c_str()));
      }
      time_vec.push_back(tmp_time_vec);
    }
  }
  
  // Go through each syllable in utt and match with pitch values
  for (int i = 0; i < utt.sylls.size(); i++)
  {
    // We here assume a framesize of 5ms
    int start_frame = utt.sylls[i].start / 0.005;
    int end_frame = utt.sylls[i].end / 0.005;
    // The start should be floored only for the first syllable
    if (start_frame != 0)
    {
      start_frame += 1;
    }
    // Obtain the slice of the pitch values and add it to the syll
    utt.sylls[i].pitch_values = slice_vector(time_vec, start_frame, end_frame);
  }
}

// Parse a list of hts full-context phone labels and add syllables to an utt
void parse_hts_lab(typename utterance::utterance &utt, std::vector<std::string> &line_list, std::vector<std::string> &delims)
{
  //  Must be of right size
  if (delims.size() != 4)
  {
    throw std::invalid_argument("Wrong number of delimiters for parse_hts_lab. Exiting.");
  }
  
  // For string splitting
  std::vector<std::string> tmp_vec;
  
  // For storing tmp syll info
  float syll_start;
  float syll_end;
  std::string syll_id;
  
  std::string phone_id;
  std::string phone_pos_beg;
  std::string phone_pos_end;
  int phone_start;
  int phone_end;
  
  
  // Go through each line and get the relevant details for each phone
  for (int i = 0; i < line_list.size(); i++)
  {
    // Split it in times and context
    tmp_vec = split_string(line_list[i]);
    // Just a silly check to make sure we're not trying something weird... which we are. this also gets rid of empty lines.
    if (tmp_vec.size() != 3)
    {
      continue;
    }
    // Get phone start and end numbers
    phone_start = std::atoi(tmp_vec[0].c_str());
    phone_end = std::atoi(tmp_vec[1].c_str());
    // Get the phone id
    std::string tmp_str = tmp_vec[2];
    int start = tmp_str.find(delims[0]);
    int end = tmp_str.find(delims[1]);
    phone_id = tmp_str.substr(start+delims[1].size(), end-start-1);
    // Get the syllable numbers
    start = tmp_str.find(delims[2]);
    end = tmp_str.find(delims[3]);
    tmp_str = tmp_str.substr(start+delims[1].size(), end-start-1);
    phone_pos_beg = tmp_str.substr(0, 1);
    // We need the size thing here because of the 'xx' for silences
    phone_pos_end = tmp_str.substr(tmp_str.size()-1, 1);
    // Make a syllable
    // If we're at the beginning of one
    if (phone_pos_beg == "1" || phone_pos_beg == "x")
    {
      syll_start = (float)phone_start/10000000.0;
      syll_id = "";
    }
    syll_id += phone_id;
    // If we're at the end of one
    if (phone_pos_end == "1" || phone_pos_end == "x")
    {
      syll_end = (float)phone_end/10000000.0;
      typename syllable::syllable syll = *new typename syllable::syllable(syll_start, syll_end, syll_id);
      utt.sylls.push_back(syll);
    }
  }
}

// Parse a simple space-delimited label list and add segments to an utterance
// Format for each line is:
// start_time_in_seconds end_time_in_seconds segment_name
void parse_simple_lab(typename utterance::utterance &utt, std::vector<std::string> &line_list)
{
  // For string splitting
  std::vector<std::string> tmp_vec;
  
  // For storing tmp segment info
  float segment_start;
  float segment_end;
  
  // Go through each line and get the relevant details for each phone
  for (int i = 0; i < line_list.size(); i++)
  {
    // Split it in times and context
    tmp_vec = split_string(line_list[i], ' ');
    // Just a check to make sure we have the correct number of items.
    if (tmp_vec.size() != 3)
    {
      // We don't want to make a fuss if it is just an empty line.
      if (line_list[i] == "")
      {
        continue;
      }
      else
      {
        std::cout << "WARNING! Line " << i+1 << " in " << utt.name << " is malformed. Skipping..." << std::endl;
        continue;
      }
    }
    // Get phone start and end time
    segment_start = std::atof(tmp_vec[0].c_str());
    segment_end = std::atof(tmp_vec[1].c_str());
    typename syllable::syllable syll = *new typename syllable::syllable(segment_start, segment_end, tmp_vec[2]);
    utt.sylls.push_back(syll);
  }
}


// Write out a file for each utterance with stylisations of each syllable line by line
void write_utts_to_file(std::vector<typename utterance::utterance> &utts,  std::string &out_path, Style_Alg &algorithm)
{
  for (int i; i < utts.size(); i++)
  {
    write_utt_to_file(utts[i], out_path, algorithm);
  }
}

// Write out a file for a utterance with stylisations of each syllable line by line
void write_utt_to_file(typename utterance::utterance &utt, std::string &out_path, Style_Alg &algorithm)
{
  std::ofstream out_file;
  // Note this won't work on windows. But I refuse to add e.g. Boost as a dependency just for safe path joining abilities.
  out_file.open(out_path + utt.name + ".sty");
  for (int i; i < utt.sylls.size(); i++)
  {
    typename syllable::syllable *tmp_syll = &utt.sylls.at(i);
    out_file << tmp_syll->start << " ";
    out_file << tmp_syll->end << " ";
    out_file << tmp_syll->identity << " ";
    out_file << tmp_syll->contour_start << " ";
    out_file << tmp_syll->contour_direction << " ";
    if (algorithm == RAW)
    {
      out_file << tmp_syll->contour_extreme << " ";
      out_file << tmp_syll->contour_extreme_pos << std::endl;
    }
    else
    {
      out_file << tmp_syll->contour_extreme << std::endl;
    }
  }
}
