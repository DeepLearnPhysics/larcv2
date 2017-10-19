import pandas as pd
import numpy as np

import matplotlib
import matplotlib.pyplot as plt
matplotlib.rcParams['font.size']=20
matplotlib.rcParams['font.family']='serif'

DEBUG=False
def pick_good_vertex(sb_mc_tree,sb_vtx_tree,signal_df_m):
    
    good_vtx_sb_v={}
    good_vtx_id_v={}
    
    for index, row in sb_mc_tree.iterrows():

        # NOTE YOU HAVE TO BE CAREFUL HERE WITH THIS LINE BELOW
        vtx_entry = sb_vtx_tree.loc[index]
        entry=signal_df_m['MCTree'].loc[index]['entry']

        # if entry==1968:
        #     DEBUG=True
        
        if type(vtx_entry) != pd.core.frame.DataFrame: 
            good_vtx_sb_v[index]  = False
            good_vtx_id_v[index] = -1
            raise Exception
            continue

        vtx_x_vv= np.row_stack(vtx_entry.vtx2d_x_v.values)
        vtx_y_vv= np.row_stack(vtx_entry.vtx2d_y_v.values)

        dx = vtx_x_vv - row.vtx2d_t
        dy = vtx_y_vv - row.vtx2d_w
        dt = np.sqrt(dx*dx + dy*dy)            # compute the distance from true to all candidates

        # remove really poor multiplicity events
        # a=vtx_entry.multi_v.apply(lambda x : len(np.where(x>0)[0])>1).values
        # b=~a
        # c=999*np.ones(list(dt[b].shape))
        # dt[b]=c
        
        min_idx_v   = np.argsort(dt.mean(axis=1)) # get the smallest mean distance from candidates
        dt_b        = (dt <= 7).sum(axis=1)       # vtx must be less than 7 pixels away
        close_vtx   = np.where(dt_b>1)[0]
        n_close_vtx = len(close_vtx)
        
        if DEBUG:
            print 
            print "Entry ",entry
            print "vtx_x_vv ",vtx_x_vv
            print "vtx_y_vv ",vtx_y_vv
            print "dx ",dx
            print "dy ",dy
            print "dt ",dt
            print "min_idx ",min_idx_v
            print "multi_v ",vtx_entry.multi_v
            print "a ",a
            print "b ",b
            print "c ",c
            print "dt_b ",dt_b
            print "close vtx ",close_vtx
            
        good_vtx_sb_v[index]  = n_close_vtx>0

        min_idx=min_idx_v[0]

        if DEBUG:
            print "Set min_idx ",min_idx
            
        if n_close_vtx>1:
            for vtx_id in close_vtx:
                if np.sum(vtx_entry.multi_v.values[vtx_id]==2)>=2:
                    min_idx=vtx_id
                    if DEBUG:
                        print "Set min_idx ",vtx_id
        good_vtx_id_v[index]  = vtx_entry.id.values[min_idx]

    good_vtx_sb_v  = pd.Series(good_vtx_sb_v)
    good_vtx_id_v  = pd.Series(good_vtx_id_v) 
    sb_vtx_df=pd.DataFrame([good_vtx_sb_v,good_vtx_id_v]).T
    sb_vtx_df.columns=['good','idx']
    return sb_vtx_df

def get_best_df(s_vtx_df,sig_vtx3d,base_index):
    good_vtx_tmp=s_vtx_df.copy()
    good_vtx_tmp=good_vtx_tmp.reset_index()
    good_vtx_tmp.columns=base_index+['good','id']
    good_vtx_tmp=good_vtx_tmp.set_index(base_index + ['id'])
    good_vtx_tmp=good_vtx_tmp.query("good==1.0")
    sig_good_vtx_df=sig_vtx3d.ix[good_vtx_tmp.index]
    return sig_good_vtx_df


