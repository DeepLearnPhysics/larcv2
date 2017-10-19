#!/usr/bin/env python
from proddb.table import table
from proddb.dbenv import *
import sys,commands,os
from subprocess import Popen, PIPE

in_project=sys.argv[1]
t=table(in_project)
if not t.exist():
    print 'Project does not exist:',sys.argv[1]
    sys.exit(1)
jobid  = int(sys.argv[2])
session = t.job_session(job_index=jobid)
t.update_job_status(status=kSTATUS_RUNNING,job_index=jobid)
#
# Sleep for sometime to avoid simultaneous execution across all jobs
#
import time
time.sleep(jobid%60)

config  = sys.argv[3]
storage = sys.argv[4]
out_project=''
if len(sys.argv) > 5:
    out_project = sys.argv[5]

if not os.path.isdir(storage):
    print 'Not present:',storage
    sys.exit(1)
if not os.path.isfile(config):
    print 'Not present:',config
    sys.exit(1)

#
# Prepare input/output locations
#

files = t.job_files(job_index=jobid)
if len(files) == 0:
    print 'No input files found for job %d' % session
    sys.exit(1)

t.close()

JOBDIR_I='tmp_input'
JOBDIR_O='tmp_output'

if os.path.isdir(JOBDIR_I):
    print 'ERROR: job temporary input directory already exist!'
    sys.exit(1)
if os.path.isdir(JOBDIR_O):
    print 'ERROR: job temporary output directory already exist!'
    sys.exit(1)

os.mkdir(JOBDIR_I)
for f in files:
    os.system('scp %s %s/' % (f,JOBDIR_I))
in_files = [f[f.rfind('/')+1:len(f)] for f in files]

os.system('scp %s %s/' % (config,JOBDIR_I))
config = config[config.rfind('/')+1:len(config)]

os.mkdir(JOBDIR_O)
LC_outfile = 'larcv_' + out_project + '_out_%04d.root' % session
LL_outfile = 'larlite_' + out_project + '_out_%04d.root' % session

#
# supera.py part
#
import ROOT
from larlite import larlite as fmwk
fmwk.storage_manager
from larcv import larcv

# Create ana_processor instance
my_proc = fmwk.ana_processor()

# Set input root file
for f in in_files:
    my_proc.add_input_file('%s/%s' % (JOBDIR_I,f))

# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.kBOTH)

# Specify output root file name
my_proc.set_ana_output_file("")
my_proc.set_output_file('%s/%s'%(JOBDIR_O,LL_outfile))

# Pre cuts
precut=fmwk.LEEPreCut()
my_proc.add_process(precut)

# Attach an analysis unit ... here we use a base class which does nothing.
# Replace with your analysis unit if you wish.
unit = fmwk.Supera()
unit.set_config('%s/%s' % (JOBDIR_I,config))
unit.supera_fname('%s/%s' % (JOBDIR_O,LC_outfile))
my_proc.add_process(unit)

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

# transfer LARCV FILE
ret=1
try:
    print 'Transferring output: %s/%s => %s' % (JOBDIR_O,LC_outfile,storage)
    ret=os.system('scp %s/%s %s' % (JOBDIR_O,LC_outfile,storage))
    if ret:
        raise OSError    
    os.system('chmod 775 %s/%s' % (storage,LC_outfile))
except OSError as e:
    os.system('rm -f %s/%s' % (storage,LC_outfile))
    sys.exit(1)

# transfer LARLITE FILE
ret=1
try:
    print 'Transferring output: %s/%s => %s' % (JOBDIR_O,LL_outfile,storage)
    ret=os.system('scp %s/%s %s' % (JOBDIR_O,LL_outfile,storage))
    if ret:
        raise OSError    
    os.system('chmod 775 %s/%s' % (storage,LL_outfile))
except OSError as e:
    os.system('rm -f %s/%s' % (storage,LL_outfile))
    sys.exit(1)

LC_record_path = '%s/%s' % (storage,LC_outfile)
LC_record_path = LC_record_path.replace('//','/')
LL_record_path = '%s/%s' % (storage,LL_outfile)
LL_record_path = LL_record_path.replace('//','/')

# Make sure if it is successful
if not os.path.isfile(LC_record_path):
    print 'ERROR: could not locate larcv output @ storage: %s' % LC_record_path
    sys.exit(1)
else:
    print 'SUCCESS: confirmed larcv output @ storage: %s' % LC_record_path

# Make sure if it is successful
if not os.path.isfile(LL_record_path):
    print 'ERROR: could not locate larlite output @ storage: %s' % LL_record_path
    sys.exit(1)
else:
    print 'SUCCESS: confirmed larlite output @ storage: %s' % LL_record_path

t=table(in_project)
if out_project:
    out_t=table(out_project)
    if not out_t.exist(): out_t.create()
    out_t.fill(session_id=session,status=kSTATUS_INIT,filepath=LC_record_path)
    out_t.fill(session_id=session,status=kSTATUS_INIT,filepath=LL_record_path)

t.update_job_status(status=kSTATUS_DONE,job_index=jobid)
