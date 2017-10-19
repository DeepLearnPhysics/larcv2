# thanks taritree

import os
import sys
import copy
import re
from .. import QtGui, QtCore
from .. import pg


import numpy as np
import time

from ..lib.whitedatamanager import WhiteDataManager
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

import pandas as pd

import pyqtgraph.exporters

class WhiteDisplay(QtGui.QWidget):

    def __init__(self, argv):
        super(WhiteDisplay, self).__init__()

        # DataManager for loading the plot image
        self.dm = WhiteDataManager(argv)

        # Size the canvas
        self.resize(700, 700)

        # Graphics window which will hold the image
        self.win = pg.GraphicsWindow()
        self.plt = self.win.addPlot()

        # Handles to the axis which we will update with wire/tick
        self.plt_x = self.plt.getAxis('bottom')
        self.plt_y = self.plt.getAxis('left')

        
        self.NEU = None
        
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

        # Navigation
        self.event = QtGui.QLineEdit("%d" % 0)      # event number
        self.lay_inputs.addWidget(QtGui.QLabel("Entry"), 0, 0)
        self.lay_inputs.addWidget(self.event, 0, 1)

        ### imin -- threshold
        self.imin = QtGui.QLineEdit("%d" % (5))
        self.lay_inputs.addWidget(QtGui.QLabel("imin"), 1, 0)
        self.lay_inputs.addWidget(self.imin, 1, 1)

        ### imax -- threshold
        self.imax = QtGui.QLineEdit("%d" % (400))
        self.lay_inputs.addWidget(QtGui.QLabel("imax"), 2, 0)
        self.lay_inputs.addWidget(self.imax, 2, 1)

        # select choice options
        self.axis_plot = QtGui.QPushButton("Replot")
        self.lay_inputs.addWidget(self.axis_plot, 2, 2)

        self.previous_plot = QtGui.QPushButton("Prev Event")
        self.lay_inputs.addWidget(self.previous_plot, 1, 2)

        self.next_plot = QtGui.QPushButton("Next Event")
        self.lay_inputs.addWidget(self.next_plot, 0, 2)

        # particle types
        # BNB
        self.kBNB = QtGui.QRadioButton("BNB")
        self.lay_inputs.addWidget(self.kBNB, 0, 6)
        self.kBNB.setChecked(True)

        # Particle
        self.kOTHER = QtGui.QRadioButton("Particle")
        self.lay_inputs.addWidget(self.kOTHER, 1, 6)

        # Both
        self.kBOTH = QtGui.QRadioButton("Both")
        self.lay_inputs.addWidget(self.kBOTH, 2, 6)

        # tmw -- changing it so that one can select the channel to show in the
        # RGB channels
        self.p0label = QtGui.QLabel("R:")
        self.lay_inputs.addWidget(self.p0label, 0, 4)
        self.p0 = QtGui.QComboBox()
        self.p0.currentIndexChanged.connect( self.changeChannelViewed )
        self.lay_inputs.addWidget(self.p0, 0, 5)

        self.p1label = QtGui.QLabel("G:")
        self.lay_inputs.addWidget(self.p1label, 1, 4)
        self.p1 = QtGui.QComboBox()
        self.p1.currentIndexChanged.connect( self.changeChannelViewed )
        self.lay_inputs.addWidget(self.p1, 1, 5)

        self.p2label = QtGui.QLabel("B:")
        self.lay_inputs.addWidget(self.p2label, 2, 4)
        self.p2 = QtGui.QComboBox()
        self.p2.currentIndexChanged.connect( self.changeChannelViewed )
        self.lay_inputs.addWidget(self.p2, 2, 5)

        self.planes = [self.p0, self.p1, self.p2]
        self.views = []
        
        #aho hack
        try:
            self.nu_dboxes = pd.read_csv('/home/vgenty/nus.txt',
                                         delimiter=' ',header=None,names=['entry','prob','x1','y1','x2','y2','gt'])
            self.cosmo_dboxes = pd.read_csv('/home/vgenty/dets.txt',
                                            delimiter=' ',header=None,names=['entry','prob','x1','y1','x2','y2'])
            self.particle_dboxes = pd.read_csv('/home/vgenty/single_dets.txt',
                                               delimiter=' ',header=None,names=['entry','class','prob','x1','y1','x2','y2','gt'])

        except IOError:
            self.nu_dboxes = None
            self.cosmo_dboxes = None
            self.particle_dboxes = None

            
        # Combo box to select the image producer
        self.lay_inputs.addWidget(QtGui.QLabel("Image2D & ROI Prod."), 0, 3)
        self.comboBoxImage = QtGui.QComboBox()
        self.image_producer = None
        self.high_res = False
        for prod in self.dm.keys['image2d']:
            self.comboBoxImage.addItem(prod)

        self.lay_inputs.addWidget(self.comboBoxImage, 1, 3)

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

        self.lay_inputs.addWidget(self.comboBoxROI, 2, 3)

        #Lock imin/imax between events
        self.user_contrast = QtGui.QCheckBox("User Contrast")
        self.lay_inputs.addWidget(self.user_contrast, 1, 7)
        self.user_contrast.setChecked(False)
        
        # Auto range function
        self.auto_range = QtGui.QPushButton("AutoRange")
        self.lay_inputs.addWidget(self.auto_range, 0, 8)


        # Yes or no to draw ROI (must hit replot)
        self.draw_bbox = QtGui.QCheckBox("Draw ROI")
        self.draw_bbox.setChecked(True)
        self.lay_inputs.addWidget(self.draw_bbox, 0, 7)

        # RGBCaffe will open and close bottom of the window
        self.rgbcaffe = QtGui.QPushButton("Enable RGBCaffe")
        self.rgbcv2 = QtGui.QPushButton("Enable OpenCV")
        try:
            import cv2
        except:
            print "No OpenCV. Disabling."
            self.rgbcv2.setEnabled(False)

        self.rgbcaffe.setFixedWidth(130)
        self.rgbcv2.setFixedWidth(130)

        self.lay_inputs.addWidget(self.rgbcaffe, 1, 8)
        self.lay_inputs.addWidget(self.rgbcv2, 2, 8)

        # Particle types
        self.kTypes = {'kBNB':   (self.kBNB, [2]),
                       'kOTHER': (self.kOTHER, [i for i in xrange(10) if i != 2]),
                       'kBOTH':  (self.kBOTH, [i for i in xrange(10)])}

        # The current image array, useful for getting meta
        self.image = None

        # (Re)Plot button
        self.axis_plot.clicked.connect(self.plotData)

        # Previous and Next event
        self.previous_plot.clicked.connect(self.previousEvent)
        self.next_plot.clicked.connect(self.nextEvent)

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
        self.cv2_layout = CV2Layout()
        self.cv2_enabled = False

        # ROI box
        self.swindow = ROISlider([0, 0], [20, 20])
        self.swindow.sigRegionChanged.connect(self.regionChanged)

    # caffe toggles, if/els statement is for opening and closing the pane
    def openCaffe(self):
        if re.search("Disable", self.rgbcaffe.text()) is None:
            self.rgbcaffe.setText("Disable RGBCaffe")
            self.resize(1200, 900)
            self.layout.addLayout(self.caffe_layout.grid(True), 5, 0)
        else:
            self.rgbcaffe.setText("Enable RGBCaffe")
            self.layout.removeItem(self.caffe_layout.grid(False))
            self.resize(1200, 700)

    # opencv editor, if/els statement is for opening and closing the pane
    def openCVEditor(self):
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

        # From QT, the threshold
        event = int(self.event.text())
            
        # get the image from the datamanager
        self.image, hasroi = self.dm.get_event_image(event,
                                                     self.image_producer,
                                                     self.roi_producer,
                                                     self.views)

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

        #self.imi.setImage(self.pimg,levels=[[10,100],  #aho hack levels
        #                                    [10,100],  #aho hack levels
        #                                    [10,100]]) #aho hack levels
        # Emplace the image on the canvas
        #self.imi.setImage(self.pimg,levels=[[100,200],[100,200],[0,150]])
        self.imi.setImage(self.pimg,levels=[[100,200],[100,200],[0,150]])

        self.modimage = None

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
        

        NEU = 1
        self.NEU = NEU

        if NEU == 1 and self.nu_dboxes is not None:
            
            if event > 14000:
                a = self.nu_dboxes.query('entry == {}'.format(event)).sort_values(by='prob',ascending=False).iloc[0]
            elif event == 416:
                a = self.cosmo_dboxes.query('entry == {}'.format(event)).sort_values(by='prob',ascending=False).iloc[0]
            else:
                a = self.cosmo_dboxes.query('entry == {}'.format(event))
        
            if event > 1400 or event == 416:
                # x,y below are relative coordinate of bounding-box w.r.t.
                # image in original unit
                print a
                bbminx = a['x1']
                bbminy = a['y1']
                bbmaxx = a['x2']
                bbmaxy = a['y2']
                
                w_b = bbmaxx - bbminx
                h_b = bbmaxy - bbminy
                
                ti = pg.TextItem(fill='r',html='<font color="white" size="5"><b>Nu: {}</b></font>'.format(np.round(a['prob'],3)),anchor=(0,0))
                ti.setPos(bbminy-1,756-bbmaxx + w_b + 35)

                    
                r1 = HoverRect(bbminy,
                               756-bbmaxx,
                               h_b,
                               w_b,
                               ti,
                               self.plt)
                
        
                r1.setPen(pg.mkPen('r',width=3))
                r1.setBrush(pg.mkBrush(None))
                self.plt.addItem(r1)
                self.plt.addItem(ti)
            else:
                for iy,b in a.iterrows():
                    bbminx = b['x1']
                    bbminy = b['y1']
                    bbmaxx = b['x2']
                    bbmaxy = b['y2']
                    
                    w_b = bbmaxx - bbminx
                    h_b = bbmaxy - bbminy
                    
                    ti = pg.TextItem(fill='r',html='<font color="white" size="5"><b>Nu: {0:.3f}</b></font>'.format(b['prob']),anchor=(0,0))
                    ti.setPos(bbminy-1,756-bbmaxx + w_b + 35)
                    
                    r1 = HoverRect(bbminy,
                                   756-bbmaxx,
                                   h_b,
                                   w_b,
                                   ti,
                                   self.plt)
        
                    r1.setPen(pg.mkPen('r',width=3))
                    r1.setBrush(pg.mkBrush(None))
                    self.plt.addItem(r1)
                    self.plt.addItem(ti)
        
            
            
            
        elif self.particle_dboxes is not None: #not neu
            
            print self.particle_dboxes
            a = self.particle_dboxes.query('entry == {}'.format(event)).sort_values(by='prob',ascending=False).iloc[0]
            print a
            bbminx = a['x1']
            bbminy = a['y1']
            bbmaxx = a['x2']
            bbmaxy = a['y2']
            
            w_b = bbmaxx - bbminx
            h_b = bbmaxy - bbminy

            #0:.3f
            prob = float(a['prob'])
            _class = {}
            _class['Eminus'] = 'Eminus'
            _class['Gamms']  = 'Gamma'
            _class['Proton'] = 'Proton'
            _class['Piminus'] = 'Piminus'
            _class['Muminus'] = 'Muminus'

            ti = pg.TextItem(fill='r',html='<font color="white" size="6"><b>{}: {}</b></font>'.format(_class[a['class']],np.round(prob,3)),anchor=(0,0))
            ti.setPos(bbminx,bbmaxy+34)
            
            r1 = HoverRect(bbminx,
                           bbminy,
                           w_b,
                           h_b,
                           ti,
                           self.plt)
            
            r1.setPen(pg.mkPen('r',width=3))
            r1.setBrush(pg.mkBrush(None))
            self.plt.addItem(r1)
            self.plt.addItem(ti)

             
        self.autoRange()
        for loc in ['left','bottom','top','right']:
            self.plt.hideAxis(loc)
            
             
        exporter = pg.exporters.ImageExporter(self.plt)

        #exporter.parameters()['width']  = 700   # (note this also affects height parameter)
        exporter.parameters()['height'] = 700   # (note this also affects height parameter)

        # save to file
        if NEU == 1:
            exporter.export('nu_color_{}_blue.png'.format(event))
        else:
            exporter.export('par_color_{}_blue.png'.format(event))

    def regionChanged(self):

        # the boxed changed but we don't intend to transform the image
        if self.cv2_layout.transform == False:
            return

        # the box has changed location, if we don't have a mask, create on
        if self.modimage is None:
            self.modimage = np.zeros(list(self.image.orig_mat.shape))

        # get the slice for the movable box
        sl = self.swindow.getArraySlice(self.image.orig_mat, self.imi)[0]

        # need mask if user doesn't want to overwrite their manipulations
        if self.cv2_layout.overwrite == False:
            idx = np.where(self.modimage == 1)
            pcopy = self.image.orig_mat.copy()

        # do the manipulation
        self.image.orig_mat[sl] = self.cv2_layout.paint( self.image.orig_mat[sl] )

        # use mask to updated only pixels not already updated
        if self.cv2_layout.overwrite == False:
            # reverts prev. modified pixels, preventing double change
            self.image.orig_mat[idx] = pcopy[idx]
            self.modimage[sl] = 1

        # we manipulated orig_mat, threshold for contrast, make sure pixels do not block
        self.pimg = self.image.set_plot_mat(self.iimin,self.iimax)  

        # return the plot image to the screen
        self.imi.setImage(self.pimg) # send it back to the viewer

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
        
        def union(xx,yy) :
            xmin = 99999
            xmax = 0
            
            ymin = 99999
            ymax = 0
            
            for ix in xrange(len(xx)):
                x = xx[ix]
                y = yy[ix]

                if x < xmin: xmin = x
                if x > xmax: xmax = x
            
                if y < ymin: ymin = y
                if y > ymax: ymax = y
        
            return (xmin,ymin,xmax,ymax)

        xx = []
        yy = []
        
        # and makew new boxes
        for roi_p in self.rois:

            if roi_p['type'] not in kType:
                continue
                            
            for ix, bbox in enumerate(roi_p['bbox']):

                if ix not in self.views: 
                    continue
                
                if ix != 2: #aho hack
                    continue
                
                imm = self.image.imgs[ix].meta()
                
            
                # x,y below are relative coordinate of bounding-box w.r.t.
                # image in original unit
                x = bbox.min_x() - imm.min_x()
                y = bbox.min_y() - imm.min_y()

                # dw_i is an image X-axis unit legnth in pixel. dh_i for
                # Y-axis. (i.e. like 0.5 pixel/cm)
                dw_i = imm.cols() / (imm.max_x() - imm.min_x())
                dh_i = imm.rows() / (imm.max_y() - imm.min_y())

                # w_b is width of a rectangle in original unit
                w_b = bbox.max_x() - bbox.min_x()
                h_b = bbox.max_y() - bbox.min_y()

                ti = pg.TextItem(text=store.particle_types[roi_p['type']])
                ti.setPos(x * dw_i, (y + h_b) * dh_i + 1)

                print x * dw_i, y * dh_i, w_b * dw_i, h_b * dh_i
                
                
                if len(roi_p['bbox']) == 3 and self.NEU==0:
                    print "aho1"
                    xx.append( x*dw_i )
                    xx.append( ( x+w_b )*dw_i )
                    yy.append( y*dh_i )
                    yy.append( ( y+h_b )*dh_i )
                    continue

                r1 = HoverRect(x * dw_i,
                               y * dh_i,
                               w_b * dw_i,
                               h_b * dh_i,
                               ti,
                               self.plt)

                r1.setPen(pg.mkPen(color='y',width=3))
                r1.setBrush(pg.mkBrush(None))
                self.plt.addItem(r1)
                self.boxes.append(r1)

            if len(roi_p['bbox']) == 3 and self.NEU==0:
                print "AHO"
                xmin,ymin,xmax,ymax = union(xx,yy)
                r1 = HoverRect(xmin,
                               ymin,
                               xmax-xmin,
                               ymax-ymin,
                               ti,
                               self.plt)
                r1.setPen(pg.mkPen(color='y',width=3))
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
        self.imi.setImage(self.pimg)

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
            self.imin.setText(str(self.iimin))
            print "Setting self.imin text {}=>{}".format(self.image.iimin,self.iimin)
            self.iimax = self.image.iimax
            self.imax.setText(str(self.iimax))
            print "Setting self.imax text {}=>{}".format(self.image.iimax,self.iimax)
            
