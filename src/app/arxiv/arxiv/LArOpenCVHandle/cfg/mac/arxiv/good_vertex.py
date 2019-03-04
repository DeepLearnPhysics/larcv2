from util import *
import root_numpy as rn

def retrieve_events(INFILE,good_vertex=True):
    base_index = ['run','subrun','event']
    trees_m = { 'MCTree' : base_index, 'Vertex3DTree' : base_index + ['id'] }
    df_m = {}
    signal_df_m = {}
    back_df_m = {}
    signal_reco_df_m = {}
    back_reco_df_m = {}

    # drop duplicates
    for tree_name_, index_ in trees_m.iteritems():
        df = pd.DataFrame(rn.root2array(INFILE,treename=tree_name_))
	df = df.drop_duplicates(subset=index_)
	df_m[tree_name_] = df.set_index(base_index)
        
    signal_mc_idx = df_m['MCTree'].query('signal==1').index

    for tree_name_, index_ in trees_m.iteritems():
        print tree_name_,index_
        signal_df = df_m[tree_name_].ix[signal_mc_idx]
        signal_df.index.names = base_index
        signal_df_m[tree_name_] = signal_df.copy()

    for tree_name_, index_ in trees_m.iteritems():
        signal_reco_df_m[tree_name_] = signal_df_m[tree_name_]

    for tree_name_, index_ in trees_m.iteritems():
        signal_reco_df_m[tree_name_] = signal_reco_df_m[tree_name_].reset_index().set_index(index_)
        
    s_vtx_tree = signal_reco_df_m['Vertex3DTree'].reset_index().set_index(base_index)
    s_mc_tree  = signal_reco_df_m['MCTree'].reset_index().set_index(base_index)

    ret_map = {}
    ret_map["signal_df_m"] = signal_df_m
    ret_map["base_index"]  = base_index

    if good_vertex==False:
        return ret_map
    
    s_vtx_df = pick_good_vertex(s_mc_tree,s_vtx_tree,signal_df_m)

    s_vtx_df.index.names = base_index
    
    #get the signal dataframe
    sig_vtx3d = signal_df_m["Vertex3DTree"].reset_index().set_index(base_index + ["id"])
    sig_good_vtx_df = get_best_df(s_vtx_df,sig_vtx3d,base_index)
    ret_map["sig_good_vtx_df"] = sig_good_vtx_df.copy()
    return ret_map

