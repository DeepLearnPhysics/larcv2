CXXFLAGS += $(shell larcv-config --includes)
CXXFLAGS += $(shell root-config --cflags)

LDFLAGS += $(shell larcv-config --libs)
LDFLAGS += $(shell root-config --libs)

OSNAME = $(shell uname -s)
include $(LARCV_BASEDIR)/Makefile/Makefile.${OSNAME}

PROGRAMS = calculate_mean dump_eventid

TARGETS = $(addsuffix .cc, $(PROGRAMS))

all: $(PROGRAMS)

$(PROGRAMS): 
	@echo '<<compiling' $@'>>'
	@$(CXX) $@.cc -o $(LARCV_BINDIR)/$@ $(CXXFLAGS) $(LDFLAGS) 
	@rm -rf *.dSYM

clean:	
	@for i in $(PROGRAMS); do ( echo "" && echo "Cleaning $$i..." && rm -f $(LARCV_BINDIR)/$$i || exit $$?; done
