#!/bin/bash

LARCV_VERBOSE=1
if [[ $# -ne 0 ]]; then
    if [[ $@ -eq -q ]]; then
        LARCV_VERBOSE=0
    fi
fi


# clean up previously set env
if [[ -z $FORCE_LARCV_BASEDIR ]]; then
    where="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
    export LARCV_BASEDIR=${where}
else
    export LARCV_BASEDIR=$FORCE_LARCV_BASEDIR
fi

# set the build dir
unset LARCV_BUILDDIR
if [[ -z $LARCV_BUILDDIR ]]; then
    export LARCV_BUILDDIR=$LARCV_BASEDIR/build
fi

# Check python version compatibility:
export LARCV_PYTHON_CONFIG=python-config
PYVERSION=$(python -c "import sys; print(sys.version_info.major)")
if [ $PYVERSION -gt 2 ]; then
    minor=$(python -c "import sys; print(sys.version_info.minor)")
    export LARCV_PYTHON_CONFIG=python${PYVERSION}.${minor}-config
fi

export LARCV_COREDIR=$LARCV_BASEDIR/src/core
export LARCV_APPDIR=$LARCV_BASEDIR/src/app
export LARCV_LIBDIR=$LARCV_BUILDDIR/lib
export LARCV_INCDIR=$LARCV_BUILDDIR/include
export LARCV_BINDIR=$LARCV_BUILDDIR/bin
export LARCV_INCLUDES="-I${LARCV_INCDIR} `${LARCV_PYTHON_CONFIG} --includes` "
export LARCV_LIBS="-L${LARCV_LIBDIR} -llarcv "

# Check Numpy
export LARCV_NUMPY=`$LARCV_BASEDIR/bin/check_numpy`

# warning for missing support
missing=""
if [ $LARCV_NUMPY -eq 0 ]; then
    missing+=" Numpy"
else
    LARCV_INCLUDES="${LARCV_INCLUDES} -I`python -c\"import numpy; print(numpy.get_include())\"`"
    LARCV_LIBS="-L`${LARCV_PYTHON_CONFIG} --prefix`/lib/ `${LARCV_PYTHON_CONFIG} --ldflags` ${LARCV_LIBS}"
fi
if [[ $missing ]]; then
    if [[ $LARCV_VERBOSE -ne 0 ]]; then
        printf "\033[93mWarning\033[00m ... missing$missing support. Build without them.\n";
    fi
fi

if [[ $LARCV_VERBOSE -ne 0 ]]; then
    echo
    printf "\033[93mLArCV\033[00m FYI shell env. may useful for external packages:\n"
    printf "    \033[95mLARCV_INCDIR\033[00m   = $LARCV_INCDIR\n"
    printf "    \033[95mLARCV_LIBDIR\033[00m   = $LARCV_LIBDIR\n"
    printf "    \033[95mLARCV_BUILDDIR\033[00m = $LARCV_BUILDDIR\n"
fi

export PATH=$LARCV_BASEDIR/bin:$LARCV_BINDIR:$PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$LARCV_LIBDIR
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$LARCV_LIBDIR


mkdir -p $LARCV_BUILDDIR;
mkdir -p $LARCV_LIBDIR;
mkdir -p $LARCV_BINDIR;

export LD_LIBRARY_PATH=$LARCV_LIBDIR:$LD_LIBRARY_PATH
export PYTHONPATH=$LARCV_BASEDIR/python:$PYTHONPATH

export LARCV_CXX=clang++
if [ -z `command -v $LARCV_CXX` ]; then
    export LARCV_CXX=g++
    if [ -z `command -v $LARCV_CXX` ]; then
        echo
        echo Looks like you do not have neither clang or g++!
        echo You need one of those to compile LArCaffe... Abort config...
        echo
        return 1;
    fi
fi

if [[ $LARCV_VERBOSE -ne 0 ]]; then
    echo
    echo "Finish configuration. To build, type:"
    echo "> make "
    echo
fi

unset LARCV_VERBOSE