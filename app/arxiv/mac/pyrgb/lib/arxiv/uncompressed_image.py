from plotimage import PlotImage

from .. import np

class UnCompressedImage(PlotImage):

    def __init__(self,img_v,roi_v,planes) :
        super(UnCompressedImage,self).__init__(img_v,roi_v,planes)
        self.name = "UnCompressedImage"

    def _get_boundaries(self,imgs):
        
        xmin,xmax,ymin,ymax = 99999,0,99999,0
        
        for img in imgs:
            meta = img.meta()

            if meta.bl().x < xmin: xmin = meta.bl().x
            if meta.tr().x > xmax: xmax = meta.tr().x
            
            if meta.bl().y < ymin: ymin = meta.bl().y
            if meta.tr().y > ymax: ymax = meta.tr().y

            # print " i saw...bl().x {} bl().y {} tr().x {} tr().y {} ".format(meta.bl().x,
            #                                                               meta.bl().y,
            #                                                               meta.tr().x,
            #                                                               meta.tr().y)
            
        print "xmin {} xmax {} ymin {} ymax {}".format(xmin,xmax,ymin,ymax)
        return ( xmin, xmax, ymin, ymax )
        
    def __create_mat__(self):
        # un compressed images are of various sizes

        xmin,xmax,ymin,ymax = self._get_boundaries(self.imgs)
        dy = ymax - ymin

        self.plot_mat = np.zeros([ int(xmax), int(ymax), 3 ])

        print self.plot_mat.shape

        for ix,img in enumerate(self.img_v):

            if ix not in self.planes: continue
            
            meta = self.imgs[ix].meta()
            
            aa = [ int(meta.bl().x)       , int(xmax - meta.tr().x) ] # column padding before and after
            bb = [ int(ymax - meta.tl().y), int(meta.bl().y)        ] # row padding before and after

            xx,yy = img.shape
            
            # Need explicit checks to make sure no fuck up with rounding
            while xx + aa[0] + aa[1] > int(xmax): aa[0] -= 1
            while xx + aa[0] + aa[1] < int(xmax): aa[0] += 1
            while yy + bb[0] + bb[1] > int(ymax): bb[0] -= 1
            while yy + bb[0] + bb[1] < int(ymax): bb[0] += 1

            img = np.pad(img,(tuple(aa),tuple(bb)),
                         mode='constant',
                         constant_values=0)
            
            img = img[:,::-1]
            
            self.plot_mat[:,:,ix] = img


        
        # don't want shit to overlap on the viewer
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

