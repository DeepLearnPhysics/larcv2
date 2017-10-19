
# coding: utf-8

# ### Analyze a Trump using LArbysImage

# In[ ]:

import os, sys
import ROOT
from ROOT import fcllite
from ROOT import geo2d
from larcv import larcv
import cv2
pygeo = geo2d.PyDraw()
from ROOT import larocv
from ROOT import std
from ROOT import cv
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.patches as patches
matplotlib.rcParams['font.size']=20
matplotlib.rcParams['font.family']='serif'

import numpy as np


# ### Prepare ProcessDriver

# In[ ]:

proc = larcv.ProcessDriver('ProcessDriver')
CFG = "example_vertex.cfg"

print "Loading config... ",CFG
proc.configure(CFG)
flist=ROOT.std.vector('std::string')()

flist.push_back("img/test_img.root")
proc.override_input_file(flist)
proc.override_output_file("c0.root")
proc.override_ana_file("c1.root")

reco_id    = proc.process_id("LArbysImage")
larbysimg  = proc.process_ptr(reco_id)

proc.initialize()


# ### Event selection

# In[ ]:

event = 0
proc.batch_process(event,1)


# ### ImageManager Instance

# In[ ]:

mgr = larbysimg.Manager()  
dm  = mgr.DataManager()
print "Where are my images?"
print ".... ImageClusterManager is here (mgr): ",mgr
print
print "Where is my per algoritm data?"
print ".... AlgoDataManger is here (dm): ",dm


# ### Grab the ADC image

# In[ ]:

adc_mat_v = mgr.InputImages(0)


# ### Get the algo data for the TrumpExample module

# In[ ]:

print "The list of modules..."
print
for ix,name in enumerate(dm.Names()):
    print ix,")",name


# In[ ]:


cheap_algo_id = mgr.GetClusterAlgID("cheap_vertex_finder")
print "Got cheap_vertex_finder algorithm ID --",cheap_algo_id
cheap_algo = mgr.GetClusterAlg(cheap_algo_id)
print "Got cheap algo pointer",cheap_algo

for plane in xrange(3):

    pt = cheap_algo._closest_pt_v.at(plane);
    print "Got point @ pt=(",pt.x,",",pt.y,")"

    img = pygeo.image(adc_mat_v.at(plane))
    plt.imshow(img)

    plt.plot(pt.x,pt.y,'*',color='cyan',markersize=10)
    plt.savefig("adc_plane_%d.png" % plane)
    plt.close()
    plt.cla()
    plt.clf()
    
    print

# In[ ]:



