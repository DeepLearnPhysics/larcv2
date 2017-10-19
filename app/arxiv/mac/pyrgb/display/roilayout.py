from .. import pg
from .. import QtGui, QtCore
from ..lib.roislider import ROISliderGroup
from ..lib import storage as store
from ..lib.iomanager import IOManager
from .. import larcv

import os
from crosshairs import CrossHairs
from LabelingTool import LabelingTool

class ROIToolLayout(QtGui.QGridLayout):

    def __init__(self,plt,images,event,run,subrun,event_num, dm=None):
        """
        variables
        ---------
        plt: PlotItem
        images: inherits from lib/image_types/defaultimage.py
        event: ??
        run: QLineEdit
        subrun: QLineEdit
        event_num: ??
        dm: datamanager
        """

        super(ROIToolLayout, self).__init__()

        # Sliding ROI which we will do OpenCV manipulations on
        self.name = "ROIToolLayout"
        # store a copy of pointer to data-manager
        self.dm = dm

        self.enabled = False
        self.cv2 = None
        self.imi = None
        self.overwrite = False
        self.transform = True

        self.title  = QtGui.QLabel("<b>ROI Tool</b>")

        # ROI File Interace
        self._makeFileFrame()
        
        self.add_roi = QtGui.QPushButton("Add ROIs")
        self.remove_roi = QtGui.QPushButton("Remove ROI")
        self.clear_roi = QtGui.QPushButton("Clear ROIs")
        self.capture_roi = QtGui.QPushButton("Capture ROIs")
        self.empty_roi   = QtGui.QPushButton("Add Empty ROI")
        self.store_roi = QtGui.QPushButton("Store ROIs")
        self.reset_roi = QtGui.QPushButton("Reset ROIs")


        self.roi_p1 = QtGui.QCheckBox("R:")
        self.roi_p1.setChecked(True)
        self.roi_p2 = QtGui.QCheckBox("G:")
        self.roi_p2.setChecked(True)
        self.roi_p3 = QtGui.QCheckBox("B:")
        self.roi_p3.setChecked(True)
        
        self.fixed_roi_box = QtGui.QCheckBox("Fixed ROI")
        self.fixed_roi_box.setChecked(False)

        # tmw -- add same ROI time
        self.same_roi_time = QtGui.QCheckBox("Same ROI time")
        self.same_roi_time.setChecked(True)

        # tmw -- add option to save ROI by 
        self.save_roi_RSE = QtGui.QCheckBox("Save ROI RSE")
        self.save_roi_RSE.setChecked(True)

        # tmw -- labels to track position of the boxes
        self.uplane_pos = QtGui.QLabel("")
        self.vplane_pos = QtGui.QLabel("")
        self.yplane_pos = QtGui.QLabel("")
        
        self.fixed_w_label = QtGui.QLabel("W:")
        self.fixed_h_label = QtGui.QLabel("H:")

        self.fixed_w = QtGui.QLineEdit("30")
        self.fixed_h = QtGui.QLineEdit("30")
        
        self.add_roi.clicked.connect(self.addROI)
        self.remove_roi.clicked.connect(self.removeROI)
        self.clear_roi.clicked.connect(self.clearROI)
        self.reset_roi.clicked.connect(self.resetROI)
        self.capture_roi.clicked.connect(self.captureROI)
        self.empty_roi.clicked.connect(self.makeEmptyROI)
        self.store_roi.clicked.connect(self.storeROI)
        self.same_roi_time.stateChanged.connect(self.toggleSameROItime)

        self.load_files.clicked.connect(self.load)
                                        
        self.rois = []

        # Pointer to the current plot windows for placing ROI
        self.plt = plt
        
        # Pointer to current list of images for computing ROI
        self.images = images

        print
        print self.images
        print

        # Pointer to the current list of events
        self.event = event # what is this variable?
        
        # Erez, July-21, 2016
        self.run = int(run.text())
        self.subrun = int(subrun.text())
        self.event_num = int(event_num.text())
        self.captured_rse_map = {}
        #print self.run, self.subrun, self.event_num
        # --------------------
        

        # The iomanager
        self.in_iom = None
        self.ou_iom = None
        
        self.user_rois = {}
        self.user_rois_larcv = {}     # stores LArCV ROIs
        self.user_vertices_larcv = {} # stores list of Vertex2D tuples per event
        self.user_rois_src_rse = {} # stores run,subrun,event
        self.user_rois_previous_rse = [] # tracks rse that already have


        # TMW
        # imported ROI: provided by load files
        self.imported_larcv_rois = {} # dictionary with key of (run,subrun,event) and value of larcv::ROI
        self.imported_rse_dict = {} # dictionary with key of (run,subrun,event) and index

        # set state of roi behavior
        self.toggleSameROItime()

        self.checked_planes = []

        # ugly, but vertex setting buttons
        self.frame_vertex = QtGui.QFrame()
        self.frame_vertex_layout = QtGui.QGridLayout()
        self.set_uvertex = QtGui.QPushButton("set U vertex")
        self.set_vvertex = QtGui.QPushButton("set V vertex")
        self.set_yvertex = QtGui.QPushButton("set Y vertex")
        self.frame_vertex_layout.addWidget( self.set_uvertex, 0, 0 )
        self.frame_vertex_layout.addWidget( self.set_vvertex, 0, 1 )
        self.frame_vertex_layout.addWidget( self.set_yvertex, 0, 2 )
        self.frame_vertex.setLayout( self.frame_vertex_layout )
        self.frame_vertex.setLineWidth(2)
        self.frame_vertex.setFrameShape( QtGui.QFrame.Box )
        self.set_uvertex.clicked.connect( self.enableUCrossHairs )
        self.set_vvertex.clicked.connect( self.enableVCrossHairs )
        self.set_yvertex.clicked.connect( self.enableYCrossHairs )
        self.crosshairs = None # going to be a list of cross hairs
        self.plt.scene().sigMouseClicked.connect( self.mouseClicked )

        # pixel labeling tools/variables
        # enable label mode to set label
        self.labeltools = LabelingTool()
        
    def setImageAndPlotWidgets( self, plotitem, imageitem ):
        self.imi = imageitem
        self.plt = plotitem
        self.labeltools.setWidgets( self.plt, self.imi )

    def setImages( self, event_key, images ):
        """ parent widget of the ROI tool sends in new event and images through this function. """
        print "event index",event_key[0]
        self.run = event_key[1]
        self.subrun = event_key[2]
        self.event_num = event_key[3]
        print self.run,self.subrun,self.event_num
        self.images = images
        #print self.images.img_v
        #for img in self.images.img_v:
        #    print img.shape
        self.labeltools.setImage( event_key[0], self.images )
        
    def storeROI(self):
        """ responsible for outputing data used by the ROI tool.
            this includes:
               - ROI
               - Vertices
               - Pixel Labels
            bad: hardcoding producer names
        """

        if self.ou_iom is None:
            "Please load a file first please!!!!"
            return

        # get the maximum key
        max_ = 0

        for event, rois in self.user_rois.iteritems():
            if int(event) > max_: max_ = event
            print event, max_
            
        max_+=1

        for event in xrange(max_):

            if not event in self.captured_rse_map.keys():
                continue

            if len(self.labeltools.stored_labels)>0:
                label_array  = self.ou_iom.get_data(larcv.kProductPixel2D,self.output_prod)

            # event == TTree entry in the image file so I place that in the event number here.

            # STORE ROIs
            
            # If this event doesn't have an ROI, save a blank and continue
            if event not in self.user_rois.keys():
                if event in self.user_rois_src_rse and self.save_roi_RSE.isChecked():
                    rse = self.user_rois_src_rse[event]
                    self.ou_iom.set_id( rse[0], rse[1], rse[2] )
                else:
                    self.ou_iom.set_id(1,0,event)

            elif len(self.user_rois[event]) == 0:
                # User accidentally hit capture ROIs when no ROI drawn, save a blank and continue
                if event in self.user_rois_src_rse and self.save_roi_RSE.isChecked():
                    rse = self.user_rois_src_rse[event]
                    self.ou_iom.set_id( rse[0], rse[1], rse[2] )
                else:
                    self.ou_iom.set_id(1,0,event)

            # It's a fine ROI
            # if event has a stored run, subrun, event. put it in here
            elif event in self.user_rois_src_rse and self.save_roi_RSE.isChecked():
                rse = self.user_rois_src_rse[event]
                print "storing ROIs for ",rse
                self.ou_iom.set_id( rse[0], rse[1], rse[2] )
            # no RSE, put a 1 in the subrun to indicate one exists
            elif event not in self.user_rois_src_rse or not self.save_roi_RSE.isChecked():
                self.ou_iom.set_id(1,1,event)

            # There is ROI so lets append the larcv converted ROIs and put them into the ROOT file
            if event in self.user_rois_larcv and len(self.user_rois_larcv[event])>0:
                roiarray = self.ou_iom.get_data(larcv.kProductROI,self.output_prod)
                for larcv_roi in self.user_rois_larcv[event]:
                    roiarray.Append(larcv_roi)

            # There are vertices, too
            if event in self.user_vertices_larcv and len(self.user_vertices_larcv[event])>0:
                vertex_array = self.ou_iom.get_data(larcv.kProductPixel2D,'vertex_%s' % self.output_prod)
                for larcv_vertex2d in self.user_vertices_larcv[event]:
                    if larcv_vertex2d != (None,None,None):
                        print "storing ",larcv_vertex2d
                        for p,vert in enumerate( larcv_vertex2d ):
                            vertex_array.Append( p, vert )

            # Are there labeling images?
            if event in self.labeltools.stored_labels:
                print "storing labels for event=",event,
                pixelclusters = self.labelimg2pixelcluster( self.labeltools.stored_labels[event] )
                for p,pixelcluster in enumerate( pixelclusters ):
                    print " plane=",p,":",pixelcluster.size()," ",
                    label_array.Append( p, pixelcluster )
            elif event not in self.labeltools.stored_labels and len(self.labeltools.stored_labels)>0:
                print "inserting empty pixelclusters"
                # make empty pixelclusters
                for p in xrange(0,3):
                    pc = larcv.Pixel2DCluster()
                    label_array.Append( p, pc )
                
            # Save them to the tree
            out_rse = self.captured_rse_map[event]
            print 'Saving RSE =',out_rse
            self.ou_iom.set_id(out_rse[0],out_rse[1],out_rse[2])
            self.ou_iom.save_entry()

        # Put it on the disk
        self.ou_iom.finalize()
        
    def load(self):
        
        input_  = str(self.input_roi.text())
        output_ = str(self.output_roi.text())
        
        self.input_prod  = str(self.input_roi_producer.text())
        self.output_prod = str(self.output_roi_producer.text())

        # No ROOT file in the input, don't make a read iomanager
        if self.in_iom is None:
            if ".root" not in input_:
                input_ = []
                self.input_roi.setText("No input provided!")
                self.input_roi_producer.setText("give producer")
                self.input_prod = None
            else:
                if os.path.exists( input_ ):
                    self.in_iom = IOManager([input_],None,0)
                    self.in_iom.set_verbosity(2)

                    # load up rois
                    idx = 0
                    for in_entry in range(0,self.in_iom.get_n_entries()):
                        self.in_iom.read_entry(in_entry)
                        event_rois = self.in_iom.get_data( larcv.kProductROI, self.input_prod )
                        rse = ( event_rois.run(), event_rois.subrun(), event_rois.event() )
                        self.imported_rse_dict[rse] = idx
                        idx += 1
                    print "Number of events with ROIs: ",len(self.imported_rse_dict)
                else:
                    print "Could not find input file: ",input_
        else:
            print "input roi file already loaded."

        if self.ou_iom is not None:
            print "Already loaded output file."
            return

        # No ROOT file in the output, return and complain
        if ".root" not in output_:
            self.output_roi.setText("No valid output ROOT file!")
            self.output_roi_producer.setText("Give output ROI producer!")
            self.output_prod = None
        else:
            self.ou_iom = IOManager([],output_,1)
            self.ou_iom.set_verbosity(2)

    def captureROI(self):

        # Get the rois on screen
        self.user_rois[int(self.event.text())] = self.rois # not allowed to ``copy" qwidgets (w/ copy.deepcopy)
        self.captured_rse_map[int(self.event.text())] = (self.run,self.subrun,self.event_num)
        
        larcv_rois = [self.roi2larcv(roisg) for roisg in self.rois]
        larcv_vertices = [self.roi2vertex2d(roisg) for roisg in self.rois]

        # save by index
        self.user_rois_larcv[int(self.event.text())] = larcv_rois # not allowed to copy qwidgets
        self.user_vertices_larcv[int(self.event.text())] = larcv_vertices
        # save event info if we have it
        if self.dm is not None:
            self.user_rois_src_rse[int(self.event.text())] = ( self.dm.run, self.dm.subrun, self.dm.event )
            
        print "--- Captured ROIs in Memory ---"
        print self.user_rois
        print self.user_rois_larcv
        print self.user_rois_src_rse
        print "-------------------------------"

    def makeEmptyROI(self):
        print "Making a blank entry intentionally"""
        event = int(self.event.text())
        self.user_rois[event] = []
        self.user_rois_larcv[event] = None
        self.user_rois_src_rse[event] = ( self.dm.run, self.dm.subrun, self.dm.event )
        print "--- Captured ROIs in Memory ---"
        print self.user_rois
        print self.user_rois_larcv
        print self.user_rois_src_rse
        print "-------------------------------"
        
        
    def addROI(self) :

        ww = 50
        hh = 50
        allow_resize=True
        
        if self.fixed_roi_box.isChecked():
            ww = int(self.fixed_w.text())
            hh = int(self.fixed_h.text())
            allow_resize=False

            
        if self.getCheckedPlanes() == False:
            return
        
        coords = [ [0,0,ww,hh] for _ in xrange(3)]
        
        roisg = ROISliderGroup(coords,self.checked_planes,3,store.colors,allow_resize, func_setlabel=self.setROILabel )

        self.rois.append(roisg)

        for roi in roisg.rois:
            self.plt.addItem(roi)

        self.toggleSameROItime()

        if not roisg.fix_vertex_to_bb:
            print "draw makers",roisg.vertexplot
            self.plt.addItem( roisg.vertexplot )

    def removeROI(self) :
        for roi in self.rois[-1].rois:
            self.plt.removeItem(roi)
            
        self.rois = self.rois[:-1]

    def clearROI(self) :

        while len(self.rois) > 0:
            for roi in self.rois[-1].rois:
                self.plt.removeItem(roi)
                
            self.rois = self.rois[:-1]

    def resetROI(self):
        self.clearROI()
        self.user_rois = {}
        self.user_rois_larcv = {}

    # File Frame Widget
    def _makeFileFrame(self):
        if hasattr(self,'fileframe'):
            return # already made

        self.fileframe  = QtGui.QFrame()
        self.filelayout = QtGui.QGridLayout()
        self.input_label = QtGui.QLabel("Input:")
        self.input_label.setFixedWidth(45)
        self.output_label = QtGui.QLabel("Output:")
        self.output_label.setFixedWidth(45)
        self.input_prod_label = QtGui.QLabel("Producer:")
        self.output_prod_label = QtGui.QLabel("Producer:")
        self.input_prod_label.setFixedWidth(60)
        self.output_prod_label.setFixedWidth(60)
        
        self.input_roi = QtGui.QLineEdit("ROI filename")
        self.input_roi_producer  = QtGui.QLineEdit("ROI producer")
        self.input_prod = None
        
        self.output_roi = QtGui.QLineEdit("ROI filename")
        self.output_roi_producer = QtGui.QLineEdit("ROI producer")
        self.output_prod = None

        self.load_files = QtGui.QPushButton("Load Files")

        # in/out labels for file and producer
        self.filelayout.addWidget( self.input_label, 0, 0 )
        self.filelayout.addWidget( self.input_roi, 0, 1 )
        self.filelayout.addWidget( self.input_prod_label, 0, 2 )
        self.filelayout.addWidget( self.input_roi_producer, 0, 3 )

        self.filelayout.addWidget( self.output_label, 1, 0 )
        self.filelayout.addWidget( self.output_roi, 1, 1 )
        self.filelayout.addWidget( self.output_prod_label, 1, 2 )
        self.filelayout.addWidget( self.output_roi_producer, 1, 3 )

        self.filelayout.addWidget( self.load_files, 2, 0, 1, 4 )
        self.fileframe.setLayout( self.filelayout )
        self.fileframe.setLineWidth(2)
        self.fileframe.setFrameShape( QtGui.QFrame.Box )
        return
        
    def reloadROI(self):
        print "reload ROI"
        self.clearROI()
        print "post-clear num rois: ",len(self.rois)

        event = int(self.event.text())
        rse = ( self.dm.run, self.dm.subrun, self.dm.event  )

        self.run = rse[0]
        self.subrun = rse[1]
        self.event_num = rse[2]

        # If no user ROIs, look through imported ROIs
        if event not in self.user_rois.keys():

            if rse in self.imported_rse_dict:
                print "rse in imported list ",rse
                idx = self.imported_rse_dict[rse]
                self.in_iom.read_entry(idx)  
                roiarray = self.in_iom.get_data(larcv.kProductROI,self.input_prod)
                self.user_rois_larcv[event] = [roi for roi in roiarray.ROIArray()]
                print "reloading ",self.user_rois_larcv[event]," from file"
                self.user_rois[event] = self.larcv2roi(self.user_rois_larcv[event])  
            else:
                print rse," not in user_rois nor in imported dict."
                return
            
        # set to active rois
        self.rois = self.user_rois[event]
        print "active rois: ",len(self.rois)

        for roisg in self.rois:
            # add bboxes for the planes
            for roi in roisg.rois:
                #print type(roi),roi
                self.plt.addItem(roi)
            # add vertex markers
            if not roisg.fix_vertex_to_bb:
                print "draw makers",roisg.vertexplot
                self.plt.addItem( roisg.vertexplot )
    
    # add widgets to self and return 
    def grid(self, enable):

        if enable == True:
            self.enabled = True
            
            self.addWidget(self.title, 0, 0)

            # Inputs for loading input and output ROI file and producers
            self.addWidget(self.fileframe, 1, 0, 3, 4)
            
            self.addWidget(self.add_roi, 1, 4)
            self.addWidget(self.remove_roi, 2, 4)
            self.addWidget(self.capture_roi, 3, 4)

            self.addWidget(self.clear_roi, 1, 5)
            self.addWidget(self.reset_roi, 2, 5)
            self.addWidget(self.store_roi, 3, 5)


            self.addWidget(self.roi_p1,1,6)
            self.addWidget(self.roi_p2,2,6)
            self.addWidget(self.roi_p3,3,6)

            self.addWidget(self.fixed_w_label,1,7)
            self.addWidget(self.fixed_h_label,2,7)

            self.addWidget(self.empty_roi, 3, 7)

            self.addWidget(self.fixed_roi_box,0,8)
            self.addWidget(self.fixed_w,1,8)
            self.addWidget(self.fixed_h,2,8)

            self.addWidget(self.same_roi_time,0,7)
            self.addWidget(self.save_roi_RSE, 0,6)


            # positions
            self.addWidget(self.uplane_pos, 0, 1, 1, 4 ) # i don't know where to put this. maybe add text to image? remove it?

            # vertex buttons
            self.addWidget( self.frame_vertex, 4, 0, 1, 3 )

            # Label mode buttons
            #self.addWidget( self.labeltools.getframe(), 4, 3, 1, self.labeltools.getframewidth() )
            self.addWidget( self.labeltools.getframe(), 4, 3, 1, 6 )

        else:

            self.enabled = False
            
            for i in reversed(range(self.count())):
                self.itemAt(i).widget().setParent(None)


        return self

    def roi2larcv(self,bboxes):

        #store all ROIs
        
        larcv_roi = larcv.ROI()

        for ix,bbox in enumerate(bboxes.rois):
            size = bbox.size()
            pos = bbox.pos()

            #print "ix",ix,"size",size,"pos",pos

            width,height,row_count,col_count,origin_x,origin_y = self.roi2imgcord(self.images.imgs[ix].meta(),size,pos)
            
            bbox_meta = larcv.ImageMeta(width,height,
                                        row_count,col_count,
                                        origin_x,origin_y,bbox.plane)
            

            larcv_roi.AppendBB(bbox_meta)

        return larcv_roi

    def roi2vertex2d(self,bboxes):
        """ extract vertex information if any, and put it into a vertex 2D.
        inputs
        ------
        bboxes: instance of ROISliderGroup

        outputs
        -------
        tuple of pixel2D instances. One for each plane.

        if no vertex asigned to ROI, return (None,None,None)
        """
        if not bboxes.fix_vertex_to_bb:
            return (None,None,None)

        return ( larcv.Pixel2D( int(bboxes.vertices[0]["pos"][0]), int(bboxes.vertices[0]["pos"][1]) ),
                 larcv.Pixel2D( int(bboxes.vertices[1]["pos"][0]), int(bboxes.vertices[1]["pos"][1]) ),
                 larcv.Pixel2D( int(bboxes.vertices[2]["pos"][0]), int(bboxes.vertices[2]["pos"][1]) ) )
                                
    def labelimg2pixelcluster(self,labelimg):
        """ extract labels from labelingtool's labelmat. It's just a numpy array. """
        if not hasattr(self,'translator'):
            self.translator = {self.labeltools.labels.index("electron"):larcv.kROIEminus,
                               self.labeltools.labels.index("muon"):larcv.kROIMuminus,
                               self.labeltools.labels.index("pion"):larcv.kROIPiminus,
                               self.labeltools.labels.index("proton"):larcv.kROIProton,
                               self.labeltools.labels.index("gamma"):larcv.kROIGamma}
        
        clusters = {}
        for iplane in xrange(0,3):
            clusters[iplane] = larcv.Pixel2DCluster()
            if iplane >= len(labelimg):
                continue
            labelmat = labelimg[iplane]
            idx_bg = self.labeltools.labels.index("background")
            idxlabels = (labelmat != idx_bg).nonzero()
            ivertmax = self.images.img_v[iplane].shape[1]-1
            for ivert in xrange(0,len(idxlabels[0])):
                vert = larcv.Pixel2D( idxlabels[0][ivert], ivertmax - idxlabels[1][ivert] )
                label = labelmat[ idxlabels[0][ivert], idxlabels[1][ivert] ]
                print "storing (",vert.X(),",",vert.Y(),") label=", self.labeltools.labels[label]
                vert.Intensity( float(self.translator[ label ] ) )
                clusters[iplane] += vert

        return ( clusters[0], clusters[1], clusters[2] )
            

    def roi2imgcord(self,imm,size,pos):

        x = pos[0]
        y = pos[1]

        dw_i = imm.cols() / (imm.max_x() - imm.min_x())
        dh_i = imm.rows() / (imm.max_y() - imm.min_y())

        x /= dw_i
        y /= dh_i

        # the origin
        origin_x = x + imm.min_x()
        origin_y = y + imm.min_y()
        
        # w_b is width of a rectangle in original unit
        w_b = size[0]
        h_b = size[1]

        # the width
        width  = w_b / dw_i
        height = h_b / dh_i

        # for now...
        row_count = 0
        col_count = 0

        # vic isn't sure why this is needed
        origin_y += height

        #print "roi2img ROI: ",(width,height,row_count,col_count,origin_x,origin_y)
        return (width,height,row_count,col_count,origin_x,origin_y)

    def img2roicord(self,imm,bbox):
        
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

        #print "img2roi ROI: ",(x*dw_i,  y*dh_i,  w_b*dw_i,  h_b*dh_i)
        return (x*dw_i,  y*dh_i,  w_b*dw_i,  h_b*dh_i)

    
    def larcv2roi(self,rois):

        converted_rois = []

        for roi in rois:

            coords = [None for _ in xrange(3)]

            bbs = roi.BB()

            planes = []
            
            for ix,bbox in enumerate(bbs):

                pl = int(bbox.plane())
                
                coord = self.img2roicord(self.images.imgs[ix].meta(),bbox)
                
                coords[pl] = coord
                planes.append(pl)
                
            roisg = ROISliderGroup(coords,planes,len(coords),store.colors)
            if self.same_roi_time.isChecked():
                roisg.useSameTimes()
            else:
                roisg.useDifferentTimes()
            
            converted_rois.append(roisg)
            

        return converted_rois
        
        
    def getCheckedPlanes(self):

        self.checked_planes = []
            
        if self.roi_p1.isChecked() == True:
            self.checked_planes.append(0)

        if self.roi_p2.isChecked() == True:
            self.checked_planes.append(1)

        if self.roi_p3.isChecked() == True:
            self.checked_planes.append(2)
            
        if len(self.checked_planes) == 0:
            return False

        return True
        
    
    def toggleSameROItime(self):
        print "same time rois: ",self.same_roi_time.isChecked()
        for roiset in self.rois:
            if self.same_roi_time.isChecked():
                roiset.useSameTimes()
            else:
                roiset.useDifferentTimes()
                
    def setROILabel(self,rois):
        if len(rois)>=3:
            try:
                text = ""
                for plane,label,roi in [("U",self.uplane_pos,rois[0]),("V",self.vplane_pos,rois[1]),("Y",self.yplane_pos,rois[2])]:
                    size = roi.size()
                    pos  = roi.pos()
                    width,height,row_count,col_count,origin_x,origin_y = self.roi2imgcord(self.images.imgs[-1].meta(),size,pos)
                    #label.setText("%s: [%.1f,%.1f],[%.1f,%.1f]"%(plane,origin_x,origin_x+width,origin_y-height,origin_y))
                    text += "%s: [%.1f,%.1f],[%.1f,%.1f]"%(plane,origin_x,origin_x+width,origin_y-height,origin_y)+"  "
                self.uplane_pos.setText(text)
            except:
                pass

    def enablePlaneCrossHairs(self,plane):
        print "Enable plane=",plane," cross hairs for vertex selection."
        print "image pointer in roitool: ",self.plt,self.crosshairs
        #if self.crosshairs is None and self.plt is not None and self.imi is not None:
        if self.crosshairs is None and self.plt is not None:
            print "create cross hairs on if plt and img exists"
            self.crosshairs = [ CrossHairs(self,self.plt,self.imi,x) for x in range(0,3) ]
        if self.crosshairs is not None:
            print "add cross hairs to image"
            self.crosshairs[plane].active = True
            self.plt.addItem( self.crosshairs[plane].vLine,  ignoreBounds=True )
            self.plt.addItem( self.crosshairs[plane].hLine,  ignoreBounds=True )
            #self.plt.addItem( self.crosshairs[plane].label )
            # disable other vertex finders
            self.set_uvertex.setEnabled(False)
            self.set_vvertex.setEnabled(False)
            self.set_yvertex.setEnabled(False)

    def enableUCrossHairs(self):
        self.enablePlaneCrossHairs(0)
    def enableVCrossHairs(self):
        self.enablePlaneCrossHairs(1)
    def enableYCrossHairs(self):
        self.enablePlaneCrossHairs(2)
    
    def mouseClicked(self,evt):
        if self.crosshairs is not None:
            for ch in self.crosshairs:
                didsomething = ch.mouseClicked(evt)
                if didsomething:
                    self.set_uvertex.setEnabled(True)
                    self.set_vvertex.setEnabled(True)
                    self.set_yvertex.setEnabled(True)
                    for roig in self.rois:
                        roig.setVertex( ch.plane, ch.getvertex() )
                        if roig.vertexplot not in self.plt.listDataItems():
                            self.plt.addItem( roig.vertexplot )
        
                        
