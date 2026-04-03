#!/bin/bash

# LArCV2 Docker Helper Script
# Usage: ./docker-run.sh [OPTIONS] [COMMAND]
# 
# Examples:
#   ./docker-run.sh                          # Start interactive bash session
#   ./docker-run.sh python script.py         # Run a Python script
#   ./docker-run.sh --mount /data bash       # Mount local /data directory

set -e

# Default values
IMAGE="ghcr.io/deeplearnphysics/larcv2:latest"
MOUNT_DIR=""
INTERACTIVE="-it"
UBUNTU_VERSION=""

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --image)
            IMAGE="$2"
            shift 2
            ;;
        --mount)
            MOUNT_DIR="$2"
            shift 2
            ;;
        --version)
            IMAGE="ghcr.io/deeplearnphysics/larcv2:$2"
            shift 2
            ;;
        --ubuntu-version)
            UBUNTU_VERSION="$2"
            shift 2
            ;;
        --no-interactive)
            INTERACTIVE=""
            shift
            ;;
        -h|--help)
            echo "LArCV2 Docker Helper Script"
            echo ""
            echo "Usage: $0 [OPTIONS] [COMMAND]"
            echo ""
            echo "Options:"
            echo "  --image IMAGE        Use specific image (default: ghcr.io/deeplearnphysics/larcv2:latest)"
            echo "  --version VERSION    Use specific version tag (e.g., 2.3.4)"
            echo "  --ubuntu-version VER Use specific Ubuntu version (22.04 or 24.04)"
            echo "  --mount DIR          Mount local directory to /data in container"
            echo "  --no-interactive     Run in non-interactive mode"
            echo "  -h, --help           Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0                                    # Interactive bash session"
            echo "  $0 python script.py                   # Run Python script"
            echo "  $0 --mount /data python process.py    # Run with mounted data directory"
            echo "  $0 --version 2.3.4 bash               # Use specific version"
            echo "  $0 --ubuntu-version 22.04 bash        # Use Ubuntu 22.04 image"
            exit 0
            ;;
        *)
            # Remaining arguments are the command to run
            break
            ;;
    esac
done

# Apply Ubuntu version to image tag if specified
if [ -n "$UBUNTU_VERSION" ]; then
    # Extract base image and tag
    if [[ "$IMAGE" =~ ^(.+):(.+)$ ]]; then
        BASE="${BASH_REMATCH[1]}"
        TAG="${BASH_REMATCH[2]}"
        IMAGE="${BASE}:${TAG}-ubuntu${UBUNTU_VERSION}"
    else
        # No tag specified, use ubuntu version as tag
        IMAGE="${IMAGE}:ubuntu${UBUNTU_VERSION}"
    fi
fi

# Build docker run command
DOCKER_CMD="docker run --rm ${INTERACTIVE}"

# Add volume mount if specified
if [ -n "$MOUNT_DIR" ]; then
    DOCKER_CMD="${DOCKER_CMD} -v $(realpath $MOUNT_DIR):/data"
fi

# Add image
DOCKER_CMD="${DOCKER_CMD} ${IMAGE}"

# Add command (if provided)
if [ $# -gt 0 ]; then
    DOCKER_CMD="${DOCKER_CMD} $@"
fi

# Print and execute
echo "Running: ${DOCKER_CMD}"
echo ""
eval ${DOCKER_CMD}
