from cv2selection import CV2Selection
from cv2selection import QtGui

class CV2CopyPaste(CV2Selection):

    def __init__(self):
        super(CV2CopyPaste, self).__init__()

        self.name = "CV2CopyPaste"

     
        self.widgets['copy'] = (QtGui.QLabel(""), QtGui.QPushButton("Copy"))
        self.widgets['copy'][1].clicked.connect(self.copyData)

        self.widgets['paste'] = (QtGui.QLabel(""), QtGui.QPushButton("Paste"))
        self.widgets['paste'][1].clicked.connect(self.pasteData)

        self.widgets['aoverwrite'] = (QtGui.QLabel("Copy Paste Mode"),
                                     QtGui.QLabel("Please turn overwrite on!"))


        self.image_ref = None

        self.paste = False
        self.copy = False

        self.pasted = False
        self.copied = False

        self.image_copy = None

    def __description__(self):
        return "No description provided!"

    def __parsewidgets__(self):
        print "Nothing to parse"

    def __implement__(self, image): # updated every ROI move
        self.image_ref = image # shit better be reference or i will cry

        if self.copy == False and self.paste == False:
            return image

        if self.paste == True and self.pasted == False:
            self.pasted = True
            return self.image_copy

        if self.paste == True and self.pasted == True:
           self.image_copy = None #clear out the copy for the next go around

        self.copy = False
        self.copied = False

        self.paste = False
        self.pasted = False

        return image

    def copyData(self):
        self.copied = True
        self.image_copy = self.image_ref.copy()

    def pasteData(self):
        self.paste = True
