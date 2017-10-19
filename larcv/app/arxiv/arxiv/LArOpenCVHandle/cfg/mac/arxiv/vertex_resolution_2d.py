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

dw_v=[]
dt_v=[]

for name, row in sig_good_vtx_df.reset_index().set_index(base_index).iterrows():
    run,subrun,event=name
    mc_tree_s = sig_mctree_s.query("run==@run&subrun==@subrun&event==@event")
    dw_v.append(list(mc_tree_s.vtx2d_t.values[0] - row.vtx2d_x_v))
    dt_v.append(list(mc_tree_s.vtx2d_w.values[0] - row.vtx2d_y_v))


dw_v=np.vstack(dt_v)
dt_v=np.vstack(dw_v)

for i in xrange(3):
    fig,ax=plt.subplots(figsize=(10,6))
    ax.hist(dw_v[:,i],
            label='Plane {}'.format(i),
            bins=np.arange(-20,20+1,1),
            alpha=0.5)
    print i,np.mean(dw_v[:,i]),np.std(dw_v[:,i])
    ax.set_xlabel("(MC - Reco) Wire [pixel]",fontweight='bold')
    ax.set_ylabel("Count / pixel",fontweight='bold')

    (ymin,ymax)=ax.get_ylim()
    ax.vlines(-7,ymin,ymax,lw=2,color='orange',alpha=0.7)
    ax.vlines(7,ymin,ymax,lw=2,color='orange',alpha=0.7)
    ax.legend(fontsize=18)
    plt.tight_layout()
    plt.grid()
    plt.show()
    

for i in xrange(3):
    fig,ax=plt.subplots(figsize=(10,6))
    ax.hist(dt_v[:,i],
            label='Plane {}'.format(i),
            bins=np.arange(-20,20+1,1),
            alpha=0.5)
    print i,np.mean(dt_v[:,i]),np.std(dt_v[:,i])
    ax.set_xlabel("(MC - Reco) Time [pixel]",fontweight='bold')
    (ymin,ymax)=ax.get_ylim()
    ax.vlines(-7,ymin,ymax,lw=2,color='orange',alpha=0.7)
    ax.vlines(7,ymin,ymax,lw=2,color='orange',alpha=0.7)
    ax.legend(fontsize=18)
    ax.set_ylabel("Count / pixel",fontweight='bold')
    plt.grid()
    plt.show()
    
    
for i in xrange(3):
    fig,ax=plt.subplots(figsize=(10,6))
    ax.hist(np.sqrt(np.power(dw_v[:,i],2)+np.power(dt_v[:,i],2)),
            label='Plane {}'.format(i),
            bins=np.arange(0,20+1,1),
            alpha=0.5,lw=2)
    ax.set_xlabel("Dist MC Reco Vtx [pixel]",fontweight='bold')
    ax.set_xlim(0,50)
    ax.legend()
    plt.tight_layout()
    plt.grid()
    plt.show()


