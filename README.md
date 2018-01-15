[![Build Status](https://travis-ci.org/DeepLearnPhysics/larcv2.svg?branch=develop)](https://travis-ci.org/DeepLearnPhysics/larcv2) [![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://raw.githubusercontent.com/DeepLearnPhysics/larcv2/develop/LICENSE)
# LArCV: Liquid Argon Computer Vision
Software framework for image(2D)/volumetric(3D) data processing with APIs to interface deep neural network open-source softwares, written in C++ with extensive Python supports.  Originally developed for analyzing data from [time-projection-chamber (TPC)](https://en.wikipedia.org/wiki/Time_projection_chamber). It is then converted to be a generic tool to handle 2D-projected images and 3D-voxelized data. 

***Note*** This repository is re-created from LArbys/LArCV repository, referred to as larbys version. The larbys version is still under active development for analysis purpose in MicroBooNE experiment. This repository is split for more generic technical R&D work in October 2017.

## Installation

### Dependencies

* ROOT6
* Python (optional)
* OpenCV 3 (optional)
* Numpy (optional)

### Setup

0. Dependencies to build with are determined automatically through the following conditions.

  * ROOT: determined through the ability to run rootcling
  * OpenCV: the presence of OPENCV_INCDIR and OPENCV_LIBDIR environment variables
  * Numpy: being able to import `numpy`

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
