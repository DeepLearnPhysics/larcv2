#!/usr/bin/env python
import sys,os

if len(sys.argv) < 3:
    msg  = '\n'
    msg += "Usage 1: %s CONFIG_FILE OUT_FILENAME $INPUT_ROOT_FILE(s)\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)
#from ROOT import larlite as fmwk
import ROOT
from ROOT import fcllite
fcllite.PSet
from larlite import larlite as fmwk
#import ROOT
#from ROOT import fcllite
#from ROOT import larlite
#from larlite import larlite as fmwk
#from larcv import larcv
fmwk.storage_manager

#fmwk.LEEPreCut()
OUTFILE='out.root'

if os.path.exists(OUTFILE):
    print "Output file exists. Please remove."
    print "Output file specified: ", OUTFILE
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

my_proc.set_output_file('larlite_%s' % OUTFILE)

# Specify output root file name
my_proc.set_ana_output_file("")

# Attach an analysis unit ... here we use a base class which does nothing.
# Replace with your analysis unit if you wish.

precut=fmwk.LEEPreCut()
#precut_cfg=fcllite.CreatePSetFromFile(sys.argv[1],'precut').get_pset('LEEPreCut')
#precut.configure(precut_cfg)
my_proc.add_process(precut)

unit = fmwk.Supera()
unit.set_config(sys.argv[1])
unit.supera_fname('larcv_%s' % OUTFILE)
my_proc.add_process(unit)

my_proc.enable_filter()

print
print  "Finished configuring ana_processor. Start event loop!"
print

my_proc.set_data_to_read(fmwk.data.kSimChannel,       "largeant")
my_proc.set_data_to_read(fmwk.data.kMCTruth,        "generator")
my_proc.set_data_to_read(fmwk.data.kMCShower,       "mcreco")
my_proc.set_data_to_read(fmwk.data.kMCTrack,        "mcreco")
my_proc.set_data_to_read(fmwk.data.kWire,           "caldata")
my_proc.set_data_to_read(fmwk.data.kOpFlash,        "simpleFlashBeam")
my_proc.set_data_to_read(fmwk.data.kOpHit,          "ophitBeam")
my_proc.set_data_to_read(fmwk.data.kOpFlash,        "simpleFlashCosmic")
my_proc.set_data_to_read(fmwk.data.kOpHit,          "ophitCosmic")
my_proc.set_data_to_read(fmwk.data.kOpDetWaveform,  "saturation")
my_proc.set_data_to_read(fmwk.data.kTrigger,        "triggersim")
my_proc.set_data_to_read(fmwk.data.kSWTrigger,      "swtrigger")
my_proc.set_data_to_read(fmwk.data.kHit,            "gaushit")
my_proc.set_data_to_read(fmwk.data.kChStatus,       "chstatus")

my_proc.set_data_to_write(fmwk.data.kMCTruth,        "generator")
my_proc.set_data_to_write(fmwk.data.kMCShower,       "mcreco")
my_proc.set_data_to_write(fmwk.data.kMCTrack,        "mcreco")
my_proc.set_data_to_write(fmwk.data.kWire,           "caldata")
my_proc.set_data_to_write(fmwk.data.kOpFlash,        "simpleFlashBeam")
my_proc.set_data_to_write(fmwk.data.kOpHit,          "ophitBeam")
my_proc.set_data_to_write(fmwk.data.kOpFlash,        "simpleFlashCosmic")
my_proc.set_data_to_write(fmwk.data.kOpHit,          "ophitCosmic")
my_proc.set_data_to_write(fmwk.data.kOpDetWaveform,  "saturation")
my_proc.set_data_to_write(fmwk.data.kTrigger,        "triggersim")
my_proc.set_data_to_write(fmwk.data.kSWTrigger,      "swtrigger")
my_proc.set_data_to_write(fmwk.data.kHit,            "gaushit")
my_proc.set_data_to_write(fmwk.data.kChStatus,       "chstatus")

# Let's run it.
my_proc.run()

# done!
print
print "Finished running ana_processor event loop!"
print

sys.exit(0)
