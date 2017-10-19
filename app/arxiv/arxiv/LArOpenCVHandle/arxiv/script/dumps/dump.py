#!/usr/bin/env python
import ROOT, sys, os
from ROOT import std

from larlite import larlite
from larcv import larcv
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.path as path
colormap=['blue','red','magenta','green','orange','yellow','pink']
nc=len(colormap)
from ROOT import geo2d
from ROOT import cv
from ROOT.cv import Point_ as Vector
DTYPE='float'
import matplotlib.patches as patches


# In[6]:
iom=larcv.IOManager(larcv.IOManager.kBOTH)
iom.reset()
iom.set_verbosity(2)
iom.add_in_file("/Users/vgenty/Downloads/ccqe_0000_0099.root")
iom.add_in_file("/Users/vgenty/Downloads/ccqe_0100_0199.root")
iom.add_in_file("/Users/vgenty/Downloads/ccqe_0200_0299.root")
iom.set_out_file("/tmp/trash.root")
iom.initialize()

larbysimg=larcv.LArbysImage()
cfg=larcv.CreatePSetFromFile("unit.fcl","LArbysImage")
larbysimg.configure(cfg)
larbysimg.initialize()

pygeo   = geo2d.PyDraw()

for event in [23]:
    print "EVENT IS ",event
    fig,ax=plt.subplots(figsize=(10,10))
    ax.text(-30,75,"Event %d"%event,fontsize=35,color='blue',fontweight='bold')
    ax.plot([-100,100],[-100,100],'-',lw=10,color='black')
    ax.plot([-100,100],[100,-100],'-',lw=10,color='black')
    ax.set_xticks([])
    ax.set_yticks([])
    ax.set_xticklabels([])
    ax.set_yticklabels([])
    plt.savefig("%04d_0_0.png"%event)
    plt.cla()
    plt.clf()
    plt.close()
    
    iom.read_entry(event)#8
    larbysimg.process(iom)

    mgr=larbysimg.Manager()
    dm=mgr.DataManager()

    img_v = []

    for mat in mgr.InputImages():
        img_v.append(pygeo.image(mat))
    for plane in xrange(3):
        print "plane : ",plane
        fig,ax=plt.subplots(figsize=(20,20))

        shape_img=np.where(img_v[plane]>10.0,1.0,0.0).astype(np.uint8)
        
        plt.imshow(shape_img,cmap='Greys',interpolation='none')
        nz_pixels=np.where(shape_img>0.0)

        ax.set_ylim(np.min(nz_pixels[0])-10,np.max(nz_pixels[0])+10)
        ax.set_xlim(np.min(nz_pixels[1])-10,np.max(nz_pixels[1])+10)

        mgr=larbysimg.Manager()
        dm=mgr.DataManager()
        data=dm.Data(1)

        compound_v = data._raw_cluster_vv[plane]

        print "clusters size:",compound_v.get_cluster().size()
        for compound in compound_v.get_cluster():
            print " - atoms size: ",compound.get_atoms().size()
            for atom in compound.get_atoms():
                print "-- ctor size: ", atom._ctor.size()
                pts = np.array([[pt.x,pt.y] for pt in atom._ctor])
                ax.plot(pts[:,0],pts[:,1],'-o')
            for defect in compound.get_defects():
                ax.plot([0,900],[defect._split_line.y(0),defect._split_line.y(900)])

        ax.set_aspect(2.0)
        plt.savefig("%04d_1_%1d.png"%(event,plane))
        plt.cla()
        plt.clf()
        plt.close()


    alg=mgr.GetClusterAlg(3)

    dm=mgr.DataManager()
    ref_data = dm.Data(3)
    tickscore0_y=[]
    tickscore0_x=[]

    score0_v = alg.TimeBinnedScore0Mean()
    for idx in xrange(score0_v.size()):
        v = score0_v[idx]
        tickscore0_y.append(v)
        tickscore0_x.append(idx*1 + alg.TimeBinMin())

    tickscore1_y=[]
    tickscore1_x=[]
    score1_v = alg.TimeBinnedScore1Mean()
    for idx in xrange(score1_v.size()):
        v = score1_v[idx]
        tickscore1_y.append(v)
        tickscore1_x.append(idx*1 + alg.TimeBinMin())

    tickscore0_x = np.array(tickscore0_x)
    tickscore0_y = np.array(tickscore0_y)
    tickscore1_x = np.array(tickscore1_x)
    tickscore1_y = np.array(tickscore1_y)

    #print "[",tickscore0_x,tickscore0_y,tickscore1_y,tickscore1_y,"]"
    
    if tickscore0_x.size==0:continue
    if tickscore0_y.size==0:continue
    if tickscore1_x.size==0:continue
    if tickscore1_y.size==0:continue
    
    ymin = tickscore0_y.min()
    ymax = tickscore0_y.max()
    if ymin > tickscore1_y.min(): ymin = tickscore1_y.min()
    if ymax < tickscore1_y.max(): ymax = tickscore1_y.max()

    fig,ax = plt.subplots(figsize=(24,8),facecolor='w')
    ax.plot(tickscore0_x,tickscore0_y,marker='o',linestyle='-',color='red')
    ax.plot(tickscore1_x,tickscore1_y,marker='o',linestyle='-',color='blue')

    minimum_v = alg.TimeBinnedScoreMinIndex()
    for idx in xrange(minimum_v.size()):
        xval = tickscore0_x[minimum_v[idx]]
        ax.plot([xval,xval],[ymin,ymax],marker='',linestyle='--',color='black',linewidth=2)

    ax.set_ylim(ymin,ymax)
    ax.set_xlim(tickscore0_x.min(),tickscore0_x.max())
    plt.savefig("%04d_2_0.png"%event)
    plt.cla()
    plt.clf()
    plt.close()

    for plane in xrange(3):
        fig,ax=plt.subplots(figsize=(20,20),facecolor='w')
        shape_img=np.where(img_v[plane]>10.0,1.0,0.0).astype(np.uint8)
        
        plt.imshow(shape_img,cmap='Greys',interpolation='none')
        nz_pixels=np.where(shape_img>0.0)

        ymin,ymax = (np.min(nz_pixels[0])-10,np.max(nz_pixels[0])+10)
        xmin,xmax = (np.min(nz_pixels[1])-10,np.max(nz_pixels[1])+10)    


        ax.set_ylim(ymin,ymax)
        ax.set_xlim(xmin,xmax)
        ix=-1

        alg = mgr.GetClusterAlg(3)

        cv=larbysimg.Manager().GetClusterAlg(3)
        mgr=larbysimg.Manager()
        dm=mgr.DataManager()

        #Getting Defect Points
        defect_data=dm.Data(1)
        compound_v = defect_data._raw_cluster_vv[plane]
        for compound in compound_v.get_cluster():
            for defect in compound.get_defects():
                pt = defect._pt_defect
                ax.plot([pt.x],[pt.y],marker='o',color='blue',markersize=10)

        pca_data = dm.Data(2)
        for pt in pca_data.points(plane):
            ax.plot([pt.x],[pt.y],marker='o',color='red',markersize=10)

        ref_data   = dm.Data(3)

        plane_scan = ref_data.get_plane_data(plane)

        for circle in plane_scan._circle_scan_v:
            c=patches.Circle((circle.center.x,circle.center.y),circle.radius,ec='cyan',alpha=0.05,fc='none',lw=10)
            ax.add_patch(c)

        minimum_v = alg.TimeBinnedScoreMinIndex()
        for idx in xrange(minimum_v.size()):
            xval = minimum_v[idx] + alg.TimeBinMin()
            ax.plot([xval,xval],[ymin,ymax],marker='',linestyle='--',color='magenta',linewidth=2)

        vtx_vv = ref_data.get_circle_vertex()

        vtx_idx=0
        for vtx_idx in xrange(len(vtx_vv)):
            vtx = vtx_vv[vtx_idx][plane]
            ax.plot([vtx.center.x],[vtx.center.y],marker='$\star$',color='yellow',markersize=24)

        ax=plt.gca()

        plt.savefig("%04d_3_%d.png"%(event,plane))
        plt.cla()
        plt.clf()
        plt.close()

    vtx_cluster_data = dm.Data(4)
    for vtx_data in vtx_cluster_data._vtx_cluster_v:
        print "vtx_data ptr: ",vtx_data
        for plane in xrange(3):
            print "plane: ",plane
            print "... num clusters: ",vtx_data.num_clusters(plane),"... num_pixels: ",vtx_data.num_pixels(plane)
            print "... num pixel frac: ",vtx_data.num_pixel_fraction(plane)
        print ""

    iv=-1
    for vtx_data in vtx_cluster_data._vtx_cluster_v:
        iv+=1
        for plane in xrange(3):
        
            fig,ax=plt.subplots(figsize=(20,20),facecolor='w')

            shape_img=np.where(img_v[plane]>1.0,1.0,0.0).astype(np.uint8)

            plt.imshow(shape_img,cmap='Greys',interpolation='none')
            
            nz_pixels=np.where(shape_img>0.0)
            
            ymin,ymax = (np.min(nz_pixels[0])-10,np.max(nz_pixels[0])+10)
            xmin,xmax = (np.min(nz_pixels[1])-10,np.max(nz_pixels[1])+10)    
            
            ax.set_ylim(ymin,ymax)
            ax.set_xlim(xmin,xmax)

            for cluster in vtx_data.get_clusters(plane):
                ctor = cluster._ctor;
                c_=[[pt.x,pt.y] for pt in ctor]
                c_.append(c_[0])
                c_=np.array(c_)
                print plane,c_.shape
                plt.plot(c_[:,0],c_[:,1],'-o')
            
            plt.savefig("%04d_4_%d_%d.png"%(event,iv,plane))
            
            
