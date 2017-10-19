#
# This is an example GNUmakefile for my packages
#
PACKAGE_NAME = CalibADC

# specific names for this package
SOURCES = $(wildcard *.cxx)
FMWK_HEADERS = LinkDef.h
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
#HEADERS += IOManager.inl

# include options for this package
INCFLAGS  = -I.                       #Include itself
INCFLAGS += $(shell larcv-config --includes)
INCFLAGS += `root-config --cflags`

# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

include $(LARCV_BASEDIR)/Makefile/Makefile.${OSNAME}

LDFLAGS += $(shell larcv-config --libs) -lLArCVPMTWeights

# call the common GNUmakefile
include $(LARCV_BASEDIR)/Makefile/GNUmakefile.CORE

pkg_build:
pkg_clean:


all: analyze_output

analyze_output: analyze_output.o
	$(CXX) $(CXXFLAGS) -o analyze_output analyze_output.o $(LDFLAGS) `root-config --libs`

analyze_output.o: analyze_output.cc
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -c analyze_output.cc -o analyze_output.o
