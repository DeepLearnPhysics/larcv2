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
LARCV_PYVERSION=0
export LARCV_PYTHON=`which python`
if [ `command -v python` ]; then
    LARCV_PYVERSION=$($LARCV_PYTHON -c "import sys; print(sys.version_info.major)")
else
    export LARCV_PYTHON=`which python3`
    LARCV_PYVERSION=$($LARCV_PYTHON -c "import sys; print(sys.version_info.major)")
fi
if [[ $LARCV_PYVERSION -gt 2 ]]
then
    minor=$(python3 -c "import sys; print(sys.version_info.minor)")
    export LARCV_PYTHON_CONFIG=python${LARCV_PYVERSION}.${minor}-config
fi

# Check edep-sim
if [ `command -v edep-sim` ]; then
    where="$( dirname "$( dirname `which edep-sim` )" )"
    export LARCV_EDEPSIM_INCLUDES=$where/include
    export LARCV_EDEPSIM_LIBDIR=$where/lib
    export LARCV_EDEPSIM_LIBS="-L${LARCV_EDEPSIM_LIBDIR} -ledepsim -ledepsim_io "
fi

export LARCV_COREDIR=$LARCV_BASEDIR/larcv/core
export LARCV_APPDIR=$LARCV_BASEDIR/larcv/app
export LARCV_LIBDIR=$LARCV_BUILDDIR/lib
export LARCV_INCDIR=$LARCV_BUILDDIR/include
export LARCV_BINDIR=$LARCV_BUILDDIR/bin
export LARCV_INCLUDES="-I${LARCV_INCDIR} `${LARCV_PYTHON_CONFIG} --includes` "
export LARCV_LIBS="-L${LARCV_LIBDIR} -llarcv "

# Abort if ROOT not installed. Let's check rootcint for this.
if [ `command -v rootcling` ]; then
    export LARCV_ROOT6=1
else 
    if [[ -z `command -v rootcint` ]]; then
	echo
	echo Looks like you do not have ROOT installed.
	echo You cannot use LArLite w/o ROOT!
	echo Aborting.
	echo
	return 1;
    fi
fi


# Check OpenCV
export LARCV_OPENCV=1
if [[ -z $OPENCV_INCDIR ]]; then
    export LARCV_OPENCV=0
fi
if [[ -z $OPENCV_LIBDIR ]]; then
    export LARCV_OPENCV=0
fi

if [ $LARCV_OPENCV -eq 1 ]; then
    export LARCV_INCLUDES="${LARCV_INCLUDES} -I${OPENCV_INCDIR}"
    export LARCV_LIBS="-L${OPENCV_LIBDIR} -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs ${LARCV_LIBS}"
fi

# Check Numpy
export LARCV_NUMPY=`$LARCV_PYTHON $LARCV_BASEDIR/bin/check_numpy`

# warning for missing support
missing=""
if [ $LARCV_OPENCV -eq 0 ]; then
    missing+=" OpenCV"
fi
if [ $LARCV_NUMPY -eq 0 ]; then
    missing+=" Numpy"
else
    LARCV_INCLUDES="${LARCV_INCLUDES} -I`$LARCV_PYTHON -c\"import numpy; print(numpy.get_include())\"`"
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

if [ $LARCV_OPENCV -eq 1 ]; then
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$OPENCV_LIBDIR
    export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$OPENCV_LIBDIR
fi

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
