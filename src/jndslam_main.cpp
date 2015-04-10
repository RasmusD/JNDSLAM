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

void usage()
{
  std::cout << "Usage:" << std::endl;
  std::cout << "-s/--nosmooth\tDo not smooth input f0 values." << std::endl;
  std::cout << "-l/--labdir\tSpecify a custom .lab location. Default: data/lab/ NOTE! Dirs are not checked for correctness! This is not safe currently be careful!" << std::endl;
  std::cout << "-p/--pitchdir\tSpecify a custom .f0 location. Default: data/pitch/ NOTE! Dirs are not checked for correctness! This is not safe currently be careful!" << std::endl;
  std::cout << "-0/--outdir\tSpecify a custom output location. Default: data/out/ NOTE! Dirs are not checked for correctness! This is not safe currently be careful!" << std::endl;
  std::cout << "-h/--help\tPrint this message." << std::endl;
  std::exit(0);
}

int main(int argc, char *argv[])
{
  // Global argument container
  struct global_args_t {
    bool smoothing;              // Are we smoothing?
    std::string lab_path;        // Where can we find the .lab files?
    std::string pitch_path;      // Where can we find the .f0 files?
    std::string out_path;      // Where shall we put the output files?
  } global_args;
  
  // Initialise global args to defaults
  global_args.smoothing = true;
  global_args.lab_path = "data/lab/";
  global_args.pitch_path = "data/pitch/";
  global_args.out_path = "data/out/";
  
  // Long options
  static const struct option long_opts[] = {
    { "nosmooth", no_argument, NULL, 's' }, // Do not perform smoothing
    { "labdir", required_argument, NULL, 'l' }, // New lab dir
    { "pitchdir", required_argument, NULL, 'p' }, // New pitch dir
    { "outdir", required_argument, NULL, 'o' }, // New out dir
    { "help", no_argument, NULL, 'h' } // Show usage()
  };
  
  // Short options
  static const char *opt_string = "sl:p:o:h";
  
  // Parse command line options
  // cplusplus.com is down today so can't see their getopts tutorial.
  // Will change to use getopts later.
  int long_index = 0;
  int opt = getopt_long(argc, argv, opt_string, long_opts, &long_index);
  while (opt != -1)
  {
    switch (opt)
    {
      case 's':
        global_args.smoothing = false;
        break;
      case 'l':
        global_args.lab_path = std::string(optarg);
        break;
      case 'p':
        global_args.pitch_path = std::string(optarg);
        break;
      case 'o':
        global_args.out_path = std::string(optarg);
        break;
      case 'h':
      default:
        usage();
        break;
    }
    
    opt = getopt_long(argc, argv, opt_string, long_opts, &long_index);
  }
  
  // Get a list of files
  std::vector<std::string> lab_dir = list_dir(global_args.lab_path);
  std::vector<std::string> pitch_dir = list_dir(global_args.pitch_path);
  
  // Prepare list of opened files
  std::vector<std::vector<std::string> > lab_files;
  std::vector<std::vector<std::string> > pitch_files;
  
  // Ready a list of utts
  std::vector<typename utterance::utterance> utts;
  
  // Match each lab file to a pitch file
  for (int i = 0; i < lab_dir.size(); i++)
  {
    struct stat s;
    if (stat((global_args.lab_path+lab_dir[i]).c_str(), &s) == 0)
    {
      // Is this a file?
      if(s.st_mode & S_IFREG)
      {
        // Open it
        lab_files.push_back(open_file(global_args.lab_path+lab_dir[i]));
        // Find the basename
        std::string basename = split_string(lab_dir[i], '.')[0];
        // Try and open the corresponding pitch file.
        std::string pitch_file_path = global_args.pitch_path+basename+".f0";
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
  
  // Smooth pitch for each segment if applicable
  if (global_args.smoothing)
  {
    smooth_utts(utts);
  }
  else
  {
    remove_unvoiced(utts);
  }
  
  // Stylise syllables
  stylise(utts);
  
  // Write output stylisation
  write_utts_to_file(utts, global_args.out_path);
  
  // Add style info to HTS lab and write lab
  // TODO
  // add_style_to_lab()
  // write_file()
  
  return 0;
}
