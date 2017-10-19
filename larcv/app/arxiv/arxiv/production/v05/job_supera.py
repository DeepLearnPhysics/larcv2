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
outfile = out_project + '_out_%04d.root' % session

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
my_proc.set_io_mode(fmwk.storage_manager.kREAD)

# Specify output root file name
my_proc.set_ana_output_file("")

# Attach an analysis unit ... here we use a base class which does nothing.
# Replace with your analysis unit if you wish.
unit = fmwk.Supera()
unit.set_config('%s/%s' % (JOBDIR_I,config))
unit.supera_fname('%s/%s' % (JOBDIR_O,outfile))
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

ret=1
try:
    ret=os.system('scp %s/%s %s' % (JOBDIR_O,outfile,storage))
    if ret:
        raise OSError    
    os.system('chmod 775 %s/%s' % (storage,outfile))
except OSError as e:
    os.system('rm -f %s/%s' % (storage,outfile))
    sys.exit(1)

record_path = '%s/%s' % (storage,outfile)
record_path = record_path.replace('//','/')

# Make sure if it is successful
if not os.path.isfile(record_path):
    print 'ERROR: could not locate output @ storage: %s' % record_path
    sys.exit(1)

t=table(in_project)
if out_project:
    out_t=table(out_project)
    if not out_t.exist(): out_t.create()
    out_t.fill(session_id=session,status=kSTATUS_INIT,filepath=record_path)

t.update_job_status(status=kSTATUS_DONE,job_index=jobid)
