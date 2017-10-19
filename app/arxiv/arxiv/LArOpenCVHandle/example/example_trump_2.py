
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
CFG = os.path.join("trump_example_2.cfg")

print "Loading config... ",CFG
proc.configure(CFG)
flist=ROOT.std.vector('std::string')()

flist.push_back("trump_test.root")
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

trump_algo_id = mgr.GetClusterAlgID("trumpexample")
print "Got TrumpExample algorithm ID --",trump_algo_id
print



for plane in xrange(3):
    par_data = dm.Data(trump_algo_id,plane).as_vector()
    
    print "Got",par_data.size(),"particles on plane",plane
    
    # I know there is only one particle so lets get the first one
    par=par_data.front()
    
    # Convert it to a numpy array
    hair_ctor = [[pt.x,pt.y] for pt in par._ctor]
    hair_ctor = np.array(hair_ctor)
    
    fig,ax=plt.subplots(figsize=(10,6))
    img = pygeo.image(adc_mat_v.at(plane))
    ax.imshow(img,cmap='Greys',interpolation='none')
    
    # Draw the contour on the image
    ax.plot(hair_ctor[:,0],hair_ctor[:,1],lw=2)
    
    ax.set_title("Trump @ Plane %d"%plane,fontsize=20)
    ax.grid()
    plt.savefig("example_2_%d.png"%plane)
    plt.close()
    plt.cla()
    plt.clf()
    print


# In[ ]:



