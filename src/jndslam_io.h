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

#ifndef JNDSLAM_IO_H
#define JNDSLAM_IO_H

#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <fstream>

#include <dirent.h>
#include <sys/stat.h>

#include "utterance.h"
#include "jndslam_std.h"

// Checks if a file exists
bool file_exists(std::string &filename);

// List the contents of a directory
std::vector<std::string> list_dir(std::string dir);

// Opens a file line by line. Throws if file does not exist.
std::vector<std::string> open_file(std::string filepath);

// Parse an EST style file opened line by line and add timing information
// to sylls in an utt.
// Note that sylls must already exist for this to work.
void parse_est(typename utterance::utterance &utt, std::vector<std::string> &line_list);

// Parse an HTS style label list and add sylls to an utterance
void parse_hts_lab(typename utterance::utterance &utt, std::vector<std::string> &line_list);

// Parse a simple space-delimited label list and add sylls to an utterance
// Format for each line is:
// start_time_in_seconds end_time_in_seconds segment_id
void parse_simple_lab(typename utterance::utterance &utt, std::vector<std::string> &line_list);

// Write out a file for each utterance with stylisations of each syllable line by line
void write_utts_to_file(std::vector<typename utterance::utterance> &utts, std::string &out_path);

// Write out a file for a utterance with stylisations of each syllable line by line
void write_utt_to_file(typename utterance::utterance &utts, std::string &out_path);

#endif
