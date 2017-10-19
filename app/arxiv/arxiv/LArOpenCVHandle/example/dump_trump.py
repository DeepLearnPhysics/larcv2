from larcv import larcv

iom = larcv.IOManager()

iom.add_in_file("trump_test.root")

iom.initialize()

iom.read_entry(0)

ev_img = iom.get_data(larcv.kProductImage2D,"trump")
ev_trk = iom.get_data(larcv.kProductImage2D,"trump_trk")
ev_shr = iom.get_data(larcv.kProductImage2D,"trump_shr")

import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt
import numpy as np

for plane in xrange(3):

    print "@plane=",plane
    
    adc_array = larcv.as_ndarray(ev_img.Image2DArray().at(plane))
    trk_array = larcv.as_ndarray(ev_trk.Image2DArray().at(plane))
    shr_array = larcv.as_ndarray(ev_shr.Image2DArray().at(plane))

    plt.imshow(adc_array,cmap="Greys",interpolation='None')
    plt.savefig("adc_plane_%d.png"%plane)

    plt.imshow(trk_array,cmap="Greys",interpolation='None')
    plt.savefig("trk_plane_%d.png"%plane)
    
    plt.imshow(shr_array,cmap="Greys",interpolation='None')
    plt.savefig("shr_plane_%d.png"%plane)
