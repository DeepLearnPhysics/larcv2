
ifndef LARCV_BASEDIR
ERROR_MESSAGE := $(error LARCV_BASEDIR is not set... run configure.sh!)
endif

OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

include $(LARCV_BASEDIR)/Makefile/Makefile.${OSNAME}

CORE_SUBDIRS := base
# ifeq ($(LARCV_NUMPY),1)
# CORE_SUBDIRS += PyUtil
# endif
# ifeq ($(LARCV_OPENCV),1)
#   CORE_SUBDIRS += CVUtil
# endif
$(info $$CORE_SUBDIRS is [${CORE_SUBDIRS}])

APP_SUBDIRS := 

.phony: all clean

all: directories obj  

directories:
	@mkdir -p $(LARCV_BUILDDIR)
	@mkdir -p $(LARCV_LIBDIR)
	@mkdir -p $(LARCV_BINDIR)
	@mkdir -p $(LARCV_LIBDIR)/larcv/
	@touch $(LARCV_LIBDIR)/larcv/__init__.py


distclean: clean
	@echo "Removing entire build area."
	@rm -r $(LARCV_BUILDDIR)

clean: clean_app clean_core
	@rm -f $(LARCV_LIBDIR)/liblarcv.so
clean_core:
	@for i in $(CORE_SUBDIRS); do ( echo "" && echo "Cleaning $$i..." && cd $(LARCV_COREDIR)/$$i && rm -rf $(LARCV_BUILDDIR)/$$i && rm -rf $(LARCV_BUILDDIR)/lib/*$ii.* ) || exit $$?; done
clean_app:
	@for i in $(APP_SUBDIRS); do ( echo "" && echo "Cleaning $$i..." && cd $(LARCV_APPDIR)/$$i && rm -rf $(LARCV_BUILDDIR)/$$i && rm -rf $(LARCV_BUILDDIR)/lib/*$ii.* ) || exit $$?; done



obj: $(LARCV_LIBDIR)/larcv/__init__.py
	@echo
	@echo Building core...
	@echo
	@for i in $(CORE_SUBDIRS); do ( echo "$i" && echo "Compiling $$i..." && cd $(LARCV_COREDIR)/$$i && $(MAKE) && echo "done" ) || exit $$?; done
	@echo Building app...
	@for i in $(APP_SUBDIRS); do ( echo "" && echo "Compiling $$i..." && cd $(LARCV_APPDIR)/$$i && $(MAKE) ) || exit $$?; done

lib: obj
	@ if [ `python ${LARCV_BASEDIR}/bin/libarg.py build` ]; then \
	    echo Linking library...; \
	    $(SOMAKER) $(SOFLAGS) $(shell python $(LARCV_BASEDIR)/bin/libarg.py); \
	  else \
	   echo Nothing to be done for lib...; \
	fi
	@echo
