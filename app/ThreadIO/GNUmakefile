#
# This is an example GNUmakefile for my packages
#
PACKAGE_NAME = DLInterface

# specific names for this package
SOURCES = $(wildcard *.cxx)
FMWK_HEADERS = LinkDef.h
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
#HEADERS += IOManager.inl

# include options for this package
INCFLAGS  = -I.                       #Include itself
INCFLAGS += $(shell larcv-config --includes)

# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

include $(LARCV_BASEDIR)/Makefile/Makefile.${OSNAME}

LDFLAGS += $(shell larcv-config --libs)
LDFLAGS += -L/usr/local/Cellar/python/2.7.11/Frameworks/Python.framework/Versions/2.7/lib -lpython2.7

# call the common GNUmakefile
include $(LARCV_BASEDIR)/Makefile/GNUmakefile.CORE

pkg_build:
#	@cp -f bin/run_filler $(LARCV_BINDIR)/run_filler
pkg_clean:
#	@rm -f $(LARCV_BINDIR)/run_filler

