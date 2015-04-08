# JNDSLAM
A method for automatic pitch contour stylisation

This version 0.01 features:

A simple method for stylising pitch contours based on the Just Noticeable Difference in pitch.

NOTE! This is incomplete! LOWESS smoothing is not yet implemented so the output is incorrect.

To use:

Unpack the tar.gz file and then the normal ./configure && make && make install should work.

I recommend just ./configure && make and then running from the src dir.

To run simply do run the jndslam file in source. It takes the .lab files in data/lab opens and parses them and tries to open and parse a corresponding .f0 in data/pitch. Output is written to data/out as .sty files (simply tab delimited lines, one per syllable).

Simply replace the .f0 and .lab files with your own to use those. Please be aware that .f0 files are assumed to be in EST format and .lab in HTS Full context label. The example .lab files may differ from the standard HTS implementation. Suppor for other formats will be provided asap.

Any issues please mail rasmus@dall.dk
