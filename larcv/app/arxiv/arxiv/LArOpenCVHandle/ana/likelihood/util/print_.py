import pandas as pd


def print_stats(name,comb_df,cut_df):
    print "@ sample",name

    print
    if comb_df is not None:
        print "N input vertices....",comb_df.index.size
        print "N input events......",len(comb_df.groupby(rse))
    if cut_df is not None:
        print "N cut vertices......",cut_df.index.size
        print "N cut events........",len(cut_df.groupby(rse))
    print

    scedr = 5
    print "Good vertex scedr < 5"
    if comb_df is not None:
        print "N input vertices....",comb_df.query("scedr<@scedr").index.size
        print "N input events......",len(comb_df.query("scedr<@scedr").groupby(rse))
    if cut_df is not None:
        print "N cut vertices......",cut_df.query("scedr<@scedr").index.size
        print "N cut events........",len(cut_df.query("scedr<@scedr").groupby(rse))
    print

    print "Bad vertex scedr > 5"
    if comb_df is not None:
        print "N input vertices....",comb_df.query("scedr>@scedr").index.size
        print "N input events......",len(comb_df.query("scedr>@scedr").groupby(rse))
    if cut_df is not None:
        print "N cut vertices......",cut_df.query("scedr>@scedr").index.size
        print "N cut events........",len(cut_df.query("scedr>@scedr").groupby(rse))
    print

def ana_stats(all_df,event_df,scedr,name,LLCUT):
    rse    = ['run','subrun','event']
    print "Loaded events...........",event_df.index.size
    print "...num with cROI........",event_df.query("num_croi>0").index.size
    print "...good cROI counter....",event_df.query("good_croi_ctr>0").index.size
    print "...reco.................",event_df.query("num_vertex>0").index.size
    print
    if 'cosmic' not in name:
        print "1L1P....................",event_df.query("selected1L1P==1").index.size
        print "...num with cROI........",event_df.query("num_croi>0 & selected1L1P==1").index.size
        print "...good cROI counter....",event_df.query("good_croi_ctr>0 & selected1L1P==1").index.size
        print "...reco.................",event_df.query("good_croi_ctr>0 & selected1L1P==1 & num_vertex>0").dropna().index.size
        print
        print "1L1P E in [200,800] MeV.",event_df.query("selected1L1P==1 & energyInit>=200 & energyInit<=800").index.size
        print "...num with cROI........",event_df.query("num_croi>0 & selected1L1P==1 & energyInit>=200 & energyInit<=800").index.size
        print "...good cROI counter....",event_df.query("selected1L1P==1 & good_croi_ctr>0 & energyInit>=200 & energyInit<=800").index.size
        print "...reco.................",event_df.query("selected1L1P==1 & good_croi_ctr>0 & energyInit>=200 & energyInit<=800 & num_vertex>0").dropna().index.size
        print
    if LLCUT == None:
        print "===> TOTAL Vertices <===".format(scedr)
        print ">>> num_vertex>0 <<<"
        SS="num_vertex>0"
        print "...total................",all_df.query(SS).index.size
        print "...events...............",len(all_df.query(SS).groupby(rse))
        print
        if 'cosmic' not in name:
            print ">>> num_vertex>0 & 1L1P <<<"
            SS="num_vertex>0 & selected1L1P==1"
            print "...total................",all_df.query(SS).index.size
            print "...events...............",len(all_df.query(SS).groupby(rse))
            print
            print ">>> num_vertex>0 & 1L1P & E<<<"
            SS="num_vertex>0 & selected1L1P==1 & energyInit>=200 & energyInit<=800"
            print "...total................",all_df.query(SS).index.size
            print "...events...............",len(all_df.query(SS).groupby(rse))
            print
            print "===> GOOD vertices scedr<{} <===".format(scedr)
            SS="scedr<@scedr"
            print "...total................",all_df.query("num_vertex>0").query(SS).index.size
            print "...events...............",len(all_df.query("num_vertex>0").query(SS).groupby(rse))
            print
            print ">>>good cROI<<<"
            SS="scedr<@scedr & good_croi_ctr>0"
            print "...total................",all_df.query("num_vertex>0").query(SS).index.size
            print "...events...............",len(all_df.query("num_vertex>0").query(SS).groupby(rse))
            print 
            print ">>>good cROI + 1L1P<<<"
            SS="scedr<@scedr & good_croi_ctr>0 & selected1L1P==1"
            print "...total................",all_df.query("num_vertex>0").query(SS).index.size
            print "...events...............",len(all_df.query("num_vertex>0").query(SS).groupby(rse))
            print
            print ">>>good cROI + 1L1P + E<<<"
            SS="scedr<@scedr & good_croi_ctr>0 & selected1L1P==1 & energyInit>=200 & energyInit<=800"
            print "...total................",all_df.query("num_vertex>0").query(SS).index.size
            print "...events...............",len(all_df.query("num_vertex>0").query(SS).groupby(rse))
            print
    else:
        print "===> TOTAL Vertices <===".format(scedr)
        print ">>> num_vertex>0 <<<"
        SS="num_vertex>0 & LL>@LLCUT"
        print "...total................",all_df.query(SS).index.size
        print "...events...............",len(all_df.query(SS).groupby(rse))
        print
        if 'cosmic' not in name:
            print ">>> num_vertex>0 & 1L1P <<<"
            SS="num_vertex>0 & selected1L1P==1 & LL>@LLCUT"
            print "...total................",all_df.query(SS).index.size
            print "...events...............",len(all_df.query(SS).groupby(rse))
            print
            print ">>> num_vertex>0 & 1L1P & E<<<"
            SS="num_vertex>0 & selected1L1P==1 & energyInit>=200 & energyInit<=800 & LL>@LLCUT"
            print "...total................",all_df.query(SS).index.size
            print "...events...............",len(all_df.query(SS).groupby(rse))
            print
            print "===> GOOD vertices scedr<{} <===".format(scedr)
            SS="scedr<@scedr & LL>@LLCUT"
            print "...total................",all_df.query("num_vertex>0").query(SS).index.size
            print "...events...............",len(all_df.query("num_vertex>0").query(SS).groupby(rse))
            print
            print ">>>good cROI<<<"
            SS="scedr<@scedr & good_croi_ctr>0 & LL>@LLCUT"
            print "...total................",all_df.query("num_vertex>0").query(SS).index.size
            print "...events...............",len(all_df.query("num_vertex>0").query(SS).groupby(rse))
            print 
            print ">>>good cROI + 1L1P<<<"
            SS="scedr<@scedr & good_croi_ctr>0 & selected1L1P==1 & LL>@LLCUT"
            print "...total................",all_df.query("num_vertex>0").query(SS).index.size
            print "...events...............",len(all_df.query("num_vertex>0").query(SS).groupby(rse))
            print
            print ">>>good cROI + 1L1P + E<<<"
            SS="scedr<@scedr & good_croi_ctr>0 & selected1L1P==1 & energyInit>=200 & energyInit<=800 & LL>@LLCUT"
            print "...total................",all_df.query("num_vertex>0").query(SS).index.size
            print "...events...............",len(all_df.query("num_vertex>0").query(SS).groupby(rse))
            print
    return 1;
