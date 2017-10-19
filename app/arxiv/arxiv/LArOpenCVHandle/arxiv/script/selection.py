import ROOT, sys, os
from ROOT import std

from larcv import larcv
from larlite import larlite as ll
from larlite import larutil as lu

import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.path as path
import matplotlib.patches as patches

from ROOT import geo2d,cv
from ROOT.cv import Point_ as Vector
DTYPE='float'

import root_numpy as rn
import pandas as pd

geoh=lu.GeometryHelper.GetME()
geo=lu.Geometry.GetME()
larp=lu.LArProperties.GetME()
pygeo = geo2d.PyDraw()

matplotlib.rcParams['font.size']=20
matplotlib.rcParams['font.family']='serif'

INFILE=sys.argv[1]
reco_event_df    = pd.DataFrame(rn.root2array(INFILE,treename='EventTree'))
reco_vtx3d_df    = pd.DataFrame(rn.root2array(INFILE,treename='Vtx3DTree'))
reco_particle_df = pd.DataFrame(rn.root2array(INFILE,treename='ParticleTree'))
mc_event_df      = pd.DataFrame(rn.root2array(INFILE,treename='mctree'))

#remove duplicates if there is any

reco_event_df    = reco_event_df.drop_duplicates(subset=['run','subrun','event'])
mc_event_df = mc_event_df.drop_duplicates(subset=['run','subrun','event'])

# no containment asked
CONTAINMENT=False
if CONTAINMENT:
    pass

#selection
mc_zdf         = mc_event_df.set_index(['run','subrun','event'])
reco_event_zdf = reco_event_df.set_index(['run','subrun','event'])
reco_vtx3d_zdf = reco_vtx3d_df.set_index(['run','subrun','event'])
reco_par_zdf   = reco_particle_df.set_index(['run','subrun','event'])

groups_=mc_zdf.groupby(mc_zdf.index)

n_event_vtx3d     = 0
n_event_vtx3d_f   = 0
n_event_vtx3d_f_a = 0

cc0 = []
cc1 = []

n_single_choice=0
n_single_choice_right_good=0
n_single_choice_right_bad=0
n_single_choice_wrong_good=0
n_single_choice_wrong_bad=0
n_make_a_choice=0
n_reco_chose_wrong=0
n_reco_chose_wrong_good=0
n_reco_chose_wrong_bad=0
n_reco_chose_right=0
n_reco_chose_right_good=0
n_reco_chose_right_bad=0
n_filtered=0

n_1_vtx=0
n_2_vtx=0
n_3_vtx=0
n_4more_vtx=0

n_good_reco=0
n_no_good_reco=0

groups_=mc_zdf.groupby(mc_zdf.index)
n_total=len(groups_)

cc0=[]
cc1=[]

def correct(reco_vtx3d_,group_,chosen_idx_):

    ###projected vertex
    
    #recox = reco_vtx3d_.vtx2d_x_v.values[chosen_idx_]
    #recoy = reco_vtx3d_.vtx2d_y_v.values[chosen_idx_]

    ###circle vertex
    
    recox = reco_vtx3d_.circle_vtx_x_v.values[chosen_idx_]
    recoy = reco_vtx3d_.circle_vtx_y_v.values[chosen_idx_]
    
    mcx   = group_.vtx2d_t.values[0]
    mcy   = group_.vtx2d_w.values[0]
    
    dist=np.sqrt(np.power(recox-mcx,2)+np.power(recoy-mcy,2))

    dist.sort()

    return (dist[0] + dist[1]) < 2.0*7.0


DEBUG = False

