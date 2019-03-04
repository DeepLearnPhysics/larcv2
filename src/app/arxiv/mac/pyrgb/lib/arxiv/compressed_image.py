from plotimage import PlotImage

from .. import np

class CompressedImage(PlotImage):

    def __init__(self,img_v,roi_v,planes) :
        super(CompressedImage,self).__init__(img_v,roi_v,planes)
        self.name = "CompressedImage"
    
    def __create_mat__(self):
        #compressed images all have the same shape
        xmin,xmax,ymin,ymax = self._get_boundaries(self.imgs)
        
        self.plot_mat = np.zeros(list(self.img_v[0].shape) + [3])
        
        for ix,img in enumerate(self.img_v):

            if ix not in self.planes:
                continue
            
            img = img[:,::-1]
            
            self.plot_mat[:,:,ix] = img


        self.plot_mat[:,:,0][ self.plot_mat[:,:,1] > 0.0 ] = 0.0
        self.plot_mat[:,:,0][ self.plot_mat[:,:,2] > 0.0 ] = 0.0
        self.plot_mat[:,:,1][ self.plot_mat[:,:,2] > 0.0 ] = 0.0
            
    def __threshold_mat__(self,imin,imax):

        #Have to profile this copy operation, could be bad
        self.plot_mat_t = self.plot_mat.copy()

        #I don't know how to slice
        self.plot_mat_t[ self.plot_mat_t < imin ] = 0
        self.plot_mat_t[ self.plot_mat_t > imax ] = imax
        

    def __create_rois__(self):
        
        for ix,roi in enumerate(self.roi_v) :
            
            if roi.BB().size() == 0: #there was no ROI continue...
                continue

            r = {}

            r['type'] = roi.Type()
            r['bbox'] = []

            for iy in xrange(3):
                r['bbox'].append( roi.BB(iy) )
                
            self.rois.append(r)

