#!/usr/bin/env python
from proddb.table import table
from proddb.dbenv import *
import sys,commands,os
from subprocess import Popen, PIPE

in_project1=sys.argv[1]
in_project2=sys.argv[2]
t1=table(in_project1)
if not t1.exist():
    print 'Project does not exist:',sys.argv[1]
    sys.exit(1)
t2=table(in_project2)
if not t2.exist():
    print 'Project does not exist:',sys.argv[2]
    sys.exit(1)
jobid  = int(sys.argv[3])
session = t1.job_session(job_index=jobid)

t1.update_job_status(status=kSTATUS_RUNNING,job_index=jobid)
t2.update_job_status(status=kSTATUS_RUNNING,job_index=jobid)
#
# Sleep for sometime to avoid simultaneous execution across all jobs
#
import time
time.sleep(jobid%60)

config = sys.argv[4]
storage = sys.argv[5]
out_project=''
if len(sys.argv) > 6:
    out_project = sys.argv[6]

if not os.path.isdir(storage):
    print 'Not present:',storage
    sys.exit(1)
if not os.path.isfile(config):
    print 'Not present:',config
    sys.exit(1)

#
# Prepare input/output locations
#

files1 = t1.job_files(job_index=jobid)
if len(files1) == 0:
    print 'No input files found for job %d' % jobid
    sys.exit(1)
t1.close()

files2 = t2.job_files(job_index=jobid)
if len(files2) == 0:
    print 'No input files found for job %d' % jobid
    sys.exit(1)
t2.close()

JOBDIR_I='tmp_input'
JOBDIR_O='tmp_output'

if os.path.isdir(JOBDIR_I):
    print 'ERROR: job temporary input directory already exist!'
    sys.exit(1)
if os.path.isdir(JOBDIR_O):
    print 'ERROR: job temporary output directory already exist!'
    sys.exit(1)

os.mkdir(JOBDIR_I)
for f in files1:
    os.system('scp %s %s/' % (f,JOBDIR_I))
in_files1 = [f[f.rfind('/')+1:len(f)] for f in files1]
for f in files2:
    os.system('scp %s %s/' % (f,JOBDIR_I))
in_files2 = [f[f.rfind('/')+1:len(f)] for f in files2]

os.system('scp %s %s/' % (config,JOBDIR_I))
config = config[config.rfind('/')+1:len(config)]

os.mkdir(JOBDIR_O)
outfile = out_project + '_out_%04d.root' % session

anafile = 'ana_' + outfile

#
# run_merger part
#
import ROOT, sys,os
from ROOT import std
from larcv import larcv

proc = larcv.MergeTwoStream()

proc.configure('%s/%s' % (JOBDIR_I,config))

proc.override_output_file('%s/%s' % (JOBDIR_O,outfile))

proc.override_ana_file('%s/%s' % (JOBDIR_O,anafile))

vlist1 = ROOT.std.vector('string')()
for f in in_files1:
    vlist1.push_back('%s/%s' % (JOBDIR_I,f))

vlist2 = ROOT.std.vector('string')()
for f in in_files2:
    vlist2.push_back('%s/%s' % (JOBDIR_I,f))

proc.override_input_file(vlist1,vlist2)

proc.initialize()

proc.batch_process()

proc.finalize()

#
# Transfer analysis output file
#
ret=1
if os.path.isfile('%s/%s' % (JOBDIR_O,anafile)):
    try:
        ret=os.system('scp %s/%s %s' % (JOBDIR_O,outfile,storage))
        if ret:
            raise OSError    
        os.system('chmod 775 %s/%s' % (storage,outfile))
    except OSError as e:
        os.system('rm -f %s/%s' % (storage,anafile))
        sys.exit(1)
#
# Transfer production output file
#
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
    os.system('rm -f %s/%s' % (storage,anafile))
    sys.exit(1)

t1=table(in_project1)
t2=table(in_project2)
if out_project:
    out_t=table(out_project)
    if not out_t.exist(): out_t.create()
    out_t.fill(session_id=session,status=kSTATUS_INIT,filepath=record_path)

t1.update_status(status=kSTATUS_DONE,job_index=jobid)
t2.update_status(status=kSTATUS_DONE,job_index=jobid)
