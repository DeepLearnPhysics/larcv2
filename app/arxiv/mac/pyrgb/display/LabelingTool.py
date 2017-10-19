import os,sys
import numpy as np
from .. import pg
from .. import QtGui
from ..lib.roislider import ROISlider

class LabelingTool:
    """ Class holding labeling functions.
    This class is a state machine that modifies a ImageItem stored in a PlotItem.
    
    states:  
    uninitiated: not yet worked with an image.
    drawing: swaps ImageItem from PlotItem. The grabbed image is flattened, 
             and the user is allowed to label on top of it.
    display: swaps ImageItem with an image it creates from the labeling that has been done.
    inactive: means that the original imageitem has been returned once drawing or displaying over
    """
    
    # state enum
    kUNINIT = 0
    kDRAWING = 1
    kDISPLAY = 2
    kBOXMODE = 3
    kINACTIVE = 4
    statenames = { kUNINIT:"Unint",
                   kDRAWING:"Labeling Mode",
                   kDISPLAY:"Display Mode",
                   kBOXMODE:"Box Mode",
                   kINACTIVE:"Inactive" }
    
    #def __init__(self,labels,color_codes):
    def __init__(self):
        """
        labels: a list of strings. index of labels used in code.
        color_codes: dictionary mapping label names to color
        """
        self.state = LabelingTool.kUNINIT
        self._makeFrame()
        if "background" not in self.labels:
            raise ValueError("Labels in labeling tool must have 'background' label!")
        # critical variables
        self.plotitem = None
        self.imageitem = None
        self.images = None

        self.stored_labels = {} # eventkey, dict of label matrices, gross

    def setWidgets( self, plotitem, imageitem ):
        self.plotitem = plotitem
        self.imageitem = imageitem
        # revert the state to uninit: we have to assume the image has changes as well
        self.reset()

    def setImage( self, eventkey, image ):
        self.images  = image
        self.nplanes = len(self.images.imgs) # attempt to not be hardcoded, but probably fragile
        # now initilize label containers
        if self.state==LabelingTool.kDRAWING:
            self.restoreImage()
        self._initialize( eventkey, self.images )
        self.setButtonStates()

    def goIntoLabelingMode(self, label, plotitem, imageitem, src_images ):
        """
        switches state from inactive to drawing
        plotitem: the plot item it is suppose to manipulate
        imageitem: the image item in the plot it is suppose to manipulate
        src_images: plot image object (see lib/image_types/plotimage.py)

        for the drawing mode, we can only use black-to-white images as far as I can tell.
        so we convert to grey-scale, marking previously labeled pixels (same as current) with white.
        confusing, but what can we do for now?

        """
            
        if self.shape()!=src_images.plot_mat.shape[:2]:
            #raise ValueError("Image shape passed into LabelingTool is not the same shape as before")
            print "Warning: Image shape passed into LabelingTool is not the same shape as before"
            self._initialize( None, src_images, reinit=True )

        if label not in self.labels:
            raise ValueError("Label not recognized")

        # set the state
        self.state = LabelingTool.kDRAWING
        self.plane = self._menu_labelplane.currentIndex()

        # first keep pointer to data from the image item
        self.orig_img = src_images
        
        # for pixels we've already labeled as 'label', we put them into the image
        # we use a trick. the reduce color pixels by 10
        # this way labeled values can be distinguished later
        working = np.copy( self.orig_img.plot_mat )
        fmax = np.max( working )*1.1
        self.current_idx_label = self.labels.index( label )
        if label!="background":
            working[ self.labelmat[self.plane]==self.current_idx_label ] = fmax
        else:
            working[ self.labelmat[self.plane]!=self.current_idx_label ] = fmax
        working /= fmax # normalize

        # now set the image
        imageitem.setImage( image=working, levels=[[0.0,1.0],[0.0,1.0],[0.0,1.0]] )

        # and turn on the drawing state:
        #kern = np.array( [[[ self.colors[label][0], self.colors[label][1], self.colors[label][2] ]]] )
        kernwidth = self._menu_kernsize.currentIndex()
        if kernwidth<3:
            kernwidth = np.power( 3, kernwidth )
            self.setKernel( kernwidth, label, imageitem )

        self.current_img = imageitem.image


    def _initialize(self, eventkey, src_images, reinit=False ):
        """ we setup the numpy array to store the labels now that we know image shape we are dealing with."""
        labelmatshape = src_images.plot_mat.shape[:2]
        if eventkey is None:
            eventkey = self.current_eventkey
        if eventkey not in self.stored_labels or reinit==True:
            self.stored_labels[eventkey] = {}
            for plane in xrange(0,self.nplanes):
                self.stored_labels[eventkey][plane] = np.ones( labelmatshape, dtype=np.int )*self.labels.index("background")
        self.current_eventkey = eventkey
        self.labelmat = self.stored_labels[eventkey]

        # we are inactive. ready to draw.
        self.state = LabelingTool.kINACTIVE

    def reset(self):
        """ reset the state. clear current label container and reset state to uninitiated. """
        self.state = LabelingTool.kUNINIT

    def shape( self ):
        if self.state==LabelingTool.kUNINIT:
            return None
        return self.labelmat[0].shape

    def drawLabeling(self,plotitem,imageitem,src_images):
        """ put labeling colors onto current image """

        # set state to display
        self.state=LabelingTool.kDISPLAY
        self.orig_img = src_images
        self.imageitem = imageitem
        self.plotitem  = plotitem
        working = np.copy( self.orig_img.plot_mat )
        fmax = np.max( working )*1.1
        working /= fmax # normalize

        drawplanes = []
        if self._menu_labelplane.currentIndex()!=3:
            drawplanes.append( self._menu_labelplane.currentIndex() )
        else:
            drawplanes = [0,1,2]

        for plane in drawplanes:
            for idx,label in enumerate(self.labels):
                if label=="background":
                    continue
                color = np.array( self.colors[label] )/255.0
                working[ self.labelmat[plane]==idx ] = color

        self.imageitem.setImage( image=working, levels=[[0.0,1.0],[0.0,1.0],[0.0,1.0]] )
        self.setKernel( 0, None, self.imageitem ) # turn off draw mode

    def saveLabeling(self):
        """ take current image, find labeled pixels, store them in self.labelmat """
        if self.state not in [LabelingTool.kDRAWING,LabelingTool.kBOXMODE]:
            return
        # use current img
        self.plane = self._menu_labelplane.currentIndex()
        print "Saving labels event=",self.current_eventkey," plane=",self.plane," label=",self.labels[self.current_idx_label]," ]"
        marked = (self.current_img >= 1.0).all(axis=2)
        print marked.shape
        print np.count_nonzero( marked )
        self.labelmat[self.plane][ marked ] = self.current_idx_label
        # debug: print indices of marked pixels
        nz = np.nonzero( marked )
        print nz
        for x in range(0,len(nz[0])):
            print nz[0][x], nz[1][x]
        return
    
    def undo(self):
        """ restore current image, losing latest labeling """
        if self.state==LabelingTool.kBOXMODE:
            print "Move from BOXMODE TO INACIVE."
            self._menu_kernsize.setCurrentIndex(1)
            if self.plotitem is not None:
                self.state = LabelingTool.kINACTIVE
                self.plotitem.removeItem( self.labelbox )
                self.setButtonStates()
                self.restoreImage()
                return
            else:
                self.state = LabelingTool.kUNINIT
                self.setButtonStates()
                return

        working = np.copy( self.orig_img.plot_mat )
        fmax = np.max( working )*1.1
        if self.labels[self.current_idx_label]!="background":
            working[ self.labelmat==self.current_idx_label ] = fmax
        else:
            working[ self.labelmat!=self.current_idx_label ] = fmax
        working /= fmax # normalize

        # now set the image
        self.imageitem.setImage( image=working, levels=[[0.0,1.0],[0.0,1.0],[0.0,1.0]] )
        self.current_img = self.imageitem.image
        
    def switchLabel(self, newlabel):
        """ switch within drawing mode, the label used """
        if self.state!=LabelingTool.kDRAWING:
            return

        self.saveLabeling()
        self.current_idx_label = self.labels.index( newlabel )
        kernwidth = self._menu_kernsize.currentIndex()
        if kernwidth<3:
            kernwidth = np.power(3,kernwidth)
            self.setKernel( kernwidth, newlabel, self.imageitem )
        self.undo() # redraws (but now with new label highlighted using current_idx_label)
        

    def restoreImage(self):
        """ deactivate label mode """
        # restore image
        working = np.copy( self.orig_img.plot_mat )
        self.imageitem.setImage( image=working, autoLevels=True )
        self.setKernel( 0, None, self.imageitem ) # deactivate kernel
        return


    def _makeFrame(self):
        """ makes all the widgets involved in the labeling tools. parent widgets will use these buttons."""
        # This has hardcoded labels and colors
        # Maybe inheritance can allow this to be customized, extended? Worry about it later if we need it.
        if hasattr(self,'frame'):
            return self.frame
        self.labels = ["electron","muon","proton","pion","gamma","background"]
        self.colors    = {"electron":(255,128,0),
                          "muon":(0,255,255),
                          "proton":(255,0,255),
                          "pion":(0,128,255),
                          "gamma":(255,255,0),
                          "background":(0,0,0)}

        self._button_toggleLabelMode = QtGui.QPushButton("Enable Label Tool")
        self._button_toggleLabelMode.clicked.connect( self.toggleLabelMode )
        self._button_savelabel = QtGui.QPushButton("Save Labeling")
        self._button_savelabel.clicked.connect( self.saveLabels )
        self._button_showlabel = QtGui.QPushButton("Show Labels")
        self._button_showlabel.clicked.connect( self.displayLabels )
        self._button_undolabel = QtGui.QPushButton("Undo")
        self._button_undolabel.clicked.connect( self.undo )
        self._button_savelabel.setEnabled(False)
        self._button_showlabel.setEnabled(False)
        self._button_undolabel.setEnabled(False)
        self._menu_setLabelPID = QtGui.QComboBox()
        for idx,label in enumerate(self.labels):
            self._menu_setLabelPID.insertItem( idx, label )
        self._menu_setLabelPID.setCurrentIndex( 0 )
        self._label_mode = False
        self._currentlabel = self._menu_setLabelPID.currentIndex()
        self._menu_setLabelPID.currentIndexChanged.connect( self.setLabelIndex )

        self._menu_labelplane = QtGui.QComboBox()
        for idx,plane in enumerate(["U","V","Y"]):
            self._menu_labelplane.insertItem( idx, plane )
        self._menu_labelplane.setCurrentIndex( 0 )
        self._menu_labelplane.currentIndexChanged.connect( self.setLabelPlane )
        
        self._label_state = QtGui.QLabel("state: %s"%(LabelingTool.statenames[self.state]))

        self._menu_kernsize = QtGui.QComboBox()
        self.kern_options = ["1","3","9","box"]
        for n,opt in enumerate(self.kern_options):
            self._menu_kernsize.insertItem( n, opt )
        self._menu_kernsize.setCurrentIndex(1)
        self._menu_kernsize.currentIndexChanged.connect( self.setKernSizeType )

        self._label_thresh = QtGui.QLabel("Thresh:")
        self._label_thresh.setFixedWidth(60)
        self._input_thresh = QtGui.QLineEdit( "0.1" )
        self._input_thresh.setFixedWidth(40)

        self.frame = QtGui.QFrame()
        self._labellayout = QtGui.QGridLayout()
        self._labellayout.addWidget( self._button_toggleLabelMode, 0, 0 )
        self._labellayout.addWidget( self._menu_setLabelPID, 0, 1 )
        self._labellayout.addWidget( self._button_showlabel, 0, 2 )
        self._labellayout.addWidget( self._button_savelabel, 0, 3 )
        self._labellayout.addWidget( self._button_undolabel, 0, 4 )
        self._labellayout.addWidget( self._menu_labelplane,  0, 5 )
        self._labellayout.addWidget( self._menu_kernsize,    0, 6 )
        self._labellayout.addWidget( self._label_thresh,     0, 7 )
        self._labellayout.addWidget( self._input_thresh,     0, 8 )
        self._labellayout.addWidget( self._label_state,  0, 9 )
        self.frame.setLayout( self._labellayout )
        self.frame.setLineWidth(2)
        self.frame.setFrameShape( QtGui.QFrame.Box )        
        self.framewidth = 10

    def getframe(self):
        return self.frame
    
    def getframewidth(self):
        return self.framewidth

    def setKernel(self,kernwidth,label,imageitem):
        if kernwidth is None or kernwidth==0:
            print "get imageitem out of drawing mode"
            imageitem.setDrawKernel( kernel=None )

        if label!="background":
            kern = np.ones( (kernwidth,kernwidth,3) )
            imageitem.setDrawKernel( kernel=kern, mask=kern, center=(0,0), mode='add' )
        else:
            kern = np.zeros( (kernwidth,kernwidth,3) )
            imageitem.setDrawKernel( kernel=kern, mask=kern, center=(0,0), mode='set' )

    # --------------------------------------------------------------------------------------------
    # button functions

    def toggleLabelMode(self):
        """ function called when label mode button is pressed/released.
            actions dependent on state
            [UNINIT] Do nothing
            [INACTIVE] Start drawing mode
            [DISPLAY] Return to drawing mode
            [DRAWING] Set to inactive
        """
        if self.imageitem is None or self.plotitem is None:
            print "Warning: No image nor plot loaded yet for labeling. Doing nothing."
            return

        if self.state in [LabelingTool.kUNINIT]:
            print "Labeling tool in unitialized state. do nothing."
            return

        if self.state in [LabelingTool.kINACTIVE]:
            print "[Activating label mode]"
            self.state = LabelingTool.kDRAWING
            self.setButtonStates()
            self.goIntoLabelingMode( self.labels[self._menu_setLabelPID.currentIndex()], self.plotitem, self.imageitem, self.images )
        elif self.state in [LabelingTool.kDRAWING,LabelingTool.kDISPLAY]:
            print "[Deactivating label mode]"
            self.saveLabeling()
            self.restoreImage()
            self.state = LabelingTool.kINACTIVE
            self.setButtonStates()

        if self.state in [LabelingTool.kBOXMODE]:
            print "[label pixels using the box!]"
            self.labelUsingBox()
            

    def setLabelIndex(self,currentindex):
        """ called when label particle combo box changes """
        if self._currentlabel == self._menu_setLabelPID.currentIndex():
            # do nothing
            return
        self._currentlabel = self.labels[self._menu_setLabelPID.currentIndex()]
        if self.state==LabelingTool.kDRAWING:
            self.switchLabel( self._currentlabel )
        elif self.state==LabelingTool.kDISPLAY:
            self.drawLabeling( self.plotitem, self.imageitem, self.orig_img )

    def labelUsingBox( self ):
        """ called when _button_toggleLabelMode is hit while in boxmode state """
        print self.labelbox.pos(),self.labelbox.size()
        
        #width = self.labelbox.size()[0]/self.images.imgs[0].meta().pixel_width()
        #height = self.labelbox.size()[1]/self.images.imgs[0].meta().pixel_height()
        width  = self.labelbox.size()[0]
        height = self.labelbox.size()[1]

        #x1 = int(self.labelbox.pos()[0]-0.5*width)
        #x2 = int(self.labelbox.pos()[0]+0.5*width)
        #y1 = int(self.labelbox.pos()[1]-0.5*height)
        #y2 = int(self.labelbox.pos()[1]+0.5*height)
        x1 = int(self.labelbox.pos()[0])
        y1 = int(self.labelbox.pos()[1])
        x2 = x1 + width
        y2 = y1 + height
        
        self.plane = self._menu_labelplane.currentIndex()
        thresh = float( self._input_thresh.text() )
        label  = self.labels[self.current_idx_label]
        print "Labeing within box:  plane=",self.plane," label=",self.labels[self.current_idx_label]," thresh>=",thresh," w=",width," h=",height

        working = np.copy( self.orig_img.plot_mat )
        fmax = np.max( working )*1.1
        self.current_idx_label = self.labels.index( label )
        if label!="background":
            working[ self.labelmat[self.plane]==self.current_idx_label ] = fmax
        else:
            working[ self.labelmat[self.plane]!=self.current_idx_label ] = fmax
        working /= fmax # normalize
        
        matslice = working[x1:x2,y1:y2,:]
        print "slice: ",matslice.shape
        if label!="background":
            kern = np.ones( (matslice.shape[-1], ) )
            for ich in range(0,matslice.shape[-1]):
                matslice[:,:,ich][ matslice[:,:,self.plane]>thresh ] = 1.0
        else:
            for ich in range(0,matslice.shape[-1]):
                matslice[:,:,ich][ matslice[:,:,self.plane]>thresh ] = 0.0

        # now set the image
        self.imageitem.setImage( image=working, levels=[[0.0,1.0],[0.0,1.0],[0.0,1.0]] )
        self.current_img = self.imageitem.image

        return
        


    def displayLabels(self):
        """ called when show labels button pressed """
        # if transitioning from drawing state, save labeling
        if self.state==LabelingTool.kDRAWING:
            self.saveLabeling()
            self.state = LabelingTool.kDISPLAY
            self.drawLabeling( self.plotitem, self.imageitem, self.orig_img )
            self.setButtonStates()
        elif self.state==LabelingTool.kDISPLAY:
            self.state = LabelingTool.kDRAWING
            self.setButtonStates()
            self.goIntoLabelingMode( self.labels[self._menu_setLabelPID.currentIndex()], self.plotitem, self.imageitem, self.images )

    def saveLabels(self):
        """ called when save labels button hit """
        print "[put labels to memory]"
        self.saveLabeling()
        
    def setLabelPlane(self,plane):
        if self.state==self.kDISPLAY:
            # redraw with new plane
            self.drawLabeling( self.plotitem, self.imageitem, self.orig_img )
        elif self.state==self.kDRAWING:
            self.goIntoLabelingMode( self.labels[self._menu_setLabelPID.currentIndex()], self.plotitem, self.imageitem, self.images )

    def setKernSizeType(self,idx):
        """ function called when _menu_kernsize is changed. """
        #if self.state!=LabelingTool.kDRAWING:
        #    return

        kerntype = self._menu_kernsize.currentIndex()
        if kerntype<3 and self.state==LabelingTool.kDRAWING:
            # set drawing kernel size
            kernsize = np.power( 3, kerntype )
            self.setKernel( kernsize, self._currentlabel, self.imageitem )
        elif kerntype==3:
            print "[Start Box Labeling Mode]"
            self.state = self.kBOXMODE
            self.setButtonStates()
            # here we need to place box into the center for the user
            scenerect = self.plotitem.sceneBoundingRect()
            scenecenter = scenerect.center()
            mp = self.plotitem.vb.mapSceneToView(scenecenter)
            self.labelbox = pg.RectROI([mp.x(), mp.y()], [20, 20], pen=(0,9))
            self.plotitem.addItem( self.labelbox )
            self.setKernel(0,None,self.imageitem)
            print "placeing labeling box here: ",mp

    def setButtonStates(self):
        if self.state in [LabelingTool.kDRAWING]:
            self._button_toggleLabelMode.setText("Disable Label Tool")
            self._button_showlabel.setText("Show Labels") # got to labeling/drawing state
            self._button_savelabel.setEnabled(True)
            self._button_showlabel.setEnabled(True)
            self._button_undolabel.setEnabled(True)
            self._menu_kernsize.setEnabled(True)
            if self._menu_labelplane.currentIndex()==3:
                self._menu_labelplane.setCurrentIndex(0)
            self._menu_labelplane.removeItem(3) # remove the all plane option
        elif self.state in [LabelingTool.kDISPLAY]:
            self._button_toggleLabelMode.setText("Disable Label Mode") # go to inactive state, restore image
            self._button_showlabel.setText("Draw Labels") # got to labeling/drawing state
            self._button_savelabel.setEnabled(True)
            self._button_showlabel.setEnabled(True)
            self._button_undolabel.setEnabled(True)
            self._menu_labelplane.insertItem(3,"all") # remove the all plane option
            self._menu_kernsize.setEnabled(False)
        elif self.state in [LabelingTool.kINACTIVE,LabelingTool.kUNINIT]:
            self._button_toggleLabelMode.setText("Enable Label Tool")
            self._button_showlabel.setText("Show Labels")
            self._button_savelabel.setEnabled(False)
            self._button_showlabel.setEnabled(False)
            self._button_undolabel.setEnabled(False)
            self._menu_kernsize.setEnabled(False)
        elif self.state in [LabelingTool.kBOXMODE]:
            # limited options
            self._button_toggleLabelMode.setText("Label in Box")
            self._button_showlabel.setEnabled(False)
            self._button_savelabel.setEnabled(True)
            self._button_showlabel.setEnabled(False)
            self._button_undolabel.setEnabled(True)
            self._menu_kernsize.setEnabled(False)
        else:
            raise ValueError("Got into an unknown state.")
        self._label_state.setText("state: %s"%(LabelingTool.statenames[self.state]))
        
