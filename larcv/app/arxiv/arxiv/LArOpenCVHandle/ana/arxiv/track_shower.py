import os, sys

import matplotlib
import matplotlib.pyplot as plt

import numpy as np
import pandas as pd

import root_numpy as rn
from larocv import larocv

rse    = ['run','subrun','event']
rsev   = ['run','subrun','event','vtxid']
rserv  = ['run','subrun','event','roid','vtxid']

# Vertex data frame
dfs  = {}

# Event data frame
edfs = {}
mdfs = {}

sample_name = str(sys.argv[1])
sample_file = str(sys.argv[2])

for name,file_ in [(sample_name,sample_file)]:
    
    INPUT_FILE  = file_
    
    #
    # Vertex wise Trees
    #
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

    
    # print "v",vertex_df.set_index(rserv).index.size
    # print "a",angle_df.set_index(rserv).index.size
    # print "s",shape_df.set_index(rserv).index.size
    # print "g",gap_df.set_index(rserv).index.size
    # print "a",angle_df.set_index(rserv).index.size
    # print "m",match_df.set_index(rserv).index.size
    # print "d",dqds_df.set_index(rserv).index.size
    # print 
    # print "c",comb_df.index.size
    # print
    
    #
    # Store vertex wise data frame
    #
    comb_df = comb_df.reset_index()
    dfs[name] = comb_df.copy()

    #
    # Event wise Trees
    #
    event_vertex_df   = pd.DataFrame(rn.root2array(INPUT_FILE,treename="EventVertexTree"))
    # mc_df             = pd.DataFrame(rn.root2array(INPUT_FILE,treename="MCTree"))

    edfs[name] = event_vertex_df.copy()
    # mdfs[name] = mc_df.copy()
    
    print "@ sample:",name,"good croi:",event_vertex_df.query("good_croi_ctr>0").index.size
    print "total events: ", event_vertex_df.index.size
    print 
    
    

#
# Calculate the 3D opening angle, and 2D XZ projected opening angle
#


for name, comb_df in dfs.iteritems():
    # print comb_df.par_trunk_pca_theta_estimate_v
    
    comb_df['cosangle3d']=comb_df.apply(lambda x : larocv.CosOpeningAngle(x['par_trunk_pca_theta_estimate_v'][0],
                                                                          x['par_trunk_pca_phi_estimate_v'][0],
                                                                          x['par_trunk_pca_theta_estimate_v'][1],
                                                                          x['par_trunk_pca_phi_estimate_v'][1]),axis=1)

    # unused @ cut level at the moment
    #
    #comb_df['cosangleXZ']=comb_df.apply(lambda x : larocv.CosOpeningXZAngle(x['par_trunk_pca_theta_estimate_v'][0],
    #                                                                        x['par_trunk_pca_phi_estimate_v'][0],
    #                                                                        x['par_trunk_pca_theta_estimate_v'][1],
    #                                                                        x['par_trunk_pca_phi_estimate_v'][1]),axis=1)

#
# Look @ initial sample
#
d_good_vtx = float(5)
print "@ initial track/shower cut -- vertex (events)"
for name, comb_df in dfs.iteritems():
    ts_query   = "par1_type != par2_type"
    ts_query_g = ts_query + " & scedr < @d_good_vtx"
    print "Name....",name
    print "Raw.....",comb_df.index.size,",",len(comb_df.groupby(rse))
    print "All.....",comb_df.query(ts_query).index.size,",",len(comb_df.query(ts_query).groupby(rse))
    print "Good....",comb_df.query(ts_query_g).index.size,",",len(comb_df.query(ts_query_g).groupby(rse))
    print "Bad.....",comb_df.query(ts_query_g).index.size,"",len(comb_df.query(ts_query_g).groupby(rse))
    print
    


#
# 1 track & 1 shower assumption from ssnet
#
def track_shower_assumption(df):
    df['trkid'] = df.apply(lambda x : 0 if(x['par1_type']==1) else 1,axis=1)
    df['shrid'] = df.apply(lambda x : 1 if(x['par2_type']==2) else 0,axis=1)

#
# Print out
#
def vtx_evts(df):
    n_vertex_g = float(df.query("scedr < @d_good_vtx").index.size)
    n_events_g = float(len(df.query("scedr < @d_good_vtx").groupby(rse)))
    n_vertex_b = float(df.query("scedr > @d_good_vtx").index.size)
    n_events_b = float(len(df.query("scedr > @d_good_vtx").groupby(rse)))
    
    print "SV: %.03f SE: %.03f (%d,%d)"%(n_vertex_g/start_n_vertex_g,
                                         n_events_g/start_n_events_g,
                                         n_vertex_g,n_events_g)
    
    print "BV: %.03f BE: %.03f (%d,%d)"%(1.0-n_vertex_b/start_n_vertex_b,
                                         1.0-n_events_b/start_n_events_b,
                                         n_vertex_b,n_events_b)
    print


sample = sample_name
ts_mdf = dfs[sample].copy()

print "<< Before >>"
print "Good Vertex (Events)",ts_mdf.query("scedr<@d_good_vtx").index.size,len(ts_mdf.query("scedr<@d_good_vtx").groupby(rse))
print "Bad  Vertex (Events)",ts_mdf.query("scedr>@d_good_vtx").index.size,len(ts_mdf.query("scedr>@d_good_vtx").groupby(rse))
print

#
# For relative efficiency get the number of good cROI as input
#
start_n_vertex_g = float(ts_mdf.query("scedr<@d_good_vtx").index.size)
start_n_events_g = float(edfs[sample].query("good_croi_ctr>0").index.size)
# start_n_events_g = float(len(ts_mdf.query("scedr<5").groupby(rse)))

