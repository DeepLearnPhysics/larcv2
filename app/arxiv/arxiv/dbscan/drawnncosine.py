import os,sys
from larcv import larcv
from pylard.pylardata.tpcdataplottable import TPCdataPlottable
import numpy as np

class DrawNNCosine:

    def __init__(self):
        pass

    def configure(self,pset):
        self.image_producer = pset.get("nncosine_producer")
        self.hit_producer = pset.get("hit_producer")
        self.roi_producer = pset.get("roi_producer")

    def visualize( self, larlite_io, larcv_io, rawdigit_io ):
        
        event_images    = larcv_io.get_data( larcv.kProductImage2D, self.image_producer )
        event_hitmarker = larcv_io.get_data( larcv.kProductImage2D, self.hit_producer )
        event_rois   = larcv_io.get_data( larcv.kProductROI, self.roi_producer )
        lcv_imgs = event_images.Image2DArray()
        lcv_hits = event_hitmarker.Image2DArray()

        planes = []
        out_images = larcv.EventImage2D()

        for iimg,lcv_img in enumerate(event_images.Image2DArray()):
            planes.append(iimg)
            meta = lcv_img.meta()
            img_ang = larcv.as_ndarray(lcv_img)
            img_hit = larcv.as_ndarray(lcv_hits.at(iimg))
            
            # make two images, dx component, dy component
            img_dx = np.zeros( img_ang.shape, dtype=np.float32 )
            img_dy = np.zeros( img_ang.shape, dtype=np.float32 )
            
            hits = np.argwhere( img_hit>0.1 )
            for hit in hits:
                img_dx[hit[0],hit[1]] = np.cos( img_ang[hit[0],hit[1]] )*200.0
                img_dy[hit[0],hit[1]] = np.sin( img_ang[hit[0],hit[1]] )*100.0 + 100.0
            
            lcv_dx = larcv.as_image2d( img_dx, meta )
            lcv_dy = larcv.as_image2d( img_dy, meta )

            out_images.Append( lcv_dx )
            out_images.Append( lcv_dy )
            

        pytpcdata = TPCdataPlottable( self.image_producer, out_images.Image2DArray(), event_rois.ROIArray(), planes )
        return pytpcdata