def energy_plotter(sig_mctree_s,sig_mctree_t,Emin,Emax,deltaE,key,Name):
    fig,ax=plt.subplots(figsize=(10,6))
    bins_=np.arange(0,Emax+deltaE,deltaE)
    a1=plt.hist(sig_mctree_s.query("energyInit >=@Emin & energyInit <= @Emax")[key].values,
                bins=bins_,color='blue',label='Signal')
    a3=plt.hist(sig_mctree_t.query("energyInit >=@Emin & energyInit <= @Emax")[key].values,
                bins=bins_,color='red',label='Reconstruction')
    ax.set_xlabel("%s Energy [MeV]"%Name,fontweight='bold')
    ax.set_ylabel("Count / %d"%deltaE,fontweight='bold')
    ax.set_title("Good Reconstructed Vertex",fontweight='bold')
    ax.legend(fontsize=20)
    plt.tight_layout()
    plt.grid()
    plt.show()

    fig,ax=plt.subplots(figsize=(10,6))
    b=np.nan_to_num(a3[0] / a1[0])
    bidx=np.nonzero(b)
    signal_v=a1[0]
    energy_v=a1[1][:-1]+float(deltaE)/2.0
    eff_v   =b

    cpy=[]
    res_v=np.where(eff_v==0)[0]
    eff_vv   =[]
    signal_vv=[]
    energy_vv=[]
    eff_v_   =[]
    signal_v_=[]
    energy_v_=[]
    for ix in xrange(eff_v.size):
        if ix in res_v:
            eff_vv.append(np.array(eff_v_))
            signal_vv.append(np.array(signal_v_))
            energy_vv.append(np.array(energy_v_))
            eff_v_   =[]
            signal_v_=[]
            energy_v_=[]
            continue

        eff_v_.append(eff_v[ix])
        signal_v_.append(signal_v[ix])
        energy_v_.append(energy_v[ix])

    eff_vv.append(np.array(eff_v_))
    signal_vv.append(np.array(signal_v_))
    energy_vv.append(np.array(energy_v_))
    
    for energy_v_,eff_v_,signal_v_ in zip(energy_vv,eff_vv,signal_vv):
        ax.plot(energy_v_,eff_v_,'o',color='blue',markersize=8)
        ax.errorbar(energy_v_,eff_v_,yerr=np.sqrt(eff_v_*(1-eff_v_)/signal_v_),lw=2,color='blue')
        
    ax.set_ylim(0,1.0)
    ax.set_xlabel("%s Energy [MeV]"%Name,fontweight='bold')
    ax.set_ylabel("Reconstruction Efficiency",fontweight='bold')
    plt.grid()
    plt.tight_layout()
    plt.show()


def angle_plotter(data1,data2,Tmin,Tmax,deltaT,Name):
    fig,ax=plt.subplots(figsize=(10,6))
    bins_=np.arange(Tmin,Tmax+deltaT,deltaT)

    a1=plt.hist(data1,
                bins=bins_,color='blue',label='Signal')
    a3=plt.hist(data2,
                bins=bins_,color='red',label='Reconstruction')
    
    ax.set_xlabel("%s Cos$\Theta$"%Name,fontweight='bold')
    ax.set_ylabel("Count / %d"%deltaT,fontweight='bold')
    ax.set_title("Good Reconstructed Vertex",fontweight='bold')
    ax.set_xlim(-1.0,1.0)
    ax.legend(loc='best',fontsize=20)
    plt.tight_layout()
    plt.grid()
    plt.show()

    fig,ax=plt.subplots(figsize=(10,6))
    b=np.nan_to_num(a3[0] / a1[0])
    bidx=np.nonzero(b)
    signal_v = a1[0]
    angle_v  = a1[1][:-1]+float(deltaT)/2.0
    eff_v    = b

    cpy=[]
    res_v=np.where(eff_v==0)[0]
    eff_vv   =[]
    signal_vv=[]
    angle_vv =[]
    eff_v_   =[]
    signal_v_=[]
    angle_v_ =[]

    for ix in xrange(eff_v.size):
        if ix in res_v:
            eff_vv.append(np.array(eff_v_))
            signal_vv.append(np.array(signal_v_))
            angle_vv.append(np.array(angle_v_))
            eff_v_=[]
            signal_v_=[]
            angle_v_=[]
            continue

        eff_v_.append(eff_v[ix])
        signal_v_.append(signal_v[ix])
        angle_v_.append(angle_v[ix])

    eff_vv.append(np.array(eff_v_))
    signal_vv.append(np.array(signal_v_))
    angle_vv.append(np.array(angle_v_))    

    for angle_v_,eff_v_,signal_v_ in zip(angle_vv,eff_vv,signal_vv):
        ax.plot(angle_v_,eff_v_,'o',color='blue',markersize=8)
        ax.errorbar(angle_v_,eff_v_,yerr=np.sqrt(eff_v_*(1-eff_v_)/signal_v_),lw=2,color='blue')

    ax.set_xlim(-1.0,1.0)
    ax.set_ylim(0,1.0)
    ax.set_xlabel("%s Cos$\Theta$"%Name,fontweight='bold')
    ax.set_ylabel("Reconstruction Efficiency",fontweight='bold')
    plt.grid()
    plt.tight_layout()
    plt.show()
