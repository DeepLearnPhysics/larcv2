#!/bin/bash
set -e

# Source the LArCV environment
source /app/larcv2/configure.sh -q

# Execute the provided command
exec "$@"
