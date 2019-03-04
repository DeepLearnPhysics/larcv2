#!/usr/bin/env python
import sys,commands,os
from subprocess import Popen, PIPE

outdir=sys.argv[1]
config=sys.argv[2]

if not os.path.isdir(outdir):
    print 'Not present:',outdir
    sys.exit(1)
if not os.path.isfile(config):
    print 'Not present:',config
    sys.exit(1)

if not os.path.isfile('flist.txt'):
    print 'Not present: flist.txt'
    sys.exit(1)

min_proc=1e9
max_proc=0
files=[]
for f in [x for x in open('flist.txt','r').read().split() if x.endswith('.root')]:
    os.system('scp %s ./' % f)
    files.append(f.split('/')[-1])
    fname = files[-1]
    proc=int(fname[-9:-5])
    if proc < min_proc: min_proc = proc
    if proc > max_proc: max_proc = proc

outfile = config[0:config.rfind('.')] + '_%04d_%04d.root' % (min_proc,max_proc)

cmd = 'python run_scale.py %s %s ' % (config,outfile)

for f in files: cmd += ' %s' % f

print cmd
p=Popen(cmd.split(),stdout=PIPE,stderr=PIPE)
(out,err) = p.communicate()
print out

os.system('scp %s %s' % (outfile,outdir))
