# LArCV: Liquid Argon Computer Vision
***Note*** This repository is re-created from LArbys/LArCV repository, referred to as larbys version. The larbys version is still under active development for analysis purpose in MicroBooNE experiment. This DeepLearnPhysics/larcv is split for more generic technical R&D work in October 2017.

Image/Volumetric data processing framework for LAr TPC-derived images. Developed to interface (LAr)TPC experiment software data to a deep neural network frameworks. Get to know more about this software @ our [Wiki](https://github.com/DeepLearnPhysics/larcv2/wiki)

## Installation

### Dependencies

* ROOT6
* Python (optional)
* OpenCV 3 (optional)

### Setup

0. Dependencies to build with are determined through the presence of environment variables or executables in your PATH:

  * ROOT: determined through the ability to run rootcling
  * OPENCV: the presence of OPENCV_INCDIR and OPENCV_LIBDIR

1. clone the repository

      git clone https://github.com/LArbys/LArCV.git

2. go into the LArCV directory
3. run the build configuration script

      source configure.sh
      
4. build

      make
      

That's it.


## Wiki

Checkout the [Wiki](https://github.com/DeepLearnPhysics/larcv2/wiki) for notes on using this code.
