import os,sys
import ROOT
import numpy as np
import root_numpy as rn
import pandas as pd
from array import array

FILE=str(sys.argv[1])
print "GOT %s"%FILE
NAME=str(os.path.basename(FILE).split(".")[0])
print "NAME %s"%NAME
df = pd.DataFrame(rn.root2array(FILE,treename="analysistree/pottree"))[['run','subrun','pot']]
df['pot_fname'] = NAME
FOUT="pot_%s.root"%NAME
tf = ROOT.TFile.Open(FOUT,"RECREATE")
print "OPEN %s"%FOUT
tf.cd()

run    = array( 'i', [ 0 ] )
subrun = array( 'i', [ 0 ] )
pot    = array( 'd', [ 0 ] )
fname  = ROOT.std.string()

tree = ROOT.TTree("pot_tree","")

tree.Branch("run"      , run   , "run/I")
tree.Branch("subrun"   , subrun, "subrun/I")
tree.Branch("pot"      , pot   , "pot/D")
tree.Branch("pot_fname", fname)

for index,row in df.iterrows():
    run[0]    = int(row.run)
    subrun[0] = int(row.subrun)
    pot[0]    = long(row.pot)
    fname.replace(0,ROOT.std.string.npos,ROOT.std.string(row.pot_fname))

    tree.Fill()

tree.Write()
print "WRITE %s"%tree.GetName()
tf.Close()
print "CLOSE %s"%FOUT
                   




