#!/usr/bin python
# thanks taritree(!)

import os,sys
from pyqtgraph.Qt import QtGui, QtCore
import pyqtgraph as pg
from pyrgb.display import rgbdisplay as rgbd

inputs = sys.argv

app = QtGui.QApplication([])
rgbdisplay = rgbd.RGBDisplay(sys.argv[1:])
rgbdisplay.show()

if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
    print "exec called ..."
    rgbdisplay.show()
    QtGui.QApplication.instance().exec_()
