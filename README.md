# JNDSLAM
A method for automatic pitch contour stylisation

This version 0.3 features:

A simple method for stylising pitch contours based on the Just Noticeable Difference in pitch.

This offers both the JNDSLAM and Simplified JNDSLAM method as described in a paper submitted to Interspeech 2015 (Dall, R. & Gonzalvo, X. (2015). Automatic Stylisation and Labelling of Pitch Contour for Speech Synthesis. Submitted to Interspeech 2015, Dresden, Germany).

It also offers the original SLAM method from Obin, N., Beliao, J., Veaux, C., & Lacheret, A. (2014). SLAM: Automatic Stylization and Labelling of Speech Melody. Speech Prosody 7, 246-250. Python implementation available at https://github.com/jbeliao/SLAM.

To use:

Clone the git repository and then the normal ./configure && make && make install should work.

I recommend just ./configure && make.

To run simply run the created binary jndslam. The default settings takes the .lab files in data/simple_lab opens and parses them and tries to open and parse a corresponding .f0 in data/pitch. Output is written to data/out as .sty files (simply space delimited lines, one per segment).

A few command line options exist use -h or --help to get the options.

Simply replace the .f0 and .lab files with your own to use those. Please be aware that .f0 files are assumed to be in EST format.

Input in HTS full-context label format is also supported. Sample files are in data/hts_lab/. Please note that the example .lab files may differ from the standard HTS implementation and only supports syllable stylisation. Support for arbitrary HTS full context label formats with options for other segment types is underway. If you wish to stylise other segments than syllables please use the simple_lab format.

Any issues please mail rasmus@dall.dk