for name, group in groups_:
    
    b_single_choice       = False
    b_single_choice_wrong = False
    b_single_choice_right = False
    b_reco_chose_wrong    = False
    b_reco_chose_right    = False
    b_good_reco           = False
    b_no_good_reco        = False
    b_make_a_choice       = False
    
    query_='run=={}&subrun=={}&event=={}'.format(name[0],name[1],name[2])
    reco_event = reco_event_df.query(query_)

    if DEBUG: print "run: ",name[0],"... subrun: ",name[1],"... event: ",name[2]
    
    nvtx=reco_event['n_vtx3d'].values[0]

    # no 3D vertex
    if nvtx==0: continue

    n_event_vtx3d+=1;
    
    reco_vtx3d = reco_vtx3d_df.query(query_)
    reco_par   = reco_particle_df.query(query_)

    type_v = reco_vtx3d.vtx3d_type.values

    if nvtx==1 or (nvtx>1 and np.where(type_v==0L)[0].size==1):
        if DEBUG: print "Single choice"
            
        b_single_choice=True
        type_v=np.array(type_v)
        if nvtx==2:
            chosen_idx = np.where(type_v==0L)[0][0]
        else:
            chosen_idx=0
        
        close = correct(reco_vtx3d,group,chosen_idx)

        if close == True: 
            b_single_choice_right=True
        else: 
            b_single_choice_wrong=True
        
    else:

        #pre filtering of n_pix_frac 
        
        n_pixel_frac_v=reco_vtx3d.num_pixel_frac_v

        bad_idx=[]
        for ix,npf in enumerate(n_pixel_frac_v.values):
            if ( np.where(npf>0.7)[0].size==3 ): continue;
            bad_idx.append(ix)

        reco_vtx3d = reco_vtx3d.query('vtx3d_id not in @bad_idx')
        reco_par   = reco_par.query  ('vtx3d_id not in @bad_idx')

        nvtx2=reco_vtx3d.index.values.size

        if nvtx2 < nvtx:
            if DEBUG: print "Filtered via NPX frac"
            
        nvtx=nvtx2
        
        if nvtx==0: 
            if DEBUG: print "Filtered NPX frac bad_idx: ",bad_idx
            n_event_vtx3d_f+=1;
            continue

        #pre filtering of kink-ness
        
        angle_vv=reco_vtx3d.circle_vtx_angle_v
        
        bad_idx=[]
        for ix,angle_v in enumerate(angle_vv.values):
            if( np.where(angle_v<170.)[0].size>1 ): continue;
            bad_idx.append(ix)
        
        reco_vtx3d = reco_vtx3d.query('vtx3d_id not in @bad_idx')
        reco_par   = reco_par.query  ('vtx3d_id not in @bad_idx')

        nvtx2=reco_vtx3d.index.values.size
        if nvtx2 < nvtx:
            if DEBUG: print "Filtered angle_vv bad_idx: ",bad_idx
        nvtx=nvtx2
        if nvtx==0:
            n_event_vtx3d_f_a+=1;
            continue   
            
        b_make_a_choice=True

        # now you must make your choice...

        if nvtx==1:
            n_1_vtx+=1
        elif nvtx==2:
            n_2_vtx+=1
        elif nvtx==3:
            n_3_vtx+=1
        else:
            n_4more_vtx+=1
           
        sum_pix_frac_copy = reco_vtx3d.sum_pixel_frac.values.tolist()

        angle_sum_v = [0.]*len(sum_pix_frac_copy)
        
        for ix,angle_v in enumerate(reco_vtx3d.circle_vtx_angle_v.values):
            angle_sum_v[ix] = angle_v.sum()

        #angle_sum_v=np.array(angle_sum_v)
        #chosen_idx=np.argmin(angle_sum_v)
    
        #make the decision based on the length of th shortest track
            
        dec_=[]
        if DEBUG: print "reco vtx3d id values: ",reco_vtx3d.vtx3d_id.values
        
        for ix,vtx_id in enumerate(reco_vtx3d.vtx3d_id.values):
            
            dec_.append(0)
            
            thisvtx=reco_par.query('vtx3d_id=={}'.format(vtx_id))
    
            for plane_id in xrange(3):
                thisplane=thisvtx.query("plane_id=={}".format(plane_id))

                if (thisplane.n_pars.values[0] < 2): continue
                    
                #get the shortest track

                s_idx=np.argmin(thisplane.atom_sum_length_v.values[0])
                
                #get the direction of the shortest track
                sy=thisplane.start_y_v.values[0][s_idx]
                ey=thisplane.end_y_v  .values[0][s_idx]                    

                if (ey>sy): dec_[-1]+=1
        
        
        dec_=np.array(dec_)
        
        chosen_idx=np.argmax(dec_)
        if DEBUG:
            print "chosen_idx: ", chosen_idx, "... dec_ ",dec_
            print ""
            
        close = correct(reco_vtx3d,group,chosen_idx)

        if close == True: 
            b_reco_chose_right=True
        else: 
            b_reco_chose_wrong=True


    #is there a reco vertex, we just chose it wrong?
    good_reco=False
    for idx_ in xrange(nvtx):
        
        if good_reco==True : continue
        good_reco = correct(reco_vtx3d,group,idx_)
        
    if good_reco==True: 
        b_good_reco=True
    else:
        b_no_good_reco=True

    if b_single_choice==True:
        n_single_choice+=1
        
    if b_single_choice_right==True and b_no_good_reco==False:
        n_single_choice_right_good+=1
         
    if b_single_choice_right==True and b_no_good_reco==True:
        n_single_choice_right_bad+=1
        
    if b_single_choice_wrong==True and b_no_good_reco==False:
        n_single_choice_wrong_good+=1
        
    if b_single_choice_wrong==True and b_no_good_reco==True:
        n_single_choice_wrong_bad+=1
        
    if b_make_a_choice==True:
        n_make_a_choice+=1
        
    if b_reco_chose_right==True:
        n_reco_chose_right+=1
        
    if b_reco_chose_right==True and b_no_good_reco==False:
        n_reco_chose_right_good+=1
        
    if b_reco_chose_right==True and b_no_good_reco==True:
        n_reco_chose_right_bad+=1
        
    if b_reco_chose_wrong==True:
        n_reco_chose_wrong+=1
  
    if b_reco_chose_wrong==True and b_no_good_reco==False:
        n_reco_chose_wrong_good+=1
       
    if b_reco_chose_wrong==True and b_no_good_reco==True:
        n_reco_chose_wrong_bad+=1
    
    if b_reco_chose_right == True and b_reco_chose_wrong == True:
        assert(False)
    
    if b_good_reco==True:
        n_good_reco+=1
        #cc0.append(mc_event_df.query(query_).index.values[0])
        #cc1.append(reco_par_zdf.query(query_).index.values[0])
        
    if b_no_good_reco==True:
        n_no_good_reco+=1
        cc0.append(mc_event_df.query(query_).index.values[0])
        cc1.append(reco_par_zdf.query(query_).index.values[0])