start_n_vertex_b = 1.0#float(ts_mdf.query("scedr>@d_good_vtx").index.size)
start_n_events_b = 1.0#float(len(ts_mdf.query("scedr>@d_good_vtx").groupby(rse)))

ts_mdf = ts_mdf.query("par1_type != par2_type")
track_shower_assumption(ts_mdf)

print "<< After assumption >>"
print "Good ",ts_mdf.query("scedr<@d_good_vtx").index.size,len(ts_mdf.query("scedr<@d_good_vtx").groupby(rse))
print "Bad  ",ts_mdf.query("scedr>@d_good_vtx").index.size,len(ts_mdf.query("scedr>@d_good_vtx").groupby(rse))
print

ts_mdf['trk_trunk_pca_theta_estimate'] = ts_mdf.apply(lambda x : np.cos(x['par_trunk_pca_theta_estimate_v'][x['trkid']]),axis=1) 

ts_mdf['shr_avg_length'] = ts_mdf.apply(lambda x : x['length_v'][x['shrid']] / x['nplanes_v'][x['shrid']],axis=1)
ts_mdf['trk_avg_length'] = ts_mdf.apply(lambda x : x['length_v'][x['trkid']] / x['nplanes_v'][x['trkid']],axis=1)

ts_mdf['shr_avg_width'] = ts_mdf.apply(lambda x : x['width_v'][x['shrid']] / x['nplanes_v'][x['shrid']],axis=1)
ts_mdf['trk_avg_width'] = ts_mdf.apply(lambda x : x['width_v'][x['trkid']] / x['nplanes_v'][x['trkid']],axis=1)

ts_mdf['shr_avg_perimeter'] = ts_mdf.apply(lambda x : x['perimeter_v'][x['shrid']] / x['nplanes_v'][x['shrid']],axis=1)
ts_mdf['trk_avg_perimeter'] = ts_mdf.apply(lambda x : x['perimeter_v'][x['trkid']] / x['nplanes_v'][x['trkid']],axis=1)

ts_mdf['shr_avg_area'] = ts_mdf.apply(lambda x : x['area_v'][x['shrid']] / x['nplanes_v'][x['shrid']],axis=1)
ts_mdf['trk_avg_area'] = ts_mdf.apply(lambda x : x['area_v'][x['trkid']] / x['nplanes_v'][x['trkid']],axis=1)

ts_mdf['shr_avg_npixel'] = ts_mdf.apply(lambda x : x['npixel_v'][x['shrid']] / x['nplanes_v'][x['shrid']],axis=1)
ts_mdf['trk_avg_npixel'] = ts_mdf.apply(lambda x : x['npixel_v'][x['trkid']] / x['nplanes_v'][x['trkid']],axis=1)

ts_df = ts_mdf.copy()

print '<< Track shower assumption >>'
vtx_evts(ts_df)

#
# Generic Cuts
#
query="npar==2"
ts_df = ts_df.query(query)
print "<< {} >>".format(query)
vtx_evts(ts_df)

query="in_fiducial==1"
ts_df = ts_df.query(query)
print "<< {} >>".format(query)
vtx_evts(ts_df)

query="pathexists2==1"
ts_df = ts_df.query(query)
print "<< {} >>".format(query)
vtx_evts(ts_df)

#
# Kinematic Cuts
#
query="cosangle3d >-0.995 and cosangle3d<0.995"
ts_df = ts_df.query(query)
print "<< {} >>".format(query)
vtx_evts(ts_df)

query="anglediff<176"
ts_df = ts_df.query(query)
print "<< {} >>".format(query)
vtx_evts(ts_df)

query='trk_trunk_pca_theta_estimate>-0.05'
ts_df = ts_df.query(query)
print "<< {} >>".format(query)
vtx_evts(ts_df)

#
# Shower Cuts
#
query="shr_avg_length>20"
ts_df = ts_df.query(query)
print "<< {} >>".format(query)
vtx_evts(ts_df)

query="shr_avg_width>5"
ts_df = ts_df.query(query)
print "<< {} >>".format(query)
vtx_evts(ts_df)

query="shr_avg_area>20"
ts_df = ts_df.query(query)
print "<< {} >>".format(query)
vtx_evts(ts_df)

query="shr_avg_npixel<600"
ts_df = ts_df.query(query)
print "<< {} >>".format(query)
vtx_evts(ts_df)

#
# Track Cuts
#
query="trk_avg_length<200"
ts_df = ts_df.query(query)
print "<< {} >>".format(query)
vtx_evts(ts_df)

query="trk_avg_perimeter<500"
ts_df = ts_df.query(query)
print "<< {} >>".format(query)
vtx_evts(ts_df)

query="trk_avg_npixel<500"
ts_df = ts_df.query(query)
print "<< {} >>".format(query)
vtx_evts(ts_df)

#
# dQds
#
query="dqds_ratio_01 < 0.55"
ts_df = ts_df.query(query)
print "<< {} >>".format(query)
vtx_evts(ts_df)

#
# Beta
#
query="dqds_diff_01 > 30"
ts_df = ts_df.query(query)
print "<< {} >>".format(query)
vtx_evts(ts_df)

# query="vertex_n_planes_charge==3"
# ts_df = ts_df.query(query)
# print "<< {} >>".format(query)
# vtx_evts(ts_df)

print "<< After >>"
print "Good Vertex (Events)",ts_df.query("scedr<@d_good_vtx").index.size,len(ts_df.query("scedr<@d_good_vtx").groupby(rse))
print "Bad  Vertex (Events)",ts_df.query("scedr>@d_good_vtx").index.size,len(ts_df.query("scedr>@d_good_vtx").groupby(rse))
print
