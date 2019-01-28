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

# without numpy
#ENV LARCV_NUMPY=0
#ENV LARCV_INCLUDES="-I${LARCV_INCDIR} "
#ENV LARCV_LIBS="-L${LARCV_LIBDIR} -llarcv"

# with numpy
ENV LARCV_NUMPY=1
ENV LARCV_INCLUDES="-I${LARCV_INCDIR} -I/usr/include/python2.7 -I/usr/include/x86_64-linux-gnu/python2.7"
ENV LARCV_LIBS="-L/usr/lib/ -L/usr/lib/python2.7/config-x86_64-linux-gnu -L/usr/lib"
ENV LARCV_LIBS="${LARCV_LIBS} -lpthread -ldl -lutil -lm -lpython2.7 -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions"
ENV LARCV_LIBS="${LARCV_LIBS} -L${LARCV_LIBDIR} -llarcv"

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