print "\n=========================================================\n"
print "Total Signal: ",n_total
print ""
print "Events 1 Vtx:                   ",n_single_choice
print "Correct choice:                 ",n_single_choice_right_good
print "Correct choice was no good one: ",n_single_choice_right_bad
print "Wrong choice was good one:      ",n_single_choice_wrong_good
print "Wrong choice was no good one:   ",n_single_choice_wrong_bad
print""
print "Events > 0 Vtx:             ",n_event_vtx3d
print "Events NOT pass pix frac:  ",n_event_vtx3d_f
print "Events NOT pass angle cut: ",n_event_vtx3d_f_a
print "Events parsed:             ",n_event_vtx3d-n_event_vtx3d_f-n_event_vtx3d_f_a
print ""
print "After filter..."
print "N 1 vtx:   ",n_1_vtx
print "N 2 vtx:   ",n_2_vtx
print "N 3 vtx:   ",n_3_vtx
print "N >=4 vtx: ",n_4more_vtx
print ""
print "N events make a choice:        ",n_make_a_choice
print "N chose right:                 ",n_reco_chose_right
print "N chose right was good one:    ",n_reco_chose_right_good
print "N chose right was no good one: ",n_reco_chose_right_bad
print "N chose wrong:                 ",n_reco_chose_wrong
print "N chose wrong was good one:    ",n_reco_chose_wrong_good
print "N chose wrong was no good one: ",n_reco_chose_wrong_bad
print ""
print "Good possible vtx diagnostic..."
print "Total reco correct:  ",n_single_choice_right_good+n_reco_chose_right_good
print "Total reco wrong:    ",n_single_choice_wrong_good+n_reco_chose_wrong_good
print ""
print "Best case senario..."
print "N was good one:   ",n_good_reco
print "N was wrong good: ",n_no_good_reco
print "\n=========================================================\n"
print "\n\n"
print cc0
print cc1
