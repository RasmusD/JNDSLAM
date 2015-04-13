ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

# This is temporary. config should check for this but not sure if it fails if not present.
AM_CXXFLAGS = -std=c++0x

bin_PROGRAMS = jndslam
jndslam_SOURCES = src/jndslam_main.cpp src/jndslam_main.h \
  src/utterance.cpp src/utterance.h \
  src/syllable.cpp src/syllable.h \
  src/jndslam_io.cpp src/jndslam_io.h \
  src/jndslam_std.cpp src/jndslam_std.h \
  src/jndslam_smooth.cpp src/jndslam_smooth.h \
  src/jndslam_style.cpp src/jndslam_style.h

labdir = data/lab

dist_lab_DATA = data/lab/arctic_a0001_1.lab \
  data/lab/arctic_a0002_1.lab \
  data/lab/arctic_a0003_1.lab

pitchdir = data/pitch

dist_pitch_DATA = data/pitch/arctic_a0001_1.f0 \
  data/pitch/arctic_a0002_1.f0 \
  data/pitch/arctic_a0003_1.f0

outdir = data/out

dist_out_DATA = data/out/NOTE

extradir = .

dist_extra_DATA = README.md LICENSE CHANGELOG
