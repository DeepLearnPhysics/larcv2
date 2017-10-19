#!/usr/bin/env python
import sys,os

if len(sys.argv) < 4:
    msg  = '\n'
    msg += "Usage 1: %s CONFIG_FILE OUT_FILENAME $INPUT_ROOT_FILE(s)\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)
#import ROOT
#from ROOT import larlite as fmwk
##from larlite import larlite as fmwk
import ROOT
from larlite import larlite as fmwk
fmwk.storage_manager
from larcv import larcv
if os.path.exists(sys.argv[2]):
    print "Output file exists. Please remove."
    print "Output file specified: ",sys.argv[2]
    print "Giving up."
    sys.exit(1)

# Create ana_processor instance
my_proc = fmwk.ana_processor()

# Set input root file
for x in xrange(len(sys.argv)-3):
    my_proc.add_input_file(sys.argv[x+3])

# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.kREAD)

# Specify output root file name
my_proc.set_ana_output_file("")

# Attach an analysis unit ... here we use a base class which does nothing.
# Replace with your analysis unit if you wish.
unit = fmwk.Supera()
unit.set_config(sys.argv[1])
unit.supera_fname(sys.argv[2])
my_proc.add_process(unit)


print
print  "Finished configuring ana_processor. Start event loop!"
print

# Let's run it.
my_proc.run()

# done!
print
print "Finished running ana_processor event loop!"
print

sys.exit(0)
