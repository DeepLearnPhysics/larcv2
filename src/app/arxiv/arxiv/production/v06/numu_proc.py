import commands, sys, os

part=sys.argv[1]

start_idx = 0
if len(sys.argv)>2:
    start_idx=int(sys.argv[2])

indir  = '/a/data/amsterdam/dl/dl_production_v00/larcv/mcc7_bnb_detsim_to_larcv_fullreswire_inclusive_v00_%s' % part
outdir = 'numu_%s' % part
prefix = 'mcc7_bnb_detsim_to_larcv_fullreswire_inclusive_v00_%s_' % part

if not os.path.isdir(outdir):
    os.mkdir(outdir)

flist_v=[]
pairs=[]

master_flist = [x for x in os.listdir(indir) if x.startswith(prefix) and x.endswith('.root')]
master_flist.sort()
flist=[]
for f in master_flist:
    flist.append('%s/%s' % (indir,f))
    if len(flist) == 50:
        flist_v.append(flist)
        flist = []
if len(flist):
    flist_v.append(flist)

del flist

ctr = 0
for flist in flist_v:

    out_data = '%s/out_%02d.root' % (outdir,ctr)
    out_ana = '%s/ana_%02d.root' % (outdir,ctr)
    if os.path.isfile(out_data):
        print 'Skipping',ctr,'(output already exists)'
        ctr+=1
        continue
    cwd=os.getcwd()
    cmd='cd %s;run_processor %s/hiresdiv_fullwire_mc_bnb_numu.cfg' % (outdir,cwd)
    for f in flist:
        cmd += ' %s' % f
    cmd += '; cd -'
    print 'Processing',ctr
    print commands.getoutput(cmd)
    if os.path.isfile('%s/out.root' % outdir):
        os.system('mv %s/out.root %s' % (outdir,out_data))
    if os.path.isfile('%s/ana.root' % outdir):
        os.system('mv %s/ana.root %s' % (outdir,out_ana))
    ctr+=1

