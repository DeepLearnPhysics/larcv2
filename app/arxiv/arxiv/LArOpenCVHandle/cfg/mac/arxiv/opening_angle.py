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


def dual_angle(row):

    pdg_v=row['daughterPdg_v']
    sx_vv=row['daughter2DStartX_vv']
    sy_vv=row['daughter2DStartY_vv']
    ex_vv=row['daughter2DEndX_vv']
    ey_vv=row['daughter2DEndY_vv']
    pdg_v=pdg_v[np.where(row["daughterTrackid_v"] == row["daughterParentTrackid_v"])[0]]
    lepton_id_v=np.where((pdg_v==11) | (pdg_v==13))[0]
    pdg_idx1=lepton_id_v[0]

    p0l_sx=sx_vv[0][pdg_idx1]
    p0l_ex=ex_vv[0][pdg_idx1]
    p0l_sy=sy_vv[0][pdg_idx1]
    p0l_ey=ey_vv[0][pdg_idx1]
    
    p1l_sx=sx_vv[1][pdg_idx1]
    p1l_ex=ex_vv[1][pdg_idx1]
    p1l_sy=sy_vv[1][pdg_idx1]
    p1l_ey=ey_vv[1][pdg_idx1]

    p2l_sx=sx_vv[2][pdg_idx1]
    p2l_ex=ex_vv[2][pdg_idx1]
    p2l_sy=sy_vv[2][pdg_idx1]
    p2l_ey=ey_vv[2][pdg_idx1]
    
    p0l=np.array([p0l_ex-p0l_sx,p0l_ey-p0l_sy])
    p1l=np.array([p1l_ex-p1l_sx,p1l_ey-p1l_sy])
    p2l=np.array([p2l_ex-p2l_sx,p2l_ey-p2l_sy])
    
    pdg_idx_v2=np.where(pdg_v==2212)[0]

    if pdg_idx_v2.size==0: return [0,0,0]

    pdg_idx2 = pdg_idx_v2[0]

    p0p_sx=sx_vv[0][pdg_idx2]
    p0p_ex=ex_vv[0][pdg_idx2]
    p0p_sy=sy_vv[0][pdg_idx2]
    p0p_ey=ey_vv[0][pdg_idx2]
    
    p1p_sx=sx_vv[1][pdg_idx2]
    p1p_ex=ex_vv[1][pdg_idx2]
    p1p_sy=sy_vv[1][pdg_idx2]
    p1p_ey=ey_vv[1][pdg_idx2]
    
    p2p_sx=sx_vv[2][pdg_idx2]
    p2p_ex=ex_vv[2][pdg_idx2]
    p2p_sy=sy_vv[2][pdg_idx2]
    p2p_ey=ey_vv[2][pdg_idx2]

    p0p=np.array([p0p_ex-p0p_sx,p0p_ey-p0p_sy])
    p1p=np.array([p1p_ex-p1p_sx,p1p_ey-p1p_sy])
    p2p=np.array([p2p_ex-p2p_sx,p2p_ey-p2p_sy])

    p0=np.dot(p0p,p0l) / (np.sqrt(np.dot(p0p,p0p))*np.sqrt(np.dot(p0l,p0l)))
    p1=np.dot(p1p,p1l) / (np.sqrt(np.dot(p1p,p1p))*np.sqrt(np.dot(p1l,p1l)))
    p2=np.dot(p2p,p2l) / (np.sqrt(np.dot(p2p,p2p))*np.sqrt(np.dot(p2l,p2l)))
    ret=[p0,p1,p2]
    return ret[2]
    
true_opening_plane_2 = sig_mctree_s.apply(dual_angle,axis=1)#.values[:,2]
reco_opening_plane_2 = sig_mctree_t.apply(dual_angle,axis=1)#.values[:,2]

Tmin=-1
Tmax=1
deltaT=0.05
angle_plotter(true_opening_plane_2,
              reco_opening_plane_2,
              Tmin,
              Tmax,
              deltaT,
              "(Plane 2) Opening Angle")

signal_last_bin = true_opening_plane_2[true_opening_plane_2>=(1-deltaT)]
reco_last_bin   = reco_opening_plane_2[reco_opening_plane_2>=(1-deltaT)]
bad_index = signal_last_bin.drop(reco_last_bin.index).index
print list(signal_df_m["MCTree"].ix[bad_index].entry.values)
