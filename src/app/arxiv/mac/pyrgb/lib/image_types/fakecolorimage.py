from plotimage import PlotImage
from .. import np

class FakeColorImage(PlotImage):

    def __init__(self,img_v,roi_v,planes) :
        super(FakeColorImage,self).__init__(img_v,roi_v,planes)
        self.name = "FakeColorImage"

    def __caffe_copy_image__(self):
        return self.work_mat.copy()