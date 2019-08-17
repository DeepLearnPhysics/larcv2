#FROM deeplearnphysics/ubuntu16.04_deps
FROM deeplearnphysics/larcv2:minimal

MAINTAINER kterao@slac.stanford.edu

# larcv build
ENV LARCV_BASEDIR=/app/larcv2
ENV LARCV_BUILDDIR="${LARCV_BASEDIR}/build"
ENV LARCV_COREDIR="${LARCV_BASEDIR}/larcv/core"
ENV LARCV_APPDIR="${LARCV_BASEDIR}/larcv/app"
ENV LARCV_LIBDIR="${LARCV_BUILDDIR}/lib"
ENV LARCV_INCDIR="${LARCV_BUILDDIR}/include"
ENV LARCV_BINDIR="${LARCV_BUILDDIR}/bin"
ENV LARCV_ROOT6=1
ENV LARCV_CXX=g++

# with numpy
ENV LARCV_NUMPY=1
ENV LARCV_INCLUDES="-I/app/larcv2/build/include -I/usr/include/python3.6m -I/usr/local/lib/python3.6/dist-packages/numpy/core/include"
ENV LARCV_LIBS="-L/usr/lib/ -L/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu -L/usr/lib -lpython3.6m -lpthread -ldl -lutil -lm -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions -L/app/larcv2/build/lib -llarcv"
ENV LARCV_PYTHON=/usr/bin/python3
ENV LARCV_PYTHON_CONFIG=python3.6-config

# set bin and lib path
ENV PATH=${LARCV_BASEDIR}/bin:${LARCV_BINDIR}:${PATH}
ENV LD_LIBRARY_PATH=${LARCV_LIBDIR}:${LD_LIBRARY_PATH}:
ENV PYTHONPATH=${LARCV_BASEDIR}/python:${PYTHONPATH}

# larcv
RUN mkdir -p /app && \
    cd /app && \
    git clone https://github.com/DeepLearnPhysics/larcv2 && \
    cd larcv2 && \
    mkdir -p $LARCV_BUILDDIR && \
    mkdir -p $LARCV_LIBDIR && \
    mkdir -p $LARCV_BINDIR && \
    make -j4
