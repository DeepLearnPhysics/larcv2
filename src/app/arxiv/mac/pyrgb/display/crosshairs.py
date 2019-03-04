import os,sys
from .. import pg

# #cross hair
# vLine = pg.InfiniteLine(angle=90, movable=False)
# hLine = pg.InfiniteLine(angle=0, movable=False)
# p1.addItem(vLine, ignoreBounds=True)
# p1.addItem(hLine, ignoreBounds=True)


# vb = p1.vb

# def mouseMoved(evt):
#     pos = evt[0]  ## using signal proxy turns original arguments into a tuple
#     if p1.sceneBoundingRect().contains(pos):
#         mousePoint = vb.mapSceneToView(pos)
#         index = int(mousePoint.x())
#         if index > 0 and index < len(data1):
#             label.setText("<span style='font-size: 12pt'>x=%0.1f,   <span style='color: red'>y1=%0.1f</span>,   <span style='color: green'>y2=%0.1f</span>" % (mousePoint.x(), data1[index], data2[index]))
#         vLine.setPos(mousePoint.x())
#         hLine.setPos(mousePoint.y())



# proxy = pg.SignalProxy(p1.scene().sigMouseMoved, rateLimit=60, slot=mouseMoved)
# #p1.scene().sigMouseMoved.connect(mouseMoved)


class CrossHairs:
    def __init__( self, roitool, pltitem, imgitem, plane ):
        self.plane = plane
        self.plt = pltitem
        self.imi = imgitem
        self.roitool = roitool
        self.vLine = pg.InfiniteLine(angle=90, movable=False)
        self.hLine = pg.InfiniteLine(angle=0, movable=False) 
        self.colors = ['red','green','blue']
        self.label = pg.LabelItem(justify='right')
        self.proxy = pg.SignalProxy(self.plt.scene().sigMouseMoved, rateLimit=60, slot=self.mouseMoved)
        #self.plt.scene().sigMouseClicked.connect( self.mouseClicked )
        self.active = False
        self.last_point = None
        self.last_click = None
        

    def mouseMoved(self,evt):
        if not self.active:
            return

        pos = evt[0]  ## using signal proxy turns original arguments into a tuple
        if self.plt.sceneBoundingRect().contains(pos):
            mousePoint = self.plt.vb.mapSceneToView(pos)
            index = int(mousePoint.x())  
            tt = "<span style='font-size: 12pt' style='color: %s'>x=%0.1f,   <span style='color: %s'>y=%0.1f</span>" % (self.colors[self.plane],
                                                                                                                        mousePoint.x(), 
                                                                                                                        self.colors[self.plane],
                                                                                                                        mousePoint.y() )
            self.label.setText(tt)
            self.vLine.setPos(mousePoint.x())
            self.hLine.setPos(mousePoint.y())
            self.last_point = mousePoint

            
    def mouseClicked(self,evt):
        if self.active:
            print "mouse-click: cross-hair response. plane=",self.plane
            print "(x,y)=",self.last_point
            self.plt.removeItem( self.vLine )
            self.plt.removeItem( self.hLine )
            if self.plane==0:
                self.roitool.set_uvertex.setEnabled(True)
            elif self.plane==1:
                self.roitool.set_vvertex.setEnabled(True)
            elif self.plane==2:
                self.roitool.set_yvertex.setEnabled(True)
            self.active = False
            return True
        return False

    def activate(self):
        pass
    
    def getvertex(self):
        return (self.last_point.x(),self.last_point.y())
