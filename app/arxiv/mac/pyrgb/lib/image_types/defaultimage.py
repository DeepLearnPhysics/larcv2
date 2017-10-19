from plotimage import PlotImage
from .. import np


class DefaultImage(PlotImage):

    def __init__(self, img_v, roi_v, planes):
        super(DefaultImage, self).__init__(img_v, roi_v, planes)
        self.name = "DefaultImage"

    def __caffe_copy_image__(self):
        return self.work_mat.copy()