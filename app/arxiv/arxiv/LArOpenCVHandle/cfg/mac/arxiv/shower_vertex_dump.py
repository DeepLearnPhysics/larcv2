from larcv import larcv
larcv.load_pyutil
larcv.load_cvutil
import sys,os
import cv2
import ROOT
from ROOT import fcllite
from ROOT import geo2d
from ROOT import larocv

import matplotlib
import matplotlib.pyplot as plt
plt.rc('grid', linestyle="-", color='black')

import numpy as np

proc = larcv.ProcessDriver('ProcessDriver')

CFG="../reco_shower_true.cfg"
print "Loading config... ",CFG
proc.configure(CFG)
flist=ROOT.std.vector('std::string')()
flist.push_back("/Users/vgenty/Desktop/nue_8000.root")
proc.override_input_file(flist)
filter_id = proc.process_id("NuFilter")
mcinfo_id = proc.process_id("LArbysImageMC")
reco_id   = proc.process_id("LArbysImage")
ana_id    = proc.process_id("LArbysImageAna")
filter_proc   = proc.process_ptr(filter_id)
mcinfo_proc   = proc.process_ptr(mcinfo_id)
mcinfo_proc.SetFilter(filter_proc)
larbysimg     = proc.process_ptr(reco_id)
larbysimg_ana = proc.process_ptr(ana_id)
larbysimg_ana.SetManager(larbysimg.Manager())
proc.override_ana_file("/tmp/test.root")
proc.initialize()

