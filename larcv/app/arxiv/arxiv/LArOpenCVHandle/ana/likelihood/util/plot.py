import pandas as pd
import numpy as np

import matplotlib
import matplotlib.pyplot as plt
matplotlib.rcParams['font.size']=20
matplotlib.rcParams['font.family']='serif'


def eff_plot(df1,df2,
             Xmin,Xmax,dX,
             query,key,
             Xlabel,Ylabel,
             name):
    
    fig,ax = plt.subplots(figsize=(10,6))
    
    bins = np.arange(Xmin,Xmax+dX,dX)

    data = df1.query(query)[key].values
    sig  = ax.hist(data,bins=bins,color='blue',label='Signal')

    data = df2.query(query)[key].values
    reco = ax.hist(data,bins=bins,color='red' ,label='Reconstructed')
    
    ax.set_xlabel(Xlabel,fontweight='bold')
    ax.set_ylabel(Ylabel,fontweight='bold')
    ax.legend(fontsize=20)
    plt.tight_layout()
    plt.grid()
    SS="{}_hist.pdf".format(name)
    print "Write {}".format(SS)
    plt.savefig("ll_dump/%s" % SS,format='pdf')
    plt.cla() 
    plt.clf()
    plt.close()

    fig,ax = plt.subplots(figsize=(10,6))
    
    reco_sig = reco[0] / sig[0]
    reco_sig = np.nan_to_num(reco_sig)
    bidx     = np.nonzero(reco_sig)
    signal_v = sig[0]
    param_v  = sig[1][:-1]+float(dX)/2.0
    eff_v    = reco_sig

    res_v    = np.where(eff_v==0)[0]
    eff_vv   = []
    signal_vv= []
    param_vv = []
    eff_v_   = []
    signal_v_= []
    param_v_ = []

    
    for ix in xrange(eff_v.size):
        if ix in res_v:
            eff_vv.append(np.array(eff_v_))
            signal_vv.append(np.array(signal_v_))
            param_vv.append(np.array(param_v_))
            eff_v_    = []
            signal_v_ = []
            param_v_  = []
            continue

        eff_v_.append(eff_v[ix])
        signal_v_.append(signal_v[ix])
        param_v_.append(param_v[ix])

    eff_vv.append(np.array(eff_v_))
    signal_vv.append(np.array(signal_v_))
    param_vv.append(np.array(param_v_))
    
    for param_v_,eff_v_,signal_v_ in zip(param_vv,eff_vv,signal_vv):
        ax.plot(param_v_,eff_v_,'o',color='blue',markersize=8)
        ax.errorbar(param_v_,eff_v_,yerr= np.sqrt( eff_v_ * ( 1 - eff_v_ ) / signal_v_ ),lw=2,color='blue')
        
    ax.set_ylim(0,1.0)
    ax.set_xlabel(Xlabel,fontweight='bold')
    ax.set_ylabel("Reconstruction Efficiency",fontweight='bold')
    plt.grid()
    plt.tight_layout()
    SS="{}_eff.pdf".format(name)
    print "Write {}".format(SS)
    plt.savefig("ll_dump/%s" % SS,format='pdf')
    plt.cla() 
    plt.clf()
    plt.close()

def histo1(df1,
           Xmin,Xmax,dX,
           query,key,
           Xlabel,Ylabel,
           name):

    fig,ax = plt.subplots(figsize=(10,6))

    data = df1[key].values
    if query == None or query == "": pass
    else: data = df1.query(query)[key].values
    
    bins = np.arange(Xmin,Xmax+dX,dX)
    weights = [1/float(data.size)] * data.size

    ax.hist(data,bins=bins,weights=weights,color='blue')
    ax.set_xlabel(Xlabel,fontweight='bold')
    ax.set_ylabel(Ylabel,fontweight='bold')
    plt.grid()
    plt.tight_layout()    
    SS="{}_histo.pdf".format(name)
    print "Write {}".format(SS)
    plt.savefig("ll_dump/%s" % SS,format='pdf')
    plt.cla() 
    plt.clf()
    plt.close()
    
def histo2(df1,
           Xmin,Xmax,dX,
           query1,query2,key,
           Xlabel,Ylabel,
           name):

    fig,ax = plt.subplots(figsize=(10,6))

    data1 = df1.query(query1)[key].values
    bins1 = np.arange(Xmin,Xmax+dX,dX)
    weights1 = [1/float(df1.index.size)] * data1.size
    data2 = df1.query(query2)[key].values
    bins2 = np.arange(Xmin,Xmax+dX,dX)
    weights2 = [1/float(df1.index.size)] * data2.size
    
    ax.hist([data1,data2],
            bins=bins1,
            weights=[weights1,weights2],
            color=['blue','red'],
            histtype='stepfilled',
            alpha=1.0,
            lw=2,
            stacked=True)
    
    ax.set_xlabel(Xlabel,fontweight='bold')
    ax.set_ylabel(Ylabel,fontweight='bold')
    plt.grid()
    plt.tight_layout()    
    SS="{}_histo.pdf".format(name)
    print "Write {}".format(SS)
    plt.savefig("ll_dump/%s" % SS,format='pdf')
    plt.cla() 
    plt.clf()
    plt.close()


def eff_scan(df1,df2,
             Xmin,Xmax,dX,
             key,
             Xlabel,Ylabel,
             name):

    fig,ax = plt.subplots(figsize=(10,6))
    

    data1    = df1[key].values
    bins1    = np.arange(Xmin,Xmax+dX,dX)
    weights1 = [1/float(data1.size)]*data1.size
    kres1    = np.histogram(data1,bins=bins1,weights=weights1)
    
    data2    = df2[key].values
    bins2    = np.arange(Xmin,Xmax+dX,dX)
    weights2 = [1/float(data2.size)]*data2.size
    kres2    = np.histogram(data2,bins=bins2,weights=weights2)
    
    ktotal1  = float(kres1[0].sum())
    centers1 = bins1 + (bins1[1]-bins1[0])/2.0
    centers1 = centers1[:-1]

    ktotal2  = float(kres2[0].sum())
    centers2 = bins2 + (bins2[1]-bins2[0])/2.0
    centers2 = centers2[:-1]

    ksum1  = [float(kres1[0][ix:].sum()) for ix in xrange(centers1.size)]
    keff1  = np.array([ksum1[ix] / ktotal1 for ix in xrange(centers1.size)])
    
    ksum2  = [float(kres2[0][ix:].sum()) for ix in xrange(centers2.size)]
    keff2  = np.array([ksum2[ix] / ktotal2 for ix in xrange(centers2.size)])
    
    ax.plot(centers1,keff1    ,'-',color='blue',lw=3,label='Signal')
    ax.plot(centers2,1.0-keff2,'-',color='red',lw=3,label='Background')

    ax.set_xlabel(Xlabel,fontweight='bold')
    ax.set_ylabel(Ylabel,fontweight='bold')
    plt.grid()
    plt.tight_layout()    
    SS="{}_eff_scan.pdf".format(name)
    print "Write {}".format(SS)
    plt.savefig("ll_dump/%s" % SS,format='pdf')
    plt.cla() 
    plt.clf()
    plt.close()
