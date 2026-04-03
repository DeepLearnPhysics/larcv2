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

To build the Docker image locally:

```bash
# Build for Ubuntu 24.04 (default)
docker build -t larcv2:local .

# Build for Ubuntu 22.04
docker build --build-arg UBUNTU_VERSION=22.04 --build-arg ROOT_VERSION=6.32.02 -t larcv2:ubuntu22.04 .
```

### Available Tags

Images are automatically built and published for both Ubuntu 22.04 and 24.04 when version tags are pushed:
- Pushing `v2.3.0` creates tags: `2.3.0-ubuntu24.04`, `2.3.0-ubuntu22.04`, `2.3-ubuntu24.04`, `2.3-ubuntu22.04`, `latest` (24.04)
- Pushing to `develop` creates: `develop-ubuntu24.04`, `develop-ubuntu22.04`
- Pushing to `main` creates: `main-ubuntu24.04`, `main-ubuntu22.04`
- Always available: `ubuntu24.04`, `ubuntu22.04`

## Releases

To create a new release:

1. Update the version in `python/larcv/version.py`
2. Commit the change: `git commit -am "Bump version to X.Y.Z"`
3. Create and push a tag: `git tag vX.Y.Z && git push origin vX.Y.Z`
4. GitHub Actions will automatically build and publish the Docker image to `ghcr.io/deeplearnphysics/larcv2:X.Y.Z`
