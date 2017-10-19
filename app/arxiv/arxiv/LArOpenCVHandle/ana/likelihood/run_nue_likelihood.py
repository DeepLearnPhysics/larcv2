import os, sys
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

import ROOT
import numpy as np
import pandas as pd

import root_numpy as rn

from util.fill_df import *


BASE_PATH = os.path.realpath(__file__)
BASE_PATH = os.path.dirname(BASE_PATH)

rse    = ['run','subrun','event']
rsev   = ['run','subrun','event','vtxid']
rserv  = ['run','subrun','event','roid','vtxid']

# Vertex data frame
dfs  = {}

# Event data frame
edfs = {}
mdfs = {}

name        = sys.argv[1]
INPUT_FILE  = sys.argv[2]

vertex_df = pd.DataFrame(rn.root2array(INPUT_FILE,treename='VertexTree'))
angle_df  = pd.DataFrame(rn.root2array(INPUT_FILE,treename='AngleAnalysis'))
shape_df  = pd.DataFrame(rn.root2array(INPUT_FILE,treename='ShapeAnalysis'))
gap_df    = pd.DataFrame(rn.root2array(INPUT_FILE,treename="GapAnalysis"))
match_df  = pd.DataFrame(rn.root2array(INPUT_FILE,treename="MatchAnalysis"))
dqds_df   = pd.DataFrame(rn.root2array(INPUT_FILE,treename="dQdSAnalysis"))

#
# Combine DataFrames
#
comb_df = pd.concat([vertex_df.set_index(rserv),
                     angle_df.set_index(rserv),
                     shape_df.set_index(rserv),
                     gap_df.set_index(rserv),
                     angle_df.set_index(rserv),
                     match_df.set_index(rserv),
                     dqds_df.set_index(rserv)],axis=1)

comb_df = comb_df.reset_index()
event_vertex_df   = pd.DataFrame(rn.root2array(INPUT_FILE,treename="EventVertexTree"))

def drop_y(df):
    to_drop = [x for x in df if x.endswith('_y')]
    df.drop(to_drop, axis=1, inplace=True)
    
comb_df = comb_df.set_index(rse).join(event_vertex_df.set_index(rse),how='outer',lsuffix='',rsuffix='_y').reset_index()
drop_y(comb_df)
    
if name not in ['cosmic']:
    nufilter_df = pd.DataFrame(rn.root2array(INPUT_FILE,treename="NuFilterTree"))
    mc_df       = pd.DataFrame(rn.root2array(INPUT_FILE,treename="MCTree"))
        
    comb_df = comb_df.set_index(rse).join(nufilter_df.set_index(rse),how='outer',lsuffix='',rsuffix='_y').reset_index()
    drop_y(comb_df)    
        
    comb_df = comb_df.set_index(rse).join(mc_df.set_index(rse),how='outer',lsuffix='',rsuffix='_y').reset_index()
    drop_y(comb_df)

comb_df = comb_df.reset_index()
comb_df = comb_df.loc[:,~comb_df.columns.duplicated()]

comb_df['cvtxid'] = 0.0

def func(group):
    group['cvtxid'] = np.arange(0,group['cvtxid'].size)
    return group

comb_df = comb_df.groupby(['run','subrun','event']).apply(func)
comb_cut_df = comb_df.copy()

print
print "@ sample",name
print
comb_cut_df = comb_df.copy()
print "--> nue assumption"
comb_cut_df = nu_assumption(comb_cut_df)
print "--> fill parameters"
comb_cut_df = fill_parameters(comb_cut_df)

#
# Pull distributions & binning for PDFs from ROOT file @ ll_bin/"
#
print "--> reading PDFs"
fin = os.path.join(BASE_PATH,"ll_bin","nue_pdfs.root")
tf_in = ROOT.TFile(fin,"READ")
tf_in.cd()

keys_v = [key.GetName() for key in tf_in.GetListOfKeys()]

sig_spectrum_m = {}
bkg_spectrum_m = {}

for key in keys_v:
    # print "@key=",key
    hist = tf_in.Get(key)
    arr = rn.hist2array(hist,return_edges=True)

    data = arr[0]
    bins = arr[1][0]
    
    assert data.sum() > 0.99999
    dx   = (bins[1] - bins[0]) / 2.0

    centers = (bins + dx)[:-1]

    type_ = key.split("_")[0]
    
    param = None
    if type_ == "sig":
        param = "_".join(key.split("_")[1:])
        sig_spectrum_m[param] = (centers,data)
    elif  type_ == "bkg":
        param = "_".join(key.split("_")[1:])
        bkg_spectrum_m[param] = (centers,data)
    else:
        raise Exception

tf_in.Close()

#
# Assert sig. and bkg. read from file correctly
#
for key in sig_spectrum_m.keys():
    assert key in sig_spectrum_m.keys(), "key=%s missing from sig"%key
    assert key in bkg_spectrum_m.keys(), "key=%s missing from bkg"%key
    assert sig_spectrum_m[key][0].size == bkg_spectrum_m[key][0].size
    assert sig_spectrum_m[key][1].size == bkg_spectrum_m[key][1].size

for key in bkg_spectrum_m.keys():
    assert key in sig_spectrum_m.keys(), "key=%s missing from sig"%key
    assert key in bkg_spectrum_m.keys(), "key=%s missing from bkg"%key
    assert sig_spectrum_m[key][0].size == bkg_spectrum_m[key][0].size
    assert sig_spectrum_m[key][1].size == bkg_spectrum_m[key][1].size

#
# Define, given value, how to get prob. from PDF
#
def nearest_id(spectrum,value):
    return np.argmin(np.abs(spectrum - value))

def nearest_id_v(spectrum_v,value_v):
    return np.array([np.argmin(np.abs(spectrum[0] - value)) for spectrum, value in zip(spectrum_v,value_v)])
    
#
# Define the LL
#
def LL(row):
    cols = row[sig_spectrum_m.keys()]
    sig_res = nearest_id_v(sig_spectrum_m.values(),cols.values)
    bkg_res = nearest_id_v(bkg_spectrum_m.values(),cols.values)
    
    sig_res = np.array([spectrum[1][v] for spectrum,v in zip(sig_spectrum_m.values(),sig_res)])
    bkg_res = np.array([spectrum[1][v] for spectrum,v in zip(bkg_spectrum_m.values(),bkg_res)])
    
    LL = np.log( sig_res / (sig_res + bkg_res) )

    return LL.sum()

#
# Apply the LL
#
print "Applying LL"
k0 = comb_cut_df.apply(LL,axis=1)
comb_cut_df['LL']=k0

#
# Choose the vertex @ event with the highest LL
#
print "Choosing vertex with max LL"
passed_df = comb_cut_df.copy()
passed_df = passed_df.sort_values(["LL"],ascending=False).groupby(rse).head(1)

OUT=os.path.join("ll_bin","%s_all.pkl" % name)
comb_df.to_pickle(OUT)
print "Store",OUT
print
OUT=os.path.join("ll_bin","%s_post_nue.pkl" % name)
comb_cut_df.to_pickle(OUT)
print "Store",OUT
print
OUT=os.path.join("ll_bin","%s_post_LL.pkl" % name)
passed_df.to_pickle(OUT)
print "Store",OUT
print
print "Done"
