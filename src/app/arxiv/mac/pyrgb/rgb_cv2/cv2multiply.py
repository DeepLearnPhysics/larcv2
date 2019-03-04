from cv2selection import CV2Selection
from cv2selection import QtGui


class CV2Multiply(CV2Selection):

    def __init__(self):
        super(CV2Multiply, self).__init__()

        self.name = "CV2Multiply"
        self.options['multiply'] = float(1)
        self.types['multiply'] = float

        self.widgets['multiply'] = (
            QtGui.QLabel("Multiply"), QtGui.QLineEdit())

        for key, value in self.widgets.iteritems():
            value[1].setText(str(self.options[key]))

    def __description__(self):
        return "No description provided!"

    def __parsewidgets__(self):
        for key, val in self.widgets.iteritems():
            self.options[key] = self.str2data(
                self.types[key], str(val[1].text()))

    def __implement__(self, image):
        return self.options['multiply'] * image
