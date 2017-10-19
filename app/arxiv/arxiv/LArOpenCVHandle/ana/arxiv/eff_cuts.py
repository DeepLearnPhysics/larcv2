import os, sys
import numpy as np
import pandas as pd

import root_numpy as rn

#
# Load DataFrames
#
rse    = ['run','subrun','event']
rsev   = ['run','subrun','event','vtxid']
rserv  = ['run','subrun','event','roid','vtxid']

INPUT_FILE  = sys.argv[1]

# Vertex wise Trees
vertex_df = pd.DataFrame(rn.root2array(INPUT_FILE,treename='VertexTree'))
lee_df    = pd.DataFrame(rn.root2array(INPUT_FILE,treename='LEE1e1pTree'))
angle_df  = pd.DataFrame(rn.root2array(INPUT_FILE,treename='AngleAnalysis'))
shape_df  = pd.DataFrame(rn.root2array(INPUT_FILE,treename='ShapeAnalysis'))
gap_df    = pd.DataFrame(rn.root2array(INPUT_FILE,treename="GapAnalysis"))

# Event wise Trees
event_vertex_df   = pd.DataFrame(rn.root2array(INPUT_FILE,treename="EventVertexTree"))
mc_df             = pd.DataFrame(rn.root2array(INPUT_FILE,treename="MCTree"))

#
# Combine DataFrames
#
comb_df = pd.concat([vertex_df.set_index(rserv),
                     lee_df.set_index(rserv),
                     angle_df.set_index(rserv),
                     shape_df.set_index(rserv),
                     gap_df.set_index(rserv)],axis=1)

comb_df = comb_df.reset_index()

assert comb_df.index.size == vertex_df.index.size

#
# Cut strings
#
cutstr_1 = ' npar==2 '
cutstr0  = ' and (q0/npx0  > 30) and (q1/npx1  > 30) '
cutstr1  = ' and (q0/area0 > 60) and (q1/area1 > 60) '
cutstr2  = ' and (1.0 < npx0/area0 and npx0/area0 < 2.5) and (1.0 < npx1/area1 and npx1/area1 < 2.5) '
cutstr3  = ' and len1 < 500'
cutstr4  = ' and len0 < 500'
cutstr5  = ' and pathexists2 == 1'
cutstr6  = ' and anglediff < 175 and anglediff > 5'

cut1        = cutstr_1
cut10       = cutstr_1+cutstr0
cut101      = cutstr_1+cutstr0+cutstr1
cut1012     = cutstr_1+cutstr0+cutstr1+cutstr2
cut10123    = cutstr_1+cutstr0+cutstr1+cutstr2+cutstr3
cut101234   = cutstr_1+cutstr0+cutstr1+cutstr2+cutstr3+cutstr4
cut1012345  = cutstr_1+cutstr0+cutstr1+cutstr2+cutstr3+cutstr4+cutstr5
cut10123456 = cutstr_1+cutstr0+cutstr1+cutstr2+cutstr3+cutstr4+cutstr5+cutstr6

cutstr = cut10123456

good_df = comb_df.query("scedr<5")
bad_df  = comb_df.query("scedr>=5")

OUTPUT_FILE = os.path.basename(INPUT_FILE)

f_ = open("eff_%s.txt"%OUTPUT_FILE,'w+')

f_.write("Good croi counter: %d" % event_vertex_df.query("good_croi_ctr>0").index.size);
f_.write("\n")
f_.write("Total Events: %s\n"%str(event_vertex_df.index.size))
f_.write("Total vertex: %s\n"%str(comb_df.index.size))
f_.write("\n")
f_.write("Cut -1) %s\n"%cutstr_1)
f_.write("Cut  0) %s\n"%cutstr0)
f_.write("Cut  1) %s\n"%cutstr1)
f_.write("Cut  2) %s\n"%cutstr2)
f_.write("Cut  3) %s\n"%cutstr3)
f_.write("Cut  4) %s\n"%cutstr4)
f_.write("Cut  5) %s\n"%cutstr5)
f_.write("Cut  6) %s\n"%cutstr6)

