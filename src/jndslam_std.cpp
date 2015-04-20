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

#include "jndslam_std.h"

// Split a string on a delimiter. Optionally include empty splits, whitespace is default delimiter.
std::vector<std::string> split_string(std::string str, char delim, bool empty)
{
  std::stringstream ss(str);
  std::string tmp;
  std::vector<std::string> out_vec;
  
  while (std::getline(ss, tmp, delim))
  {
    if (empty == true || !tmp.empty())
    {
      out_vec.push_back(tmp);
    }
  }
  
  return out_vec;
}


// Get a slice of a vector
// Ignores values out of range with a warning.
std::vector<std::vector<float> > slice_vector(std::vector<std::vector<float> > &vec, int start, int end)
{
  std::vector<std::vector<float> > out_vec;
  
  for (int i = start; i <= end; i++)
  {
    try
    {
      out_vec.push_back(vec.at(i));
    }
    catch (const std::out_of_range &oor)
    {
      std::cerr << "WARNING! Out of Range when getting slice from " << start << " to " << end << " at " << i << " Skipping..\n";
    }
  }
  
  return out_vec;
}

