from .. import pg
import numpy as np

class ROISlider(pg.ROI):

    def __init__(self,xx,yy,pen='w',allow_resize=True,plane=0): # need to look up kwargs

        #scaleSnap and translateSnap == the box can only scanle and slide to integer values
        super(ROISlider, self).__init__(xx,yy,scaleSnap=True,translateSnap=True,pen=pen)

        # which dimension is it?
        self.plane = plane

        # add the toggles to the edge of the ROI slider
        if allow_resize == True:
            #self.addScaleHandle([0.0, 0.0], [0.5, 0.5])
            #self.addScaleHandle([0.0, 1.0], [0.5, 0.5])
            #self.addScaleHandle([1.0, 1.0], [0.5, 0.5])
            #self.addScaleHandle([1.0, 0.0], [0.5, 0.5])
            self.addScaleHandle([0.0, 0.0], [1.0, 1.0])
            self.addScaleHandle([0.0, 1.0], [1.0, 0.0])
            self.addScaleHandle([1.0, 1.0], [0.0, 0.0])
            self.addScaleHandle([1.0, 0.0], [0.0, 1.0])

    
class ROISliderGroup:

    def __init__(self,coords,planes,N,pencolors,allow_resize=True,func_setlabel=None,vertices=None):
        """
        coords: list of list.
           outer list is planes, inner list is (x,y,w,h) or (x,y) lower left and width and height
        pos: list of list
           out list is also planes, inner list is (x,y) of vertex
        """
        N = int(N)
        self.rois = []
        self.planes = planes
        self.use_same_times = False   # forces all plane ROIs to have the same time coordinates (Y-axis)
        self.fix_vertex_to_bb = True  # forces vertex to be mid point of bbox (if we're not specifying it)

        # optional labels for U,V,Y: report back the positions of the boxes
        self.setROIlabel = func_setlabel

        # scatter plot of vertex: default is in the center
        self.vertexplot = pg.ScatterPlotItem(pxMode=False)

        if vertices is not None:
            print "bbox specified vertices"
            self.pos  = np.asarray( vertices )
            self.fix_vertex_to_bb = False
        else:
            print "default vertices"
            self.pos = np.zeros( (3,2), dtype=np.float )

        assert( self.pos.shape == (3,2) )
        self.vertices   = [{'pos': self.pos[0,:], 'data': 1, 'pen':(255,0,0,255),'brush':(255,0,0,255), 'symbol':'+', 'size':10},
                           {'pos': self.pos[1,:], 'data': 1, 'pen':(0,255,0,255),'brush':(0,255,0,255), 'symbol':'+', 'size':10},
                           {'pos': self.pos[2,:], 'data': 1, 'pen':(0,0,255,255),'brush':(0,0,255,255), 'symbol':'+', 'size':10}]
        self.vertexplot.addPoints( self.vertices )

        for ix in self.planes:

            coord = coords[ix]
            x,y,w,h = coord                

            roi = ROISlider([x,y], [w, h],pencolors[ix],allow_resize,ix)

            roi.sigRegionChangeFinished.connect(self.resizeROI)
            roi.sigRegionChangeFinished.connect(self.reportPositions)

            self.rois.append(roi)

            if self.fix_vertex_to_bb:
                # use center of boxes
                print "center vertex"
                self.centerVertices()


    def useSameTimes(self):
        self.use_same_times = True

    def useDifferentTimes(self):
        self.use_same_times = False

    def centerVertices(self):
        for ix,roi in enumerate(self.rois):
            roipos = roi.pos()
            roish  = roi.size()
            # use center of boxes
            vx1 = roipos[0]+0.5*roish[0]
            vx2 = roipos[1]+0.5*roish[1]
            self.vertices[ix]["pos"][0] = vx1
            self.vertices[ix]["pos"][1] = vx2
        self.vertexplot.setData( self.vertices )
            
    def resizeROI(self):
        if not self.use_same_times:
            sender = self.rois[0].sender()
            size = sender.size()
            for roi in self.rois:
                if roi == sender: continue
                roi.setSize(size, finish=False)
        else:
            #print "same-time resize. ROI set "%(self)," : "
            sender = self.rois[0].sender()
            sender_pos = sender.pos()
            sender_shape = sender.size()
            tstart = sender_pos[1]
            tend   = sender_pos[1]+sender_shape[1]
            #print "  ",sender
            #print "  ",
            for roi in self.rois:
                pos = roi.pos()
                roi.setPos( [pos[0],tstart], finish=False, update=False )
                s   = roi.size()
                roi.setSize( [s[0], sender_shape[1] ], finish=False, update=False )
                #print roi.pos(),
            #print
        # setting vertex position
        if self.fix_vertex_to_bb:
            self.centerVertices()
    
    def reportPositions(self):
        # this is a function pointer to the owner ROIToolkit class. 
        if self.setROIlabel is not None:
            self.setROIlabel( self.rois )
                
    def setVertex(self,plane, pos):
        # if called, that means user wishes to set vertex position
        print pos,self.vertices[plane]["pos"].shape
        self.fix_vertex_to_bb = False
        self.vertices[plane]["pos"][0] = pos[0]
        if self.use_same_times:
            for vert in self.vertices:
                vert["pos"][1] = pos[1]
        else:
            self.vertices[plane]["pos"][1] = pos[1]
        self.vertexplot.setData( self.vertices )

    # no real need yet for __iter__ and next()
                
