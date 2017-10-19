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

LL_OUTFILE='larlite_supera_out.root'
LC_OUTFILE='larcv_supera_out.root'

if os.path.exists(LL_OUTFILE) or os.path.exists(LC_OUTFILE):
    print "Output file exists. Please remove:",LL_OUTFILE,LC_OUTFILE
    print "Giving up."
    sys.exit(1)

# Create ana_processor instance
my_proc = fmwk.ana_processor()

my_proc.enable_filter(True)

# Set input root file
for argv in sys.argv:
    if argv.endswith('.root'):
        my_proc.add_input_file(argv)

# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.kBOTH)

my_proc.set_output_file(LL_OUTFILE)

# Specify output root file name
my_proc.set_ana_output_file("")

# Attach an analysis unit ... here we use a base class which does nothing.
# Replace with your analysis unit if you wish.
unit = fmwk.Supera()
unit.set_config(sys.argv[1])
unit.supera_fname(LC_OUTFILE)
my_proc.add_process(unit)
my_proc.enable_filter()

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
