import pandas as pd
import numpy as np
import sys
from util.good_vertex import retrieve_events
from util.util import angle_plotter

import matplotlib
import matplotlib.pyplot as plt
matplotlib.rcParams['font.size']=20
matplotlib.rcParams['font.family']='serif'

INFILE = sys.argv[1]

ret=retrieve_events(INFILE)

signal_df_m=ret["signal_df_m"]
sig_good_vtx_df=ret["sig_good_vtx_df"]
base_index=ret["base_index"]

sig_mctree_s=signal_df_m["MCTree"]
sig_mctree_t=signal_df_m["MCTree"].ix[sig_good_vtx_df.reset_index().set_index(base_index).index]



#Plane 2
def lepton_angle(row):
    cos_v=row['daughter2DCosAngle_vv'][2]
    pdg_v=row['daughterPdg_v']
    pdg_v=pdg_v[np.where(row["daughterTrackid_v"] == row["daughterParentTrackid_v"])[0]]
    lepton_id_v=np.where((pdg_v==11) | (pdg_v==13))[0]
    lepton_id=lepton_id_v[0]
    # print
    # print cos_v
    # print pdg_v
    # print lepton_id_v
    # print
    return cos_v[lepton_id]
    
true_cos_plane_2 = sig_mctree_s.apply(lepton_angle,axis=1)
reco_cos_plane_2 = sig_mctree_t.apply(lepton_angle,axis=1)

Tmin=-1
Tmax=1
deltaT=0.025
angle_plotter(true_cos_plane_2,
              reco_cos_plane_2,
              Tmin,
              Tmax,
              deltaT,
              "(Plane 2) Lepton")

signal_last_bin = true_cos_plane_2[true_cos_plane_2>=(1-deltaT)]
reco_last_bin   = reco_cos_plane_2[reco_cos_plane_2>=(1-deltaT)]
bad_index=signal_last_bin.drop(reco_last_bin.index).index
print list(signal_df_m["MCTree"].ix[bad_index].entry.values)
