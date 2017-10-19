import pandas as pd
import numpy as np
import sys
from util.good_vertex import retrieve_events

INFILE = sys.argv[1]

ret=retrieve_events(INFILE)

signal_df_m=ret["signal_df_m"]
sig_good_vtx_df=ret["sig_good_vtx_df"]
base_index=ret["base_index"]

print
print
nsignal=float(signal_df_m['MCTree'].index.size)
print "Signal -------",int(nsignal)
print "Good vertex --",sig_good_vtx_df.index.size,"/",int(nsignal)," = ",float(sig_good_vtx_df.index.size)/nsignal
# multi2_v=sig_good_vtx_df.multi_v.apply(lambda x : np.where(x==2)[0].size>=2)
# print "Multi (2) ----",multi2_v.sum(),"/",int(nsignal)," = ",float(multi2_v.sum())/nsignal

a=sig_good_vtx_df.reset_index().set_index(base_index)
# b=sig_good_vtx_df[multi2_v]

bad_mc=signal_df_m['MCTree'].drop(a.index,inplace=False)
bad_reco=signal_df_m['Vertex3DTree'].drop(a.index,inplace=False)
bad_reco=bad_reco.dropna()
# bad_mult=sig_good_vtx_df.drop(b.index,inplace=False)

print
print "Poorly reconstructed: ",len(list(bad_mc.entry.values))
print list(bad_mc.entry.values)
print
# print "Poor multiplicity"
# print list(bad_mult.entry.values.astype(np.uint32))




