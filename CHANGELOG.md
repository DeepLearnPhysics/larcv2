# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

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

## [2.3.2] - Previous release

[Unreleased]: https://github.com/DeepLearnPhysics/larcv2/compare/v2.3.3...HEAD
[2.3.3]: https://github.com/DeepLearnPhysics/larcv2/releases/tag/v2.3.3
[2.3.2]: https://github.com/DeepLearnPhysics/larcv2/releases/tag/v2.3.2
