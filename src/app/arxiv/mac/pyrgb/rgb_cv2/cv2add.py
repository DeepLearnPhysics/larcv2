from cv2selection import CV2Selection
from cv2selection import QtGui


class CV2Add(CV2Selection):

    def __init__(self):
        super(CV2Add, self).__init__()

        self.name = "CV2Add"
        self.options['add'] = int(1)
        self.types['add'] = int

        self.widgets['add'] = (QtGui.QLabel("Add"), QtGui.QLineEdit())

        for key, value in self.widgets.iteritems():
            value[1].setText(str(self.options[key]))

    def __description__(self):
        return "No description provided!"

    def __parsewidgets__(self):
        for key, val in self.widgets.iteritems():
            self.options[key] = self.str2data(
                self.types[key], str(val[1].text()))

    def __implement__(self, image):
        return self.options['add'] + image
