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

#ifndef JNDSLAM_MAIN_H
#define JNDSLAM_MAIN_H

#include <getopt.h>

#include "jndslam_io.h"
#include "jndslam_std.h"
#include "jndslam_style.h"
#include "jndslam_smooth.h"

int main(int argc, char *argv[]);

void usage();

#endif
