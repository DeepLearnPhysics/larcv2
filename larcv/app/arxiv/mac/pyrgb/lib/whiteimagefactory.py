from image_types.whiteimage   import WhiteImage

class WhiteImageFactory(object):
    def __init__(self):
        self.name = "WhiteImageFactory"

    def get(self,imdata,roidata,planes,improd,**kwargs):

        # 3 channel data
        return WhiteImage(imdata,roidata,planes)
