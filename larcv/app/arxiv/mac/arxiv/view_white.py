#!/usr/bin python
# thanks taritree(!)

import os,sys
from PyQt4 import QtGui, QtCore
import pyqtgraph as pg
from pyrgb.display import whitedisplay as whited

pg.setConfigOption('background', 'w')
pg.setConfigOption('foreground', 'k')

inputs = sys.argv

app = QtGui.QApplication([])
whitedisplay = whited.WhiteDisplay(sys.argv[1:])
whitedisplay.show()

if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
    print "exec called ..."
    whitedisplay.show()
    QtGui.QApplication.instance().exec_()
