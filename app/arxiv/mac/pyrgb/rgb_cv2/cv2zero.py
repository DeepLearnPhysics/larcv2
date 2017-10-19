from cv2selection import CV2Selection
from cv2selection import QtGui


class CV2Zero(CV2Selection):

    def __init__(self):
        super(CV2Zero, self).__init__()
        self.name = "CV2Zero"

    def __description__(self):
    	pass

    def __parsewidgets__(self):
    	pass
    	
    def __implement__(self, image):
        return 0.0 * image