#
# Apply the cuts
#

for the_pair in [("ALL",comb_df),("GOOD",good_df),("BAD",bad_df)]:
    the_name = the_pair[0]
    the_df   = the_pair[1]
    f_.write("@ %s\n"%the_name)
        
    den_   = the_df.index.size
    num_   = len(the_df.groupby(rse))
    ratio_ = 0
    if den_ > 0:
        ratio_ = float(num_)/float(den_)
        
    f_.write("   Vertex: {} events: {} = {}\n".format(den_,num_,ratio_))
    
    for ix,cut in enumerate([cut1,cut10,cut101,cut1012,cut10123,cut101234,cut1012345,cut10123456]):
        den_ = the_df.query(cut).index.size
        num_ = len(the_df.query(cut).groupby(rse))
        ratio_ = 0
        if den_ > 0:
             ratio_ = float(num_)/float(den_)
             
        f_.write("      {}) {} events: {} = {}\n".format(ix-1,den_,num_,ratio_))

    f_.write("\n")

f_.write("\n")
f_.write("\n")



# Set index

# mc_df_rse = mc_df.set_index(rse)

# signal_index = event_vertex_df.query("good_croi_ctr>0").set_index(rse).index
# reco_index   = good_df.set_index(rse).index

# ##################             #################
# ################## Energy plot #################
# ##################             #################
# from plot_util import energy_plotter


# energy_plotter(mc_df_rse.ix[signal_index],
#                mc_df_rse.ix[reco_index],
#                0,             # Emin
#                1000,          # Emax
#                25,            # deltaE
#                "energyInit",  # column
#                "Init") # X labeling
# ##################              #################
# ################## Proton angle #################
# ##################              #################

# def proton_angle(row):
#     cos_v=row['daughter2DCosAngle_vv'][2]
#     pdg_v=row['daughterPdg_v']
#     pdg_v=pdg_v[np.where(row["daughterTrackid_v"] == row["daughterParentTrackid_v"])[0]]
#     proton_id_v=np.where(pdg_v==2212)[0]
#     proton_id=proton_id_v[0]
#     return cos_v[proton_id]


# sig_mctree_s = mc_df_rse.ix[signal_index]
# sig_mctree_r = mc_df_rse.ix[reco_index]

# true_cos_plane_2 = sig_mctree_s.apply(proton_angle,axis=1)
# reco_cos_plane_2 = sig_mctree_r.apply(proton_angle,axis=1)

# from plot_util import angle_plotter
# Tmin=-1
# Tmax=1
# deltaT=0.02
# angle_plotter(true_cos_plane_2,
#               reco_cos_plane_2,
#               Tmin,
#               Tmax,
#               deltaT,
#               "(Plane 2) Proton")

# #################                      #################
# ################# Vertex Resolution 3D #################
# #################                      #################
# from plot_util import vertex_plotter

# # r
# dx=0.0125
# x0=0
# x1=10+dx
# vertex_plotter(x0,x1,dx,
#                comb_df.scedr.values,
#                "|MC-Reco| 3D Vertex Distance [cm]",
#                "Vertex Within 10 cm")

# # x
# dx=0.25
# x0=-10
# x1=10+dx
# vertex_plotter(x0,x1,dx,
#                comb_df.scex.values - ana_df.x.values,
#                "|MC-Reco| 3D Vertex X [cm]",
#                "Vertex Within 10 cm")

# # y
# dx=0.25
# x0=-10
# x1=10+dx
# vertex_plotter(x0,x1,dx,
#                comb_df.scey.values - ana_df.y.values,
#                "|MC-Reco| 3D Vertex Y [cm]",
#                "Vertex Within 10 cm")

# # z
# dx=0.25
# x0=-10
# x1=10+dx
# vertex_plotter(x0,x1,dx,
#                comb_df.scez.values - ana_df.z.values,
#                "|MC-Reco| 3D Vertex Z [cm]",
#                "Vertex Within 10 cm")
               

# #################
# ################# 
# #################
