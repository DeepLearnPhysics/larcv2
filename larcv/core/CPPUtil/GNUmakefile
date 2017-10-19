#
# This is an example GNUmakefile for my packages
#
PACKAGE_NAME = CPPUtil

# specific names for this package
SOURCES = $(wildcard *.cxx)
FMWK_HEADERS = LinkDef.h stack_trace.h
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
HEADERS_DEST = larcv/core/$(PACKAGE_NAME)

# include options for this package
INCFLAGS  = -I.
INCFLAGS += -I$(LARCV_INCDIR)

# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

include $(LARCV_BASEDIR)/Makefile/Makefile.${OSNAME}

# call the common GNUmakefile
include $(LARCV_BASEDIR)/Makefile/GNUmakefile.CORE

pkg_build: # nothing extra to do in this package
pkg_clean: # nothing extra to do in this package

