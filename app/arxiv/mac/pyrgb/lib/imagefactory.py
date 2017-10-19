from image_types.defaultimage   import DefaultImage
from image_types.fakecolorimage import FakeColorImage
from image_types.ch12image      import Ch12Image

class ImageFactory(object):
    def __init__(self):
        self.name = "ImageFactory"

    def get(self,imdata,roidata,planes,improd,**kwargs):

        # fake color producer for vic's testing
        if improd == "fake_color": 
        	return FakeColorImage(imdata,roidata,planes)

        # taritree's 12 channel data
        if improd in ["tpc_12ch","tpc_12ch_mean"]:   
        	return Ch12Image(imdata,roidata,planes)

        # 3 channel data
        return DefaultImage(imdata,roidata,planes)
