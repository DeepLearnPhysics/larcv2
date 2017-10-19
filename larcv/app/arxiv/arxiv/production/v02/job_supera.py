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

os.system('scp %s/larlite_*%04d.root ./' % (indir,proc))

files=[x for x in os.listdir('.') if x.startswith('larlite') and x.endswith('.root')]

outfile = config[0:config.rfind('.')] + '_%04d.root' % proc

cmd = 'python supera.py %s %s ' % (config,outfile)
for f in files: cmd += ' %s' % f

print cmd
p=Popen(cmd.split(),stdout=PIPE,stderr=PIPE)
(out,err) = p.communicate()
print out

os.system('scp %s %s' % (outfile,outdir))

