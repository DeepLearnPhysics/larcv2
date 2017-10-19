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

if not os.path.isfile('flist_nu.txt'):
    print 'Not present: flist_nu.txt'
    sys.exit(1)

if not os.path.isfile('flist_cosmic.txt'):
    print 'Not present: flist_cosmic.txt'
    sys.exit(1)

nu_files = [] 
cosmic_files = []

min_proc=1e9
max_proc=0

for f in [x for x in open('flist_nu.txt','r').read().split() if x.endswith('.root')]:
    os.system('scp %s ./' % f)
    nu_files.append(f.split('/')[-1])
    fname = nu_files[-1]
    proc=int(fname[-9:-5])
    if proc < min_proc: min_proc = proc
    if proc > max_proc: max_proc = proc

for f in [x for x in open('flist_cosmic.txt','r').read().split() if x.endswith('.root')]:
    os.system('scp %s ./' % f)
    cosmic_files.append(f.split('/')[-1])

fout=open('nu.txt','w')
for f in nu_files:
    fout.write('%s\n' % f)
fout.close()

fout=open('cosmic.txt','w')
for f in cosmic_files:
    fout.write('%s\n' % f)
fout.close()

outfile = 'overlay_%04d_%04d.root' % (min_proc,max_proc)

cmd = 'python run_merger.py %s %s nu.txt cosmic.txt' % (config,outfile)

print cmd
p=Popen(cmd.split(),stdout=PIPE,stderr=PIPE)
(out,err) = p.communicate()
print out

os.system('scp %s %s' % (outfile,outdir))

