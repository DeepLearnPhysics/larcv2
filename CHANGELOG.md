# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.4.1] - 2026-06-28

### Fixed
- Initialized `CRTHit` default values before ROOT streaming.
- Added missing standard library includes for `CRTHit`.

## [2.4.0] - 2026-06-27

### Changed
- Optimized `CombineTensor3D` for exact voxel-id merging when fuzzy matching is disabled.
- Initialized `VoxelSet` instance IDs in the ordered vector constructor.

## [2.3.4] - 2026-04-03

### Added
- Multi-platform Docker support for both Ubuntu 22.04 and 24.04
- Ubuntu 22.04 images for MinkowskiEngine compatibility (ROOT 6.32.02)
- `--ubuntu-version` flag to docker-run.sh helper script
- Matrix build strategy in GitHub Actions for parallel image builds

### Changed
- Dockerfile now uses build arguments for Ubuntu and ROOT version selection
- Docker images published with Ubuntu version suffixes (e.g., `-ubuntu22.04`, `-ubuntu24.04`)
- Updated documentation to explain Ubuntu version differences and usage

## [2.3.3] - 2026-04-02

### Added
- Docker containerization with automatic builds via GitHub Actions
- Container images published to ghcr.io/deeplearnphysics/larcv2
- Docker helper script (`docker-run.sh`) for easier container usage
- Version tracking in `python/larcv/version.py`
- Comprehensive Docker documentation in README

### Changed
- Modernized Dockerfile with ROOT 6.34.00 and Ubuntu 24.04 LTS
- Updated build system to support containerized builds
- Migrated from Travis CI to GitHub Actions for CI/CD
- Updated badges in README with modern alternatives

### Removed
- Deprecated Travis CI integration
- Old Singularity Hub references
