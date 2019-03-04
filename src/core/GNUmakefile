
ifndef LARCV_COREDIR
ERROR_MESSAGE := $(error LARCV_COREDIR is not set... run configure.sh!)
endif

#####################################################################################
#
# Define directories to be compile upon a global "make"...
#
SUBDIRS := Base DataFormat Processor CPPUtil
ifeq ($(LARCV_NUMPY),1)
SUBDIRS += PyUtil
endif
ifeq ($(LARCV_OPENCV),1)
  SUBDIRS += CVUtil
endif

#####################################################################################
#
# COMPILATION...
#
#.phony: all configure default-config clean
.phony: all clean

all:
	@for i in $(SUBDIRS); do ( echo "" && echo "Compiling $$i..." && cd $$i && $(MAKE) ) || exit $$?; done
#####################################################################################
#
# CLEANs...
#
clean:
	@for i in $(SUBDIRS); do ( echo "" && echo "Cleaning $$i..." && cd $$i && $(MAKE) clean && rm -rf $(LARCV_BUILDDIR)/$$i && rm -rf $(LARCV_BUILDDIR)/lib/*$ii.* ) || exit $$?; done

#####################################################################################
#
# DOCUMENTATION...
#
doxygen:
	@echo 'dOxygenising LArCV core...'
	@mkdir -p $(LARCV_COREDIR)/doc/dOxygenMyProject
	@doxygen $(LARCV_COREDIR)/doc/doxygenMyProject.script
#
#####################################################################################
#EOF
