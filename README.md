[![Build Status](https://travis-ci.org/DeepLearnPhysics/larcv2.svg?branch=develop)](https://travis-ci.org/DeepLearnPhysics/larcv2) [![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://raw.githubusercontent.com/DeepLearnPhysics/larcv2/develop/LICENSE)
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

1. Clone & build
```
git clone https://github.com/DeepLearnPhysics/larcv2.git
cd larcv2
source configure.sh
make
```
That's it. When you want to use the built larcv from a different process, you only need to repeat ```source configure.sh``` and no need to re-```make```.


## Wiki

Checkout the [Wiki](https://github.com/DeepLearnPhysics/larcv2/wiki) for notes on using this code.
