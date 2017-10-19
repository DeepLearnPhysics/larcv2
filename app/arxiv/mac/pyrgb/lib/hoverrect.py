from .. import QtGui

# subclass of ractangle item, this allows me to
# overlay the particle type if the user decides to hover over it

class HoverRect(QtGui.QGraphicsRectItem) :
    def __init__(self,x,y,w,h,text,plt):
        super(HoverRect, self).__init__(x,y,w,h)

        self.setAcceptsHoverEvents(True)
        self.text = text
        self.plot = plt
        
    def hoverEnterEvent(self, event):
        self.plot.addItem(self.text)
        
    def hoverLeaveEvent(self, event):
        self.plot.removeItem(self.text)
        
