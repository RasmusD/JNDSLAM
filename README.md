# JNDSLAM
A method for automatic pitch contour stylisation

This version 0.4 features:

A simple method for stylising pitch contours based on the Just Noticeable Difference in pitch.

This offers both the JNDSLAM and Simplified JNDSLAM method as described in a paper draft available upon request.

It also offers the original SLAM method from Obin, N., Beliao, J., Veaux, C., & Lacheret, A. (2014). SLAM: Automatic Stylization and Labelling of Speech Melody. Speech Prosody 7, 246-250. Python implementation available at https://github.com/jbeliao/SLAM.

Finally output can be given in raw semitone values. This outputs one extra value compared to the others which is the position of the extreme in the segment.

To use:

Clone the git repository and then the normal ./configure && make && make install should work.

I recommend just ./configure && make.

To run simply run the created binary jndslam. The default settings takes the .lab files in data/simple_lab opens and parses them and tries to open and parse a corresponding .f0 in data/pitch. Output is written to data/out as .sty files (simply space delimited lines, one per segment).

A few command line options exist use -h or --help to get the options.

Simply replace the .f0 and .lab files with your own to use those. Please be aware that .f0 files are assumed to be in EST format.

Input in HTS full-context label format is also supported. Sample files are in data/hts_lab/. To use the provided sample HTS labs run hts with the -hts option and specify the correct directory (i.e. ./jndslam --hts "- + : /" -l data/hts_lab/ for the sample labels). Please note that the example .lab files may differ from the standard HTS implementation and thus the delimiters may be different in your files. Only supports syllable stylisation. Support for other segmentations is underway. If you wish to stylise other segments than syllables please use the simple_lab format.

Any issues please mail rasmus@dall.dk
