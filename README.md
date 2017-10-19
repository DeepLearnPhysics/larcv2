# LArCV: Liquid Argon Computer Vision
***Note*** This repository is re-created from LArbys/LArCV repository, referred to as larbys version. The larbys version is still under active development for analysis purpose in MicroBooNE experiment. This DeepLearnPhysics/larcv is split for more generic technical R&D work in October 2017.

Data format and image processing tools, routines, and framework for LAr TPC-derived images. Developing as bridge between LArSoft and Caffe/Tensorflow.

## Installation

### Dependencies

* ROOT
* Python (optional)
* OpenCV 3 (optional)
* LArLite (optional)
* LArSoft (optional)

### Setup

0. Dependencies to build with are determined through the presence of environment variables or executables in your PATH:

  * ROOT: determined through the ability to run rootcling (ROOT6) or rootcint (ROOT5)
  * OPENCV: the presence of OPENCV_INCDIR and OPENCV_LIBDIR
  * LArLite: the presence of LARLITE_BASEDIR (created when one runs configure/setup.sh in the larlite root folder)
  * LArSoft: if MRB_TOP defined and the uboonecode source directories found, will build LArSoft API
  

1. clone the repository

      git clone https://github.com/LArbys/LArCV.git

2. go into the LArCV directory
3. run the build configuration script

      source configure.sh
      
4. build

      make
      

That's it.


## Wiki

Checkout the [Wiki](https://github.com/LArbys/LArCV/wiki) for notes on using this code.
