from whiteplotimage import WhitePlotImage
from .. import np
from .. import QtGui
import pandas as pd

class WhiteImage(WhitePlotImage):

    def __init__(self, img_v, roi_v, planes):
        super(WhiteImage, self).__init__(img_v, roi_v, planes)

        self.name = "WhiteImage"
        
        # # Custom presets for convenience
        # self.raw_adc = QtGui.QPushButton("Raw ADC")
        # self.pmt_weighted = QtGui.QPushButton("PMT Weighted")
        # self.mip = QtGui.QPushButton("MIP")
        # self.hip = QtGui.QPushButton("HIP")

        # self.presets = {"0": self.raw_adc,
        #                 "1": self.pmt_weighted,
        #                 "2": self.mip,
        #                 "3": self.hip}

        # self.raw_adc.clicked.connect( self.setRawADC )
        # self.pmt_weighted.clicked.connect( self.setPMTWeighted )
        # self.mip.clicked.connect( self.setMIP )
        # self.hip.clicked.connect( self.setHIP )

        # self.preset_layout = QtGui.QGridLayout()
        # self.preset_layout.addWidget(self.raw_adc,0,0)
        # self.preset_layout.addWidget(self.pmt_weighted,0,1)
        # self.preset_layout.addWidget(self.mip,0,2)
        # self.preset_layout.addWidget(self.hip,0,3)

    # revert back to how image was in ROOTFILE for caffe
    # assuming the image2d.as_ndarray is how you want to send it to caffe
    # if image2d is different than training, this is your chance the fix
    # it before it goes to caffe
    
    #in this instance transposes the axis, looked at during the dump
    def __caffe_copy_image__(self):
        work_copy = self.work_mat.copy()
        return np.transpose(work_copy, (1, 0, 2))

    # # connectors for the presets
    # def setRawADC(self):
    #     self.planes[0].setCurrentIndex(1)
    #     self.planes[1].setCurrentIndex(5)
    #     self.planes[2].setCurrentIndex(9)

    # def setPMTWeighted(self):
    #     self.planes[0].setCurrentIndex(2)
    #     self.planes[1].setCurrentIndex(6)
    #     self.planes[2].setCurrentIndex(10)

    # def setMIP(self):
    #     self.planes[0].setCurrentIndex(3)
    #     self.planes[1].setCurrentIndex(7)
    #     self.planes[2].setCurrentIndex(11)

    # def setHIP(self):
    #     self.planes[0].setCurrentIndex(4)
    #     self.planes[1].setCurrentIndex(8)
    #     self.planes[2].setCurrentIndex(12)
    

    
    
