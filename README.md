[![Build and Test](https://github.com/DeepLearnPhysics/larcv2/actions/workflows/build-test.yml/badge.svg)](https://github.com/DeepLearnPhysics/larcv2/actions/workflows/build-test.yml) [![License](https://img.shields.io/github/license/mashape/apistatus.svg)](https://raw.githubusercontent.com/DeepLearnPhysics/larcv2/develop/LICENSE) [![Docker](https://ghcr-badge.egpl.dev/deeplearnphysics/larcv2/latest_tag?label=ghcr.io)](https://github.com/DeepLearnPhysics/larcv2/pkgs/container/larcv2)


# LArCV
Software framework for image(2D)/volumetric(3D) data processing with APIs to interface deep neural network open-source softwares, written in C++ with extensive Python supports.  Originally developed for analyzing data from [time-projection-chamber (TPC)](https://en.wikipedia.org/wiki/Time_projection_chamber). It is then converted to be a generic tool to handle 2D-projected images and 3D-voxelized data. 

***Note*** This repository is re-created from LArbys/LArCV repository, referred to as larbys version. The larbys version is still under active development for analysis purpose in MicroBooNE experiment. This repository is split for more generic technical R&D work in October 2017.

## Quick Start with Docker 🐳

The easiest way to use LArCV2 is via Docker containers, which come with all dependencies pre-installed:

```bash
# Pull the latest image
docker pull ghcr.io/deeplearnphysics/larcv2:latest

# Run interactively
docker run --rm -it ghcr.io/deeplearnphysics/larcv2:latest

# Or use the helper script
./docker-run.sh
```

Docker images are automatically built and published for every version tag. Available tags:
- `latest` - Latest stable release
- `develop` - Latest development version
- `X.Y.Z` - Specific version (e.g., `2.3.0`)

For more Docker options, see the [Docker Usage](#docker-usage) section below.

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

## Docker Usage

### Using Pre-built Images

Pre-built Docker images are available from GitHub Container Registry:

```bash
# Pull the latest stable release (Ubuntu 24.04)
docker pull ghcr.io/deeplearnphysics/larcv2:latest

# Pull a specific version
docker pull ghcr.io/deeplearnphysics/larcv2:2.3.0-ubuntu24.04

# Pull the latest development version
docker pull ghcr.io/deeplearnphysics/larcv2:develop-ubuntu24.04

# Pull Ubuntu 22.04 version (for MinkowskiEngine compatibility)
docker pull ghcr.io/deeplearnphysics/larcv2:ubuntu22.04
```

**Ubuntu Versions:**
- **Ubuntu 24.04**: Default, latest ROOT version (6.34.00)
- **Ubuntu 22.04**: For compatibility with MinkowskiEngine and older systems (ROOT 6.32.02)

### Running the Container

**Interactive session:**
```bash
docker run --rm -it ghcr.io/deeplearnphysics/larcv2:latest
```

**Run a Python script:**
```bash
docker run --rm -v $(pwd):/data ghcr.io/deeplearnphysics/larcv2:latest python /data/your_script.py
```

**Using the helper script:**
```bash
# Interactive bash
./docker-run.sh

# Run with mounted data directory
./docker-run.sh --mount /path/to/data python script.py

# Use specific version
./docker-run.sh --version 2.3.0 bash

# Use Ubuntu 22.04 version
./docker-run.sh --ubuntu-version 22.04 bash

# See all options
./docker-run.sh --help
```

### Building Locally

Two Dockerfiles are maintained side by side:

- `docker/Dockerfile.full` builds the existing development image with the
  optional LArCV applications and OpenCV support.
- `docker/Dockerfile.runtime` builds a runtime image containing only the ROOT
  components needed for LArCV I/O, the LArCV core, Python, and NumPy. It does
  not contain OpenCV, HDF5, Torch, CMake, or Git.

To build the images locally:

```bash
# Lean ROOT + LArCV runtime for Ubuntu 24.04
docker build -f docker/Dockerfile.runtime -t larcv2:runtime .

# Full development image for Ubuntu 24.04
docker build -f docker/Dockerfile.full -t larcv2:local .

# Full development image for Ubuntu 22.04
docker build -f docker/Dockerfile.full --build-arg UBUNTU_VERSION=22.04 --build-arg ROOT_VERSION=6.32.02 -t larcv2:ubuntu22.04 .
```

The runtime retains NumPy because LArCV's `PyUtil` bridge is required by
consumers such as SPINE. ROOT's C++ compiler driver and standard-library
headers are also runtime requirements of its Cling interpreter.

### Available Tags

The full image is published for Ubuntu 22.04 and 24.04. The lean runtime is
currently published for the tested Ubuntu 24.04 / ROOT 6.34 combination:

- Pushing `v2.4.2` creates full-image tags such as `2.4.2-ubuntu24.04`,
  `2.4.2-ubuntu22.04`, and `latest`.
- The same release creates `2.4.2-runtime-ubuntu24.04` and the moving
  `runtime` tag. The runtime image never replaces `latest`.
- Pushing to `develop` creates `develop-ubuntu24.04`,
  `develop-ubuntu22.04`, and `develop-runtime-ubuntu24.04`.
- Moving platform tags are `ubuntu24.04`, `ubuntu22.04`, and
  `runtime-ubuntu24.04`.

For example:

```bash
docker pull ghcr.io/deeplearnphysics/larcv2:runtime
docker pull ghcr.io/deeplearnphysics/larcv2:2.4.2-runtime-ubuntu24.04
```

## Releases

To create a new release:

1. Update the version in `python/larcv/version.py`
2. Commit the change: `git commit -am "Bump version to X.Y.Z"`
3. Create and push a tag: `git tag vX.Y.Z && git push origin vX.Y.Z`
4. GitHub Actions will publish the full and runtime image flavors. `latest`
   remains the full Ubuntu 24.04 image and `runtime` identifies the lean image.
