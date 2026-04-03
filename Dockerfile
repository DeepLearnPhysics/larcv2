# Use ROOT6 official image as base
# Available versions: https://hub.docker.com/r/rootproject/root/tags
# Note: ROOT images are linux/amd64 only. On ARM (Apple Silicon), Docker will use emulation.
# Build args allow building for different Ubuntu versions
ARG UBUNTU_VERSION=24.04
ARG ROOT_VERSION=6.34.00
FROM rootproject/root:${ROOT_VERSION}-ubuntu${UBUNTU_VERSION}

LABEL maintainer="drielsma@stanford.edu"
LABEL org.opencontainers.image.source="https://github.com/DeepLearnPhysics/larcv2"
LABEL org.opencontainers.image.description="LArCV2: Image/volumetric data processing framework with deep learning interfaces"
LABEL org.opencontainers.image.licenses="MIT"

# Set working directory
WORKDIR /app/larcv2

# Set environment variables to prevent Python from writing bytecode and buffering
ENV PYTHONUNBUFFERED=1 \
    PYTHONDONTWRITEBYTECODE=1 \
    DEBIAN_FRONTEND=noninteractive

# Install system dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    python3-pip \
    python3-dev \
    python3-numpy \
    libopencv-dev \
    python3-opencv \
    && rm -rf /var/lib/apt/lists/*

# Set up Python symlinks (ROOT6 image might have different python setup)
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 1

# Copy the entire project
COPY . .

# Set up LArCV environment variables
ENV LARCV_BASEDIR=/app/larcv2 \
    LARCV_BUILDDIR=/app/larcv2/build \
    LARCV_COREDIR=/app/larcv2/larcv/core \
    LARCV_APPDIR=/app/larcv2/larcv/app \
    LARCV_LIBDIR=/app/larcv2/build/lib \
    LARCV_INCDIR=/app/larcv2/build/include \
    LARCV_BINDIR=/app/larcv2/build/bin \
    LARCV_ROOT6=1 \
    LARCV_CXX=g++ \
    LARCV_NUMPY=1 \
    LARCV_OPENCV=1

# Set OpenCV paths for configure script
ENV OPENCV_INCDIR=/usr/include/opencv4 \
    OPENCV_LIBDIR=/usr/lib/x86_64-linux-gnu

# Update PATH and library paths
# Note: Extending paths from base ROOT image which already has these variables set
# docker-lint: ignore=UndefinedVar
ENV PATH="${LARCV_BASEDIR}/bin:${LARCV_BINDIR}:${PATH}" \
    LD_LIBRARY_PATH="${LARCV_LIBDIR}:/usr/local/lib:${LD_LIBRARY_PATH:-}" \
    PYTHONPATH="${LARCV_BASEDIR}/python:${PYTHONPATH:-}"

# Build LArCV
RUN mkdir -p ${LARCV_BUILDDIR} ${LARCV_LIBDIR} ${LARCV_BINDIR} && \
    bash -c "source configure.sh -q && make -j$(nproc)"

# Verify the build
RUN python -c "import larcv; print('LArCV imported successfully')" || \
    (echo "Failed to import larcv" && exit 1)

# Set up entrypoint to ensure environment is loaded
COPY docker-entrypoint.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/docker-entrypoint.sh

ENTRYPOINT ["/usr/local/bin/docker-entrypoint.sh"]
CMD ["/bin/bash"]
