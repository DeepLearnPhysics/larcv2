
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
CFG = os.path.join("trump_example_1.cfg")

print "Loading config... ",CFG
proc.configure(CFG)
flist=ROOT.std.vector('std::string')()

# https://www.nevis.columbia.edu/~vgenty/public/trump_test.root

flist.push_back("trump_test.root")
proc.override_input_file(flist)
proc.override_output_file("c0.root")
proc.override_ana_file("c1.root")

reco_id    = proc.process_id("LArbysImage")
larbysimg  = proc.process_ptr(reco_id)

proc.initialize()


# In[ ]:

larbysimg


# ### Event selection

# In[ ]:

event = 0
proc.batch_process(event,1)


# ### ImageClusterManager Instance

# In[ ]:

mgr = larbysimg.Manager()  
dm  = mgr.DataManager()
print "Where are my images?"
print ".... ImageClusterManager is here (mgr): ",mgr
print
print "Where is my per algoritm data?"
print ".... AlgoDataManger is here (dm): ",dm


# ### Do something!

# In[ ]:

adc_mat_v = mgr.InputImages(0)
trk_mat_v = mgr.InputImages(1)
shr_mat_v = mgr.InputImages(2)

print "adc cv::Mat...",adc_mat_v
print "trk cv::Mat...",trk_mat_v
print "shr cv::Mat...",shr_mat_v


# ### Take a look at the whole guy on plane 2

# In[ ]:

plane = 2
plane_mat = adc_mat_v[plane]

print plane_mat

# Convert to a numpy array
plane_img = pygeo.image(plane_mat)

print plane_img
print plane_img.shape

# Display with imsho
fig,ax = plt.subplots(figsize=(10,10))
ax.imshow(plane_img,cmap="Greys",interpolation='none')
plt.savefig("adc_img.png")
plt.close()
plt.cla()
plt.clf()



# ### Take a look at the track image on plane 2

# In[ ]:

track_mat = trk_mat_v[2]
track_img = pygeo.image(track_mat)

fig,ax = plt.subplots(figsize=(10,10))
ax.imshow(track_img,cmap="Greys",interpolation='none')
plt.savefig("track_img.png")
plt.close()
plt.cla()
plt.clf()


# ### Lets try to find the contour around this guys hair, we can do this with LArOpenCV functions!

# In[ ]:

shower_mat = shr_mat_v[2]
shower_img = pygeo.image(shower_mat)

fig,ax = plt.subplots(figsize=(10,10))
ax.imshow(shower_img,cmap="Greys",interpolation='none')
plt.savefig("shower_img.png")
plt.close()
plt.cla()
plt.clf()


# ### First lets threshold the shower image so we can find the blob of hair

# In[ ]:

shower_mat = shr_mat_v[2]

# Any pixel above 10, lets set the value to be 255 (the maximum 8 bit number)
shower_mat = larocv.Threshold(shower_mat,10,255)

shower_img = pygeo.image(shower_mat)

fig,ax = plt.subplots(figsize=(10,10))
ax.imshow(shower_img,cmap="Greys",interpolation='none')
plt.savefig("shower_threshold_img.png")
plt.close()
plt.cla()
plt.clf()



# ### Lets find the contours using LArOpenCV functions located in AlgoFunction/, specifically the FindContours(cv::Mat) function, which will find clusters in the given image

# In[ ]:

# Get the shower mat
shower_mat = shr_mat_v[2]

# Any pixel above 10, lets set the value to be 255 (the maximum 8 bit number)
shower_thresh = larocv.Threshold(shower_mat,10,255)

# Find contours with LArOpenCV function
shower_ctor_v = larocv.FindContours(shower_thresh)

print shower_ctor_v

# Draw the image
shower_img = pygeo.image(shower_thresh)
fig,ax = plt.subplots(figsize=(10,10))
ax.imshow(shower_img,cmap="Greys",interpolation='none')

# Draw the contours...
print "Found",shower_ctor_v.size(),"contours in the shower image"
for contour in shower_ctor_v:
    # Make a numpy array of points for easier plotting
    ctor = [[pt.x,pt.y] for pt in contour]
    ctor = np.array(ctor)
    ax.plot(ctor[:,0],ctor[:,1],'-',lw=5)
ax.grid()
plt.savefig("contour_img.png")
plt.close()
plt.cla()
plt.clf()


# In[ ]:



