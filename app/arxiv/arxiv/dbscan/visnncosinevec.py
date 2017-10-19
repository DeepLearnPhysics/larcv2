import os,sys
import pyqtgraph as pg
from pyqtgraph import PlotDataItem
import numpy as np
from larcv import larcv

class VisNNCosineVec:
    
    def __init__(self):
        pass

    def configure(self,pset):
        self.producer = pset.get("producer")
        self.hit_producer = pset.get("hit_producer")
        self.thinningfactor = pset.get("thinning_factor", 0.0 )

    def visualize( self, larlite_io, larcv_io, rawdigit_io ):
        event_imgs = larcv_io.get_data( larcv.kProductImage2D, self.producer )
        event_hits = larcv_io.get_data( larcv.kProductImage2D, self.hit_producer )

        lcv_imgs = event_imgs.Image2DArray()
        lcv_hits = event_hits.Image2DArray()

        pixel_vecs = [] # output container

        for iimg in xrange(0,lcv_imgs.size()):
            lcv_img = lcv_imgs.at(iimg)
            lcv_hit = lcv_hits.at(iimg)

            meta = lcv_img.meta()
        
            angimg = larcv.as_ndarray(lcv_img)
            hitpx  = larcv.as_ndarray(lcv_hit)
            
            hits = np.argwhere( hitpx>0.1 )
        
            print "number of hits: ",len(hits)
            vec_data_x = np.zeros( 2*len(hits) ) # we make pair of points for vec
            vec_data_y = np.zeros( 2*len(hits) ) # we make pair of points for vec
            for i,hit in enumerate(hits):
                # set origin
                x = meta.pos_x( hit[0] )
                y = meta.pos_y( hit[1] )

                vec_data_x[2*i]   = x
                vec_data_y[2*i]   = y

                ang = angimg[hit[0],hit[1]]
                dx = np.cos(ang)*meta.pixel_width()
                dy = np.sin(ang)*meta.pixel_height()

                vec_data_x[2*i+1] = x + dx
                vec_data_y[2*i+1] = y - dy # (-) because origin of y is top corner
            
            plot = PlotDataItem( x=vec_data_x, y=vec_data_y, pen=(255,255,255,100),connect='pairs' )
            pixel_vecs.append(plot)

        return pixel_vecs
            
