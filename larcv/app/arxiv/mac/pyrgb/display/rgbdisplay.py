#thanks taritree

import os
import sys
import copy
import re
from .. import QtGui, QtCore
from .. import pg
from .. import larcv

import numpy as np
import time

from ..lib.datamanager import DataManager
from ..lib import storage as store

from ..lib.hoverrect import HoverRect
from ..lib.roislider import ROISlider

try:
    from cv2layout import CV2Layout
except:
    pass

try:
    from caffelayout import CaffeLayout
    from ..rgb_caffe.testwrapper import TestWrapper
except:
    pass

from roilayout import ROIToolLayout

import pyqtgraph.exporters

class RGBDisplay(QtGui.QWidget):

    def __init__(self, argv):
        super(RGBDisplay, self).__init__()

        # DataManager for loading the plot image
        self.dm = DataManager(argv)

        # Size the canvas
        self.resize(1200, 700)

        # Graphics window which will hold the image
        self.win = pg.GraphicsWindow()
        self.plt = self.win.addPlot()
        self.imi = pg.ImageItem(np.zeros((100,100))) # dummy
        self.plt.addItem( self.imi )

        # Handles to the axis which we will update with wire/tick
        self.plt_x = self.plt.getAxis('bottom')
        self.plt_y = self.plt.getAxis('left')

        # Main Layout
        self.layout = QtGui.QGridLayout()

        # run information up top
        self.runinfo = QtGui.QLabel(
            "<b>Run:</b> -1 <b>Subrun:</b> -1 <b>Event:</b> -1")
        self.layout.addWidget(self.runinfo, 0, 0)
        self.layout.addWidget(self.win, 1, 0, 1, 10)
        self.setLayout(self.layout)

        # Input Widgets
        # Layouts
        self.lay_inputs = QtGui.QGridLayout()
        self.layout.addLayout(self.lay_inputs, 2, 0)
        
               
        
        # -------------------------------------------------------
        # Navigation box
        self._makeNavFrame()
        self.lay_inputs.addWidget( self._navframe, 0, 0, self.navheight-1, self.navwidth  )
        
        # -------------------------------------------------------
        # Thresholding
        threshstart = self.navwidth+1
        ### imin -- threshold
        self.imin = QtGui.QLineEdit("%d" % (5))
        iminlabel = QtGui.QLabel("imin")
        iminlabel.setFixedWidth(30)
        self.lay_inputs.addWidget(iminlabel, 0, threshstart)
        self.lay_inputs.addWidget(self.imin, 0, threshstart+1)

        ### imax -- threshold
        self.imax = QtGui.QLineEdit("%d" % (400))
        imaxlabel = QtGui.QLabel("imax")
        imaxlabel.setFixedWidth(30)
        self.lay_inputs.addWidget(imaxlabel, 1, threshstart)
        self.lay_inputs.addWidget(self.imax, 1, threshstart+1)

        ### use user constrast -- threshold
        #Lock imin/imax between events
        self.user_contrast = QtGui.QCheckBox("User Contrast")
        self.lay_inputs.addWidget(self.user_contrast, 2, threshstart, 1, 2)
        self.user_contrast.setChecked(False)
        self.enableContrast()
        self.user_contrast.clicked.connect(self.enableContrast)
        
        # --------------------------------------------------------

        # False color mode
        self.use_false_color = QtGui.QCheckBox("Use False Color")
        self.use_false_color.setChecked(False)
        self.use_false_color.stateChanged.connect( self.enableFalseColor )
        self.false_color_widget = pyqtgraph.GradientWidget(orientation='right')
        # initial map
        state = { 'mode':'rgb', 'ticks':[ (0.0, (0,0,100,255)), (0.5, (128,255,255,255)), (1.0, (255, 0, 0, 255) ) ] }
        self.false_color_widget.restoreState( state )
        self.false_color_widget.sigGradientChangeFinished.connect( self.applyGradientFalseColorMap )
        self.layout.addWidget(self.false_color_widget, 1, 10, 1, 1)
        self.layout.addWidget(self.use_false_color, 0, 8, 1, 2)
        self.false_color_widget.hide()

        # --------------------------------------------------------
        # tmw -- changing it so that one can select the channel to show in the
        # RGB channels
        rgbstart = threshstart+2
        rgblabelwidth = 10
        self.p0label = QtGui.QLabel("R:")
        self.p0label.setFixedWidth(rgblabelwidth)
        self.lay_inputs.addWidget(self.p0label, 0, rgbstart)
        self.p0 = QtGui.QComboBox()
        self.p0.currentIndexChanged.connect( self.changeChannelViewed )
        self.lay_inputs.addWidget(self.p0, 0, rgbstart+1)

        self.p1label = QtGui.QLabel("G:")
        self.p1label.setFixedWidth(rgblabelwidth)
        self.lay_inputs.addWidget(self.p1label, 1, rgbstart)
        self.p1 = QtGui.QComboBox()
        self.p1.currentIndexChanged.connect( self.changeChannelViewed )
        self.lay_inputs.addWidget(self.p1, 1, rgbstart+1)

        self.p2label = QtGui.QLabel("B:")
        self.p2label.setFixedWidth(rgblabelwidth)
        self.lay_inputs.addWidget(self.p2label, 2, rgbstart)
        self.p2 = QtGui.QComboBox()
        self.p2.currentIndexChanged.connect( self.changeChannelViewed )
        self.lay_inputs.addWidget(self.p2, 2, rgbstart+1)

        self.planes = [self.p0, self.p1, self.p2]
        self.views = []

        # ------------------------------------------------
        # Producers

        # Combo box to select the image producer
        combostart = rgbstart+2
        self.lay_inputs.addWidget(QtGui.QLabel("Image2D & ROI Prod."), 0, combostart)
        self.comboBoxImage = QtGui.QComboBox()
        self.image_producer = None
        self.high_res = False
        for prod in self.dm.keys['image2d']:
            self.comboBoxImage.addItem(prod)

        self.lay_inputs.addWidget(self.comboBoxImage, 1, combostart)

        # and another combo box to select ROI
        # self.lay_inputs.addWidget(QtGui.QLabel(
        # "<center>ROI Prod</center>"), 2, 2)


        self.comboBoxROI = QtGui.QComboBox()
        self.roi_producer = None

        if len(self.dm.keys['partroi']) > 0:
            self.roi_exists = True
            for prod in self.dm.keys['partroi']:
                self.comboBoxROI.addItem(prod)
        else:
            self.roi_exists = False
            self.comboBoxROI.addItem("None")

        self.lay_inputs.addWidget(self.comboBoxROI, 2, combostart)

        # --------------------------------------------------
        # particle types
        parstart = combostart+1
        # BNB
        self.kBNB = QtGui.QRadioButton("BNB")
        self.lay_inputs.addWidget(self.kBNB, 0, parstart)
        self.kBNB.setChecked(True)

        # Particle
        self.kOTHER = QtGui.QRadioButton("Particle")
        self.lay_inputs.addWidget(self.kOTHER, 1, parstart)

        # Both
        self.kBOTH = QtGui.QRadioButton("Both")
        self.lay_inputs.addWidget(self.kBOTH, 2, parstart)

        # --------------------------------------------------------
        # Options
        optstart = parstart + 1
        # Auto range function
        self.auto_range = QtGui.QPushButton("AutoRange")
        self.lay_inputs.addWidget(self.auto_range, 0, optstart)
        
        # Save image
        self.savecounter = int(0)
        self.saveimage = QtGui.QPushButton("Save Image")
        self.saveimage.clicked.connect(self.saveImage)
        self.lay_inputs.addWidget(self.saveimage,1,optstart)

        # Yes or no to draw ROI (must hit replot)
        self.draw_bbox = QtGui.QCheckBox("Draw ROI")
        self.draw_bbox.setChecked(True)
        self.lay_inputs.addWidget(self.draw_bbox, 2, optstart)

        # -------------------------------------------------------
        # Utilities
        utilstart = optstart + 1
        # RGBCaffe will open and close bottom of the window
        #utillabel     = QtGui.QLabel("Utilities")
        self.rgbcaffe = QtGui.QPushButton("Enable RGBCaffe")
        self.rgbcv2 = QtGui.QPushButton("Enable OpenCV")
        self.rgbroi = QtGui.QPushButton("Enable ROITool")
        
        try:
            import cv2
        except:
            print "No OpenCV. Disabling."
            self.rgbcv2.setEnabled(False)

        self.rgbcaffe.setFixedWidth(130)
        self.rgbcv2.setFixedWidth(130)
        self.rgbroi.setFixedWidth(130)

        #self.lay_inputs.addWidget(utillabel, 0, utilstart)
        self.lay_inputs.addWidget(self.rgbcaffe, 0, utilstart)
        self.lay_inputs.addWidget(self.rgbcv2, 1, utilstart)
        self.lay_inputs.addWidget(self.rgbroi, 2, utilstart)
        
        # Particle types
        self.kTypes = {'kBNB':   (self.kBNB, [0,2]),
                       'kOTHER': (self.kOTHER, [i for i in xrange(10) if i != 2]),
                       'kBOTH':  (self.kBOTH, [i for i in xrange(10)])}

        # The current image array, useful for getting meta
        self.image = None

        # Radio buttons for choosing type of ROI
        self.kBNB.clicked.connect(
            lambda: self.drawBBOX(self.kTypes['kBNB'][1]))
        self.kOTHER.clicked.connect(
            lambda: self.drawBBOX(self.kTypes['kOTHER'][1]))
        self.kBOTH.clicked.connect(
            lambda: self.drawBBOX(self.kTypes['kBOTH'][1]))

        self.auto_range.clicked.connect(self.autoRange)

        # Set of ROI's on the current view -- just "boxes"
        self.boxes = []

        self.comboBoxImage.activated[str].connect(self.chosenImageProducer)
        self.comboBoxROI.activated[str].connect(self.chosenROIProducer)

        self.chosenImageProducer()
        self.chosenROIProducer()

        self.pimg = None
        self.modimg = None

        self.rgbcaffe.clicked.connect(self.openCaffe)
        self.rgbcv2.clicked.connect(self.openCVEditor)
        self.rgbroi.clicked.connect(self.openROITool)

        # Caffe Widgets
        # wrapper for FORWARD function
        try:
            self.caffe_test = TestWrapper()
            self.caffe_layout = CaffeLayout(self.caffe_test,self)
            self.caffe_enabled = True
        except:
            print "Caffe Disabled"
            self.caffe_enabled = False
            self.rgbcaffe.setEnabled(False)

        # OpenCV Widgets
        # wrapper for the opencv specific window
        try:
            self.cv2_layout = CV2Layout()
        except:
            print "no CV2"
            self.cv2_layout = None
            pass
        self.cv2_enabled = False

        # ROITool
        self.roitool_layout = ROIToolLayout(self.plt,self.image,self.event,self.run,self.subrun,self.event_num,dm=self.dm)
        self.roitool_layout.setImageAndPlotWidgets( self.plt, self.imi )
        self.roitool_enabled = False
        
        # ROI box
        self.swindow = ROISlider([0, 0], [20, 20])
        self.swindow.sigRegionChanged.connect(self.regionChanged)

        # -----------------------------------------------------------------------------
        # Erez, July-21, 2016 - get an image using R/S/E navigation
        # -----------------------------------------------------------------------------
        self.event_base_and_images = {}
        self.rse_map = {}
        print "len(self.event_base_and_images): ",len(self.event_base_and_images)






    # caffe toggles, if/els statement is for opening and closing the pane
    
    # -------------------------
    # ~~~~~~~~~~~~~~~~~~~~~~~~~
    # -------------------------
    # ~~~~~~~~~~~~~~~~~~~~~~~~~
    
    # erez
    def prepare_rse_navigation(self):
        if len(self.rse_map)==0:
            print "preparing R/S/E navigation...."
            self.dm.get_all_images(self.image_producer,self.event_base_and_images,self.rse_map)
            rselist = self.rse_map.keys()
            rselist.sort()
            print rselist
            if self.run.text()=="-1" and self.subrun.text()=="-1" and self.event_num.text()=="-1" and len(rselist)>0:
                self.run.setText("%d"%(rselist[0][0]))
                self.subrun.setText("%d"%(rselist[0][1]))
                self.event_num.setText("%d"%(rselist[0][2]))
                self.event.setText("%d"%(self.rse_map[rselist[0]]))
    
    # ~~~~~~~~~~~~~~~~~~~~~~~~~
    # -------------------------
    # ~~~~~~~~~~~~~~~~~~~~~~~~~
    # -------------------------
    
    
    def openCaffe(self):
        if re.search("Disable", self.rgbcaffe.text()) is None:
            self.rgbcaffe.setText("Disable RGBCaffe")
            self.resize(1200, 900)
            self.layout.addLayout(self.caffe_layout.grid(True), 5, 0)
        else:
            self.rgbcaffe.setText("Enable RGBCaffe")
            self.layout.removeItem(self.caffe_layout.grid(False))
            self.resize(1200, 700)

    def openROITool(self):
        if re.search("Disable", self.rgbroi.text()) is None:
            self.rgbroi.setText("Disable ROITool")
            self.resize(1200, 1000)
            self.layout.addLayout(self.roitool_layout.grid(True), 5, 0)
        else:
            self.rgbroi.setText("Enable ROITool")
            self.layout.removeItem(self.roitool_layout.grid(False))
            self.resize(1200, 700)

    # opencv editor, if/els statement is for opening and closing the pane
    def openCVEditor(self):
        if self.cv2_layout is None:
            return

        if re.search("Disable", self.rgbcv2.text()) is None:
            self.rgbcv2.setText("Disable OpenCV")
            self.resize(1200, 900)
            self.layout.addLayout(self.cv2_layout.grid(True), 5, 0)
            self.plt.addItem(self.swindow)
            self.cv2_enabled = True
        else:
            self.rgbcv2.setText("Enable OpenCV")
            self.layout.removeItem(self.cv2_layout.grid(False))
            self.resize(1200, 700)
            self.plt.removeItem(self.swindow)
            self.cv2_enabled = False

    # put the runinfo right above the graphics window
    def setRunInfo(self, run, subrun, event):
        self.runinfo.setText(
            "<b>Run:</b> {} <b>Subrun:</b> {} <b>Event:</b> {}".format(run, subrun, event))

    # which image producer
    def chosenImageProducer(self):
        self.image_producer = str(self.comboBoxImage.currentText())

    # which ROI producer
    def chosenROIProducer(self):
        if self.roi_exists == True:
            self.roi_producer = str(self.comboBoxROI.currentText())

    # set the ticks axis to be absolute coordinates
    def get_ticks(self):
        # everywhere USE ABSOLUTE COORDINATE (which is in tick/wire)
        meta = self.image.imgs[0].meta()
        xmax, ymax = meta.max_x(), meta.max_y()
        xmin, ymin = meta.min_x(), meta.min_y()

        comp_y = meta.height() / meta.rows()
        comp_x = meta.width() / meta.cols()

        dx = meta.width()
        dy = meta.height()

        ymajor = []
        yminor = []
        yminor2 = []
        xmajor = []
        xminor = []
        xminor2 = []

        for ix, y in enumerate(np.arange(int(ymin), int(ymax), comp_y)):

            label = (ix, int(y))

            if ix % 10 != 0:
                yminor2.append(label)
                continue

            if ix % 25 != 0:
                yminor.append(label)
                continue

            ymajor.append(label)

        for ix, x in enumerate(np.arange(int(xmin), int(xmax), comp_x)):

            label = (ix, int(x))

            if ix % 100 != 0:
                xminor2.append(label)
                continue

            if ix % 200 != 0:
                xminor.append(label)
                continue

            xmajor.append(label)

        return ([xmajor, xminor, xminor2], [ymajor, yminor, yminor2])

    # autorange button
    def autoRange(self):

        xticks, yticks = self.get_ticks()

        self.plt_y.setTicks(yticks)
        self.plt_x.setTicks(xticks)

        self.plt.autoRange()
        self.setRunInfo(self.dm.run,
                        self.dm.subrun,
                        self.dm.event)

        self.run.setText( "%d"%(self.dm.run) )
        self.subrun.setText( "%d"%(self.dm.subrun) )
        self.event_num.setText( "%d"%(self.dm.event) )

        if self.cv2_enabled == True:
            self.plt.addItem(self.swindow)
            self.swindow.setZValue(10)
            
        self.modimage = None

    #which type of ROI do you want, BNB/particle/both?
    def which_type(self):

        for button in self.kTypes:
            if self.kTypes[button][0].isChecked():
                return self.kTypes[button][1]
        return None

    # go to the previous event
    def previousEvent(self):

        event = int(self.event.text())

        if event == 0: return

        self.event.setText(str(event - 1))

        self.plotData()

    # go to the next event
    def nextEvent(self):

        event = int(self.event.text())
        self.event.setText(str(event + 1))

        self.plotData()

    def setViewPlanes(self):
        # the list of chosen views 
        self.views = []
        for ix, p in enumerate(self.planes):
            if p.currentIndex() != 0:
                idx = p.currentText()
                if idx == '': idx = -1 # when first loading the image it's empty, catch it
                self.views.append(int(idx))
            else:
                self.views.append(-1)  # sentinal for don't fill this channel

    def plotData(self):


        # if there are presets clear them out
        if hasattr(self.image,"preset_layout"):
            self.image.reset_presets()
            self.image.preset_layout.setParent(None)
            self.layout.removeItem(self.image.preset_layout)

        # Clear the image pointer
        self.image = None

        # Clear out plot
        self.plt.clear()

        # Add image
        self.imi = pg.ImageItem()
        self.plt.addItem(self.imi)
        self.roitool_layout.setImageAndPlotWidgets( self.plt, self.imi )

        # From QT, the threshold
        event = int(self.event.text())
        
        # -----------------------------------------------------------------------------
        # Erez, July-21, 2016
        # -----------------------------------------------------------------------------
        if self.rse_navigation.isChecked() : # my way
            #self.wanted_rse = [int(self.run.text()), int(self.subrun.text()), int(self.event_num.text())]
            self.wanted_rse = ( int(self.run.text()), int(self.subrun.text()), int(self.event_num.text()) )
            self.image, hasroi = self.dm.get_rse_image(self.event_base_and_images,
                                                       self.rse_map,
                                                       self.wanted_rse,
                                                       self.image_producer,
                                                       self.roi_producer,
                                                       self.views)
        
       
        else: # original way
            # get the image from the datamanager
            self.image, hasroi = self.dm.get_event_image(event,
                                                         self.image_producer,
                                                         self.roi_producer,
                                                         self.views)
        # -----------------------------------------------------------------------------



        # whoops no image, return
        if self.image == None: return
        
        self.image.planes = self.planes
        if hasattr(self.image,"preset_layout"):
            self.layout.addLayout(self.image.preset_layout,4,0)

        # update channel combo boxes
        nchs = self.dm.get_nchannels(event, self.image_producer)
        if self.p0.count() != (nchs + 1):
            self.p0.clear()
            self.p1.clear()
            self.p2.clear()
            # the none channel must be zero. else we have to know the number of
            # channels
            self.p0.insertItem(0, "(none)")
            self.p1.insertItem(0, "(none)")
            self.p2.insertItem(0, "(none)")
            for ch in range(0, nchs):
                self.p0.insertItem(ch + 1, "%d" % (ch))
                self.p1.insertItem(ch + 1, "%d" % (ch))
                self.p2.insertItem(ch + 1, "%d" % (ch))
            if nchs > 0:
                self.p0.setCurrentIndex(1)
            if nchs > 1:
                self.p1.setCurrentIndex(nchs / 3 + 1)
            if nchs > 2:
                self.p2.setCurrentIndex(nchs / 3 * 2 + 1)

        self.setViewPlanes()

        self.setContrast()

        # threshold for contrast, this image goes to the screen
        self.pimg = self.image.set_plot_mat(self.iimin,self.iimax)

        if hasroi:
            self.rois = self.image.parse_rois()

        # Emplace the image on the canvas
        #self.imi.setImage(self.pimg)
        self.setImage(self.pimg)
        self.modimage = None

        #this is extremely hacky, we need a central layout manager that can alert the layouts that
        #the event is changed. For now lets directly tell ROILayout

        #start vichack
        self.roitool_layout.setImages( (int(self.event.text()),int(self.run.text()), int(self.subrun.text()), int(self.event_num.text())), self.image )
        self.roitool_layout.reloadROI()
        #end vichack
        
        # no ROI's -- finish early
        if hasroi == False:
            self.autoRange()
            return

        xmin, xmax, ymin, ymax = (1e9, 0, 1e9, 0)
        for roi in self.rois:
            for bb in roi['bbox']:
                if xmin > bb.min_x():
                    xmin = bb.min_x()
                if xmax < bb.max_x():
                    xmax = bb.max_x()
                if ymin > bb.min_y():
                    ymin = bb.min_y()
                if ymax < bb.max_y():
                    ymax = bb.max_y()

        if self.roi_exists == True:
            self.drawBBOX(self.which_type())

        self.autoRange()

    def regionChanged(self):

        # the boxed changed but we don't intend to transform the image
        if self.cv2_layout is not None and self.cv2_layout.transform == False:
            return

        # the box has changed location, if we don't have a mask, create on
        if self.modimage is None:
            self.modimage = np.zeros(list(self.image.orig_mat.shape))

        # get the slice for the movable box
        sl = self.swindow.getArraySlice(self.image.orig_mat, self.imi)[0]

        # need mask if user doesn't want to overwrite their manipulations
        if self.cv2_layout is not None and self.cv2_layout.overwrite == False:
            idx = np.where(self.modimage == 1)
            pcopy = self.image.orig_mat.copy()

        # do the manipulation
        if self.cv2_layout is not None:
            self.image.orig_mat[sl] = self.cv2_layout.paint( self.image.orig_mat[sl] )

        # use mask to updated only pixels not already updated
        if self.cv2_layout is not None and self.cv2_layout.overwrite == False:
            # reverts prev. modified pixels, preventing double change
            self.image.orig_mat[idx] = pcopy[idx]
            self.modimage[sl] = 1

        # we manipulated orig_mat, threshold for contrast, make sure pixels do not block
        self.pimg = self.image.set_plot_mat(self.iimin,self.iimax)  

        # return the plot image to the screen
        #self.imi.setImage(self.pimg) # send it back to the viewer
        self.setImage(self.pimg) # send it back to the viewer

    def drawBBOX(self, kType):

        # set the planes to be drawn
        self.setViewPlanes()

        # no image to draw ontop of
        if self.image is None:
            return

        # no type to draw
        if kType is None:
            return

        # remove the current set of boxes
        for box in self.boxes:
            self.plt.removeItem(box)

        # if thie box is unchecked don't draw it
        if self.draw_bbox.isChecked() == False:
            return

        # clear boxes explicitly
        self.boxes = []

        # and makew new boxes
        for roi_p in self.rois:

            if roi_p['type'] not in kType:
                continue

            for ix, bbox in enumerate(roi_p['bbox']):

                if ix not in self.views: 
                    continue

                imm = self.image.imgs[ix].meta()

                # x,y  relative coordinate of bounding-box w.r.t. image in original unit
                x = bbox.min_x() - imm.min_x()
                y = bbox.min_y() - imm.min_y()

                # dw_i is an image X-axis unit legnth in pixel. dh_i for
                # Y-axis. (i.e. like 0.5 pixel/cm)
                dw_i = imm.cols() / (imm.max_x() - imm.min_x())
                dh_i = imm.rows() / (imm.max_y() - imm.min_y())

                # w_b is width of a rectangle in original unit
                w_b = bbox.max_x() - bbox.min_x()
                h_b = bbox.max_y() - bbox.min_y()

                ti = pg.TextItem(text=larcv.ROIType2String(roi_p['type']))
                ti.setPos(x * dw_i, (y + h_b) * dh_i + 1)

                print str(self.event.text()),x * dw_i, y * dh_i, w_b * dw_i, h_b * dh_i,"\n"

                r1 = HoverRect(x * dw_i,
                               y * dh_i,
                               w_b * dw_i,
                               h_b * dh_i,
                               ti,
                               self.plt)
                
                r1.setPen(pg.mkPen(store.colors[ix]))
                r1.setBrush(pg.mkBrush(None))
                self.plt.addItem(r1)
                self.boxes.append(r1)

    # user scrolled to another channel
    def changeChannelViewed(self):

        # fill self.views -- the indicies of the chosen channels
        self.setViewPlanes()

        # get the contrast
        self.setContrast()
        
        # swap what is in work_mat to orig_mat and do the thresholding
        self.pimg = self.image.swap_plot_mat( self.iimin, self.iimax, self.views )

        # set the image for the screen
        #self.imi.setImage(self.pimg)
        self.setImage(self.pimg)

    # you probably hit "forward" so load the current image into the wrapper
    # through caffe_layout.py
    def load_current_image(self):

        print "Loading current image!"
        
        # revert the image back to Image2D.nd_array style
        self.image.revert_image()

        # make caffe_image which would be different than image2d, i.e.
        # do some operation on work_mat if you want
        self.image.emplace_image()

        # revert work_mat/orig_mat back since we made a copy into
        # self.image.caffe_copy
        self.image.revert_image()

        # send off to the network (through caffe_layout.py)
        return self.image.caffe_image

    def setContrast(self):
        
        assert self.image is not None

        self.image.set_imin_imax()
        
        if self.user_contrast.isChecked() == True:
            self.iimin = float(self.imin.text())
            self.iimax = float(self.imax.text())
        else: # get it from the max and min value of image
            self.iimin = self.image.iimin
            self.iimax = self.image.iimax
            self.imin.setText("%.2f"%(self.iimin))
            self.imax.setText("%.2f"%(self.iimax))
            #print "Setting self.imin text {}=>{}".format(self.image.iimin,self.iimin)
            #print "Setting self.imax text {}=>{}".format(self.image.iimax,self.iimax)

    def saveImage(self):
        exporter = pg.exporters.ImageExporter(self.plt)
        #not sure what it the best way to do this...
        #exporter.parameters()['width']  = 700   # (note this also affects height parameter)
        exporter.parameters()['height'] = 700 
        exporter.export('R{}_S{}_E{}.png'.format(str(self.run.text()),str(self.subrun.text()),str(self.event.text())))