for event in xrange(0,1000):

    proc.batch_process(event,1)

    if (filter_proc.selected()==False): continue

    print event

    mgr=larbysimg.Manager()
    pygeo = geo2d.PyDraw()
    img_v = []
    oimg_v = []
    track_img_v=[]
    otrack_img_v=[]
    shower_img_v=[]
    oshower_img_v=[]

    for mat in mgr.OriginalInputImages(0):
        oimg_v.append(pygeo.image(mat))
    for mat in mgr.OriginalInputImages(1):
        otrack_img_v.append(pygeo.image(mat))
    for mat in mgr.OriginalInputImages(2):
        oshower_img_v.append(pygeo.image(mat))
    
    for mat in mgr.InputImages(0):
        img_v.append(pygeo.image(mat))
    for mat in mgr.InputImages(1):
        track_img_v.append(pygeo.image(mat))
    for mat in mgr.InputImages(2):
        shower_img_v.append(pygeo.image(mat))


    for plane in xrange(len(track_img_v)):
        oshower_img = np.where(oshower_img_v[plane]>10.0,85.0,0.0).astype(np.uint8)
        otrack_img = np.where(otrack_img_v[plane]>10.0,160.0,0.0).astype(np.uint8)
        shower_img = np.where(shower_img_v[plane]>10.0,85.0,0.0).astype(np.uint8)
        track_img = np.where(track_img_v[plane]>10.0,160.0,0.0).astype(np.uint8)
        f, (ax1, ax2) = plt.subplots(1, 2, sharey=True,figsize=(15,10))
        oimg=oshower_img+otrack_img
        img=shower_img+track_img
        ax1.imshow(oimg,cmap='jet',interpolation='none',vmin=0.,vmax=255.)
        ax2.imshow(img,cmap='jet',interpolation='none',vmin=0.,vmax=255.)
        ax1.set_xlabel('Time [6 ticks]',fontsize=20)
        ax2.set_xlabel('Time [6 ticks]',fontsize=20)
        ax1.set_xlim(0,512)
        ax2.set_xlim(0,512)
        ax1.set_ylim(0,512)
        ax2.set_ylim(0,512)
        ax1.set_ylabel('Wire',fontsize=20)
        ax1.tick_params(labelsize=20)
        plt.tight_layout()        
        SS="out/%04d_00_track_shower_%d.png"%(event,plane)
        print "Saving ",SS
        plt.savefig(SS)
        plt.cla()
        plt.clf()
        plt.close()

    #["LinearTrackFinder","SuperClusterMaker","ShowerVertexSeeds","ShowerVertexEstimate"]
    dm=mgr.DataManager()
    colors=['red','green','blue','orange','magenta','cyan','pink']
    colors*=10

    dm=mgr.DataManager()
    data=dm.Data(0,0)
    lintrk_v=data.as_vector()
    print "Found ",lintrk_v.size()," linear track clusters"
    strack_n=-1
    for strack in lintrk_v:
        strack_n+=1
        # the only good one...
        e13d=strack.edge1
        e23d=strack.edge2
        print e13d,e23d
        for plane in xrange(3):
            strack2d = strack.get_cluster(plane)
            fig,ax=plt.subplots(figsize=(12,12),facecolor='w')
            shape_img = img_v[plane]
            shape_img=np.where(img_v[plane]>0.0,1.0,0.0).astype(np.uint8)
            plt.imshow(shape_img,cmap='Greys',interpolation='none')
            nz_pixels=np.where(shape_img>0.0)
            if strack2d.ctor.size()>0:
                ctor = [[pt.x,pt.y] for pt in strack2d.ctor]
                ctor.append(ctor[0])
                ctor=np.array(ctor)
    
                plt.plot(ctor[:,0],ctor[:,1],'-o',lw=3)
    
                e1=strack2d.edge1
                e2=strack2d.edge2
                
                plt.plot(e1.x,e1.y,'*',color='orange',markersize=20)
                plt.plot(e2.x,e2.y,'*',color='yellow',markersize=20)

                try:
                    vtx2d=e13d.vtx2d_v[plane]
                    pt=vtx2d.pt
                    plt.plot(pt.x,pt.y,'o',color='green',markersize=40,alpha=0.7)
                except:
                    pass

                try:
                    vtx2d=e23d.vtx2d_v[plane]
                    pt=vtx2d.pt
                    plt.plot(pt.x,pt.y,'o',color='green',markersize=40,alpha=0.7)
                except:
                    pass
            
            ax.set_aspect(1.0)
            ax.set_ylim(np.min(nz_pixels[0])-10,np.max(nz_pixels[0])+10)
            ax.set_xlim(np.min(nz_pixels[1])-10,np.max(nz_pixels[1])+10)
            plt.xlabel('Time [6 ticks]',fontsize=20)
            plt.ylabel('Wire [2 wires]',fontsize=20)
            plt.tick_params(labelsize=20)
            ax.set_aspect(0.8)

            SS="out/%04d_01_%04d_plane_%02d.png"%(event,strack_n,plane)
            ax.set_title(SS,fontsize=30)
            plt.savefig(SS)
            plt.cla()
            plt.clf()
            plt.close()


    dm=mgr.DataManager()
    data=dm.Data(3,0)
    print data
    for vtx3d in data.as_vector():
        for plane in xrange(3):
            fig,ax=plt.subplots(figsize=(12,12),facecolor='w')
            shape_img = img_v[plane]
            shape_img=np.where(shape_img>0.0,1.0,0.0).astype(np.uint8)
            plt.imshow(shape_img,cmap='Greys',interpolation='none')
            nz_pixels=np.where(shape_img>0.0)
    
            cvtx=vtx3d.cvtx2d_v[plane]
            plt.plot(cvtx.center.x,cvtx.center.y,'o',color='red',markersize=10)
            circl=matplotlib.patches.Circle((cvtx.center.x,cvtx.center.y),
                                      cvtx.radius,fc='none',ec='cyan',lw=5)
            for xs in cvtx.xs_v:
                plt.plot(xs.pt.x,xs.pt.y,'o',color='orange',markersize=10)
                
            ax.add_patch(circl)
            ax.set_aspect(1.0)
            ax.set_ylim(np.min(nz_pixels[0])-10,np.max(nz_pixels[0])+10)
            ax.set_xlim(np.min(nz_pixels[1])-10,np.max(nz_pixels[1])+10)
            plt.xlabel('Time [6 ticks]',fontsize=20)
            plt.ylabel('Wire [2 wires]',fontsize=20)
            plt.tick_params(labelsize=20)
            ax.set_aspect(0.8)

        SS="out/%04d_02_plane_%02d.png"%(event,plane)
        ax.set_title(SS,fontsize=30)
        plt.savefig(SS)
        plt.cla()
        plt.clf()
        plt.close()
        
    assman=dm.AssManager()
    #New VertexCluster
    vtx_data=dm.Data(3,0).as_vector()
    
    for vtx in vtx_data:
        for plane in xrange(3):
            fig,ax = plt.subplots(figsize=(12,12),facecolor='w')
            shape_img = img_v[plane]
            shape_img=np.where(shape_img>0.0,1.0,0.0).astype(np.uint8)
            plt.imshow(shape_img,cmap='Greys',interpolation='none')
            nz_pixels=np.where(shape_img>0.0)
            
            par_data=dm.Data(4,plane)
            
            ass_t = np.array(assman.GetManyAss(vtx,par_data.ID()))
            if ass_t.size==0:continue
                
            par_data_v=par_data.as_vector()
            for id_ in ass_t:
                ctor=np.array([[pt.x,pt.y] for pt in par_data_v[id_]._ctor])
                plt.plot(ctor[:,0],ctor[:,1],'-o',lw=2)
            
            ax.set_ylim(np.min(nz_pixels[0])-10,np.max(nz_pixels[0])+10)
            ax.set_xlim(np.min(nz_pixels[1])-10,np.max(nz_pixels[1])+10)
            SS="out/%04d_03_plane_%02d.png"%(event,plane)
            ax.set_title(SS,fontsize=30)
            plt.savefig(SS)
            plt.cla()
            plt.clf()
            plt.close()

