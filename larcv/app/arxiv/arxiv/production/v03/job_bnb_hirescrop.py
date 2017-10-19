#!/usr/bin/env python
import sys,commands,os
from subprocess import Popen, PIPE

proc=int(sys.argv[1])
indir=sys.argv[2]
outdir=sys.argv[3]
config=sys.argv[4]

if not os.path.isdir(indir):
    print 'Not present:',indir
    sys.exit(1)
if not os.path.isdir(outdir):
    print 'Not present:',outdir
    sys.exit(1)
if not os.path.isfile(config):
    print 'Not present:',config
    sys.exit(1)
if not os.path.isfile('procs.txt'):
    print 'Not present: procs.txt'
    sys.exit(1)

procs = [int(x) for x in open('procs.txt','r').read().split()]

if proc >= len(procs):
    print 'Process',proc,'exceeds needed # processes (%d)' % len(procs)
    sys.exit(1)

proc = procs[proc]

fname = 'supera_data_%04d.root' % proc
os.system('scp %s/%s ./' % (indir,fname))

outfile = 'hirescrop_bnb_%04d.root' % proc

cmd = 'python run_hiresdev.py %s %s %s' % (config,outfile,fname)

print cmd
p=Popen(cmd.split(),stdout=PIPE,stderr=PIPE)
(out,err) = p.communicate()
print out

os.system('scp %s %s' % (outfile,outdir))

