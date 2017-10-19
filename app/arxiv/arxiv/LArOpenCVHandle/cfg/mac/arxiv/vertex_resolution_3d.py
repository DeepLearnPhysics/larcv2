import pandas as pd
import numpy as np
import sys
from util.good_vertex import retrieve_events

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

dx_v=[]
dy_v=[]
dz_v=[]

for name, row in sig_good_vtx_df.reset_index().set_index(base_index).iterrows():
    run,subrun,event=name
    mc_tree_s = sig_mctree_s.query("run==@run&subrun==@subrun&event==@event")
    dx_v.append(mc_tree_s.parentX.values[0] - row.x)
    dy_v.append(mc_tree_s.parentY.values[0] - row.y)
    dz_v.append(mc_tree_s.parentZ.values[0] - row.z)


dx_v=np.array(dx_v)
dy_v=np.array(dy_v)
dz_v=np.array(dz_v)

fig,ax=plt.subplots(figsize=(10,6))
rmin=0.0
rmax=10.0
rstep=0.2
bins=np.arange(rmin,rmax+rstep,rstep)
ax.hist(np.sqrt(dx_v*dx_v+dy_v*dy_v+dz_v*dz_v),bins=bins)
ax.set_xlabel("(MC - Reco) 3D Vertex [cm]",fontweight='bold')
ax.set_ylabel("Count / %0.02f [cm]"%rstep,fontweight='bold')
plt.grid()
plt.tight_layout()
plt.show()

for r in ['x','y','z']:
    fig,ax=plt.subplots(figsize=(10,6))
    rmin=-10.0
    rmax=10.0
    rstep=0.2
    bins=np.arange(rmin,rmax+rstep,rstep)
    exec("dr=d%s_v"%r)
    ax.hist(dr,bins=bins)
    ax.set_xlabel("(MC - Reco) $\Delta$%s [cm]"%r,fontweight='bold')
    ax.set_ylabel("Count / %0.02f [cm]"%rstep,fontweight='bold')
    plt.grid()
    plt.tight_layout()
    plt.show()
        
