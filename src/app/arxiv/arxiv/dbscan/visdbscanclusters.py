import os,sys
import pyqtgraph as pg
from pyqtgraph import PlotDataItem
import numpy as np
from larcv import larcv

class VisDBScanClusters:

    COLORS = [ (76,0,153,125), # purple
               (102,255,102),  # green
               (51,255,255),   # cyan
               (255,128,0),    # orange
               (0,76,153),     # blue
               (204,0,0),      # red
               (0,102,0),      # dark-green
               (102,0,51),     # dark-magenta
               (255,255,0),    # yellow
               ]
    NCOLORS = len(COLORS)
               
    
    def __init__(self):
        pass

    def configure(self,pset):
        self.producer = pset.get("dbscan_producer")

    def visualize( self, larlite_io, larcv_io, rawdigit_io ):
        event_imgs = larcv_io.get_data( larcv.kProductImage2D, self.producer )

        lcv_imgs = event_imgs.Image2DArray()

        cluster_vecs = [] # output container

        for iimg in xrange(0,lcv_imgs.size()):
            lcv_img = lcv_imgs.at(iimg)

            meta = lcv_img.meta()
            plane = meta.plane()
        
            img = larcv.as_ndarray(lcv_img)

            img += 0.1 # to help with float to int rounding
            maxid = int(np.amax( img ))
            print "number of clusters: ",type(maxid)

            for ic in xrange(1,maxid): # skip nothing label
                hits = np.argwhere( np.logical_and( img>(ic), img<ic+1) )
                nhits = len(hits)
                if nhits==0:
                    continue
                print "clusterid=",ic," number of hits=",nhits
                x = np.zeros( len(hits) )
                y = np.zeros( len(hits) )
                for ihit in xrange(0,len(hits)):

                    x[ihit] = meta.pos_x( hits[ihit][0] )
                    y[ihit] = meta.pos_y( hits[ihit][1] )
            
                if ic==1:
                    # background
                    color = (100,100,100,75)
                    plot = PlotDataItem( x=x, y=y, pen=None,symbolBrush=pg.mkBrush(color=color),symbol='o',symbolPen=pg.mkPen(color=color,width=0.0),width=1.0 )
                else:
                    color = VisDBScanClusters.COLORS[ ic%VisDBScanClusters.NCOLORS ]
                    plot = PlotDataItem( x=x, y=y, 
                                         pen=pg.mkPen(color=color,width=2),
                                         symbolBrush=pg.mkBrush(color=color),
                                         symbol='o',symbolPen=pg.mkPen(color=color,width=0.0) )

                cluster_vecs.append(plot)
        print "number of cluster plots: ",len(cluster_vecs)
        return cluster_vecs
            
