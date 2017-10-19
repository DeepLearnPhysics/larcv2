from cv2selection import CV2Selection
from cv2selection import QtGui
import cv2

class CV2GausBlur(CV2Selection):
    def __init__(self):
    	super(CV2GausBlur,self).__init__()
        
    	self.name = "CV2GausBlur"

    	# default options
    	self.options['ksize']  = (5,5)
    	self.options['sigmaX'] = 1.0
        self.options['sigmaY'] = 1.0

        self.types['ksize']  = tuple
        self.types['sigmaX'] = float
        self.types['sigmaY'] = float

        self.widgets['ksize']  = (QtGui.QLabel("Size"), QtGui.QLineEdit())
        self.widgets['sigmaX'] = (QtGui.QLabel("Sigma X"), QtGui.QLineEdit())
        self.widgets['sigmaY'] = (QtGui.QLabel("Sigma Y"), QtGui.QLineEdit())
        
        for key,value in self.widgets.iteritems():
            value[1].setText(str(self.options[key]))
        
    def __description__(self):
    	return "No description provided!"

    def __parsewidgets__(self):
	 for key,val in self.widgets.iteritems():
            self.options[key] = self.str2data(self.types[key],str(val[1].text()))
    
    def __implement__(self,image):
	return cv2.GaussianBlur(image,**self.options)
