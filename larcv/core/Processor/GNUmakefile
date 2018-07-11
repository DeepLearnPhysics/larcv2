#
# This is an example GNUmakefile for my packages
#
PACKAGE_NAME = Processor

# specific names for this package
SOURCES = $(wildcard *.cxx)
FMWK_HEADERS = LinkDef.h
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
HEADERS_DEST = larcv/core/$(PACKAGE_NAME)

# include options for this package
INCFLAGS  = -I.                       #Include itself
INCFLAGS += -I$(LARCV_INCDIR)

# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

include $(LARCV_BASEDIR)/Makefile/Makefile.${OSNAME}

LDFLAGS += -L$(LARCV_LIBDIR) -lLArCVBase -lLArCVDataFormat
# call the common GNUmakefile
include $(LARCV_BASEDIR)/Makefile/GNUmakefile.CORE

pkg_build:
	@cp -f bin/run_processor $(LARCV_BINDIR)/run_processor
	@cp -f bin/run_processor.py $(LARCV_BINDIR)/run_processor.py
	@cp -f bin/run_processordb $(LARCV_BINDIR)/run_processordb
pkg_clean:
	@rm -f $(LARCV_BINDIR)/run_processor
	@rm -f $(LARCV_BINDIR)/run_processor.py
	@rm -f $(LARCV_BINDIR)/run_processordb