#        exporter.export('saved_image_{}_{}.png'.format(str(self.event.text()),self.savecounter))
        print "Saved image {}".format(self.savecounter)
        self.savecounter += 1

    def setImage( self, img ):
        """Wrapper for hacking. """
        if img is None:
            # sometimes no image to set yet
            return

        if not self.use_false_color.isChecked():
            self.imi.setImage( img )
        else:
            self.applyGradientFalseColorMap()
            #print "flatten image and set it"
            flatten = np.sum( img, axis=2 )
            self.imi.setImage( flatten )
        

    def enableContrast(self):

        if self.user_contrast.isChecked() == True:
            self.imin.setDisabled(False)
            self.imax.setDisabled(False)
        else :
            self.imin.setDisabled(True)
            self.imax.setDisabled(True)

    def applyGradientFalseColorMap(self):
        """ connected to false color widget signal: sigGradientChangeFinished.
            job is to set the color map of the image plot (self.imi)"""
        if self.use_false_color.isChecked():
            #print "Set false color scale"
            self.lut = self.false_color_widget.colorMap().getLookupTable(0.0, 1.0, 256)
            try:
                self.imi.setLookupTable(self.lut)
            except:
                pass

    def enableFalseColor(self):
        """ connected to false color check box: self.use_false_color. 
            when checkbox toggled, this is called to activate false color drawing.
            you can find how that is being done in setImage. """
        # colorscale:
        if self.use_false_color.isChecked():
            self.false_color_widget.show()
            self.applyGradientFalseColorMap()
            self.setImage( self.pimg )
        else:
            # attempt to reset it
            #print "restore false color scale"
            self.false_color_widget.hide()
            try:
                self.imi.setLookupTable(None)
                self.imi.setLevels( [[0,255],[0,255],[0,255]] )
            except:
                pass
            self.setImage( self.pimg )

    def _makeNavFrame(self):
        self._navframe = QtGui.QFrame()
        self._navlayout = QtGui.QGridLayout()

        labelwidth = 40
        inputwidth = 60

        # Navigation between entries
        self.event = QtGui.QLineEdit("%d" % 0)      # entries number
        entry_label = QtGui.QLabel("entry")
        entry_label.setFixedWidth(labelwidth)
        
        # -------------------------------------------------------
        # erez - July-21, 2016
        # -------------------------------------------------------
        # Navigation - run / sub / event from run-info
        self.run = QtGui.QLineEdit("%d" % -1)      # run
        self.subrun = QtGui.QLineEdit("%d" % -1)      # subrun
        self.event_num = QtGui.QLineEdit("%d" % -1)      # event
        run_label = QtGui.QLabel("run")
        run_label.setFixedWidth(labelwidth)
        event_label = QtGui.QLabel("event")
        event_label.setFixedWidth(labelwidth)
        subrun_label = QtGui.QLabel("subrun")
        subrun_label.setFixedWidth(labelwidth)
        for input in [ self.run, self.subrun, self.event_num, self.event ]:
            input.setFixedWidth( inputwidth )
        
        self._navlayout.addWidget( run_label, 0, 0 )
        self._navlayout.addWidget( subrun_label, 1, 0 )
        self._navlayout.addWidget( event_label, 2, 0 )
        self._navlayout.addWidget( entry_label, 3, 0 )

        self._navlayout.addWidget( self.run, 0, 1 )
        self._navlayout.addWidget( self.subrun, 1, 1 )
        self._navlayout.addWidget( self.event_num, 2, 1 )
        self._navlayout.addWidget( self.event,  3, 1 )

        # Yes or no to navigate using R/S/E
        self.rse_navigation = QtGui.QCheckBox("R/S/E navigation")
        self.rse_navigation.setChecked(False)#True)
        self.rse_navigation.stateChanged.connect( self.prepare_rse_navigation )
        self._navlayout.addWidget( self.rse_navigation, 0, 2, 1, 2 )

        # select choice options
        self.axis_plot = QtGui.QPushButton("Go/Replot")
        self.previous_plot = QtGui.QPushButton("Prev Event")
        self.next_plot = QtGui.QPushButton("Next Event")

        # prev, next, replot
        self._navlayout.addWidget( self.previous_plot, 1, 2, 1, 2 )
        self._navlayout.addWidget( self.next_plot,     2, 2, 1, 2 )
        self._navlayout.addWidget( self.axis_plot,     3, 2, 1, 2 )

        # select choice option: redundant, because we have the RSE checkbox
        #self.go_rse_plot = QtGui.QPushButton("Go R/S/E")
        #self.lay_inputs.addWidget(self.go_rse_plot, 1, 2)

        self.navheight = 4
        self.navwidth  = 4

        # [ signal connect ]

        # (Re)Plot button
        self.axis_plot.clicked.connect(self.plotData)

        # Previous and Next event
        self.previous_plot.clicked.connect(self.previousEvent)
        self.next_plot.clicked.connect(self.nextEvent)
        #self.go_rse_plot.clicked.connect(self.go_rse_Event) # erez, july-21,2016

        # set frame
        self._navframe.setLayout( self._navlayout )

