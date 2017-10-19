from .. import QtGui


class CaffeLayout(QtGui.QGridLayout):

    def __init__(self, tw, display):
        super(CaffeLayout, self).__init__()

        #unforuntaely this layout has to know about it's parent
        #for multichannel image
        self.display = display

        self.name = "CaffeLayout"
        self.caffe_label = QtGui.QLabel("<b>Caffe Integration</b>")
        self.open_deploy = QtGui.QPushButton("Open")
        self.load_config = QtGui.QPushButton("Load")
        self.line_deploy = QtGui.QLineEdit("absolute path to configuration YAML")
        self.load_config.clicked.connect(self.loadConfig)
        self.open_deploy.clicked.connect(self.selectFile)

        self.forward = QtGui.QPushButton("Forward")
        self.forward.clicked.connect(self.network_forward)

        self.loaded_config = QtGui.QLabel("")
        self.scores = QtGui.QLabel("")

        self.tw = tw

    def selectFile(self):
        self.line_deploy.setText(QtGui.QFileDialog.getOpenFileName())

    def grid(self, enable):
        if enable == True:
            self.addWidget(self.caffe_label, 0, 0)
            self.addWidget(self.line_deploy, 1, 0)
            self.addWidget(self.open_deploy, 1, 1)
            self.addWidget(self.load_config, 1, 2)
            self.addWidget(self.forward, 1, 3)
            self.addWidget(self.loaded_config, 2, 0)
            self.addWidget(self.scores, 3, 0)

        else:
            for i in reversed(range(self.count())):
                self.itemAt(i).widget().setParent(None)

        return self

    def loadConfig(self):
        cfg = str(self.line_deploy.text())
        self.tw.set_config(cfg)
        self.loaded_config.setText("<b>Loaded:</b> {}".format(cfg))

    def network_forward(self):
        self.tw.set_image(self.display.load_current_image())
        self.tw.forward_result()
        self.scores.setText("<b>Scores:</b> {}".format(self.tw.scores))
