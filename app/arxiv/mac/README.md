#RGBViewer
------
Documentation to do...  
.  
.  
###Branches
###roi_feature 
------
documentation for feature branch [LArbys/LArCV/tree/roi_feature](https://github.com/LArbys/LArCV/tree/roi_feature)

also... this is beta... so please contact me (`vgenty@larbys.com`) or open a ticket with issues 
#####How is the data stored
ROIs placed on the image are stored in an EventROI data product in the output ROOT file, separate from the file containing the displayed image. For the output LArCV file, I set the "id" of the event to `(1,X,event)` where `event` is the TTree entry number for the file containg the Image2D and `X` to indicate if the event contains a valid ROI (i.e. is not empty). When `X=1` the user has indicated a valid ROI, when `X=0` the ROI is empty. `entry` is the same integer value listed under the display in the `Entry` box. 

#####How you should scan

I would start from the beginning and go one by one, placing a set of ROIs (1 per plane) of fixed size (user can choose this in the far right panel) per image. If I don't see what I want, move to the next event. If I put ROIs to my likeing on an image, make sure to press **Capture ROIs** to save that event's ROIs to memory. If I get tired I would stop at a certain point, hit **Capture ROIs** then ***Store ROIs***. To pick up where I left off I would **start a brand new ROOT file** and move where I stopped. Then offline I would **concatenate** the two files.  
  
 The point of concatenation is to combine files when you start and stop a scan. The only way to match up the ROI to the Image2D is via the "id" I give it (described above). As an example please take a look at [mac/concat_roi.py](https://github.com/LArbys/LArCV/blob/roi_feature/mac/mac/concat_roi.py). I have two files for which I did two scans ([god.root](http://www.nevis.columbia.edu/~vgenty/public/god.root) and [god2.root](http://www.nevis.columbia.edu/~vgenty/public/god2.root)). In the first file I placed an ROI on the first event and stopped my scan. In the second file I skipped the first two events, and on the third event (TTree entry 2) I placed an ROI. With this example I produce a new file `god3.root` which contains ROIs for both scans. The first and third event have ROIs. The second one is empty.

#####What do all the buttons do?
* **Load Files** : start IOManager to do I/O for the given input/output LArCV files.
* **Add ROIs** : adds a set of 3 ROI boxes to the plane for you to move around
* **Remove ROIs** : remove the most recent ROI group (3 rois) from the image
* **Capture ROIs** : store current set of ROIs on the screen into memory
* **Clear ROIs** : clear all the ROIs from the current image
* **Reset ROIs** : clear all ROIs in memory
* **Store ROIs** : store to LArCV file
* **Fixed ROI** : mark this check box if you want to pre-configure how big your ROI box will be in pixel width and height. If this box is un-checked, the user can manually set the size of the ROI boxes by dragging.
* **W** : fixed pixel width
* **H** : fixed pixel height

#####How to run
1. Open up RGBViewer in this branch
![alt text](http://www.nevis.columbia.edu/~vgenty/public/_____1.png "1")

2. Toggle the ROITool menu by clicking the **Enable ROITool**
![alt text](http://www.nevis.columbia.edu/~vgenty/public/_____2.png "2")
 
3. If this your first time, or you want to start a new scan specify the output file path and output file ROI producer (this will be the producer name in the LArCV file). I have chosen `output.root` with producer `ahoproducer`. Click **Load Files**.

![alt text](http://www.nevis.columbia.edu/~vgenty/public/_____3.png "3")

4. Lets add an ROI to our image by clicking **Add ROIs**. Something will appear at the bottom left of your screen. There are 3 ROI boxes on top of each other which we will move to the particle location.

![alt text](http://www.nevis.columbia.edu/~vgenty/public/_____4.png "4")

5. I have moved my 3 color boxes to the location of the particles, and resized them to enclose the area I want. **Important note** due to a quirk of `pyqtgraph` once you resize one of the boxes you must make a slight positional adjustment to that box for the other 2 boxes to update in size. I have restricted the size of the boxes to be all the same we can change that if it bothers you. Once you have them in the right spot you must click **Capture ROIs** to save them into memory. To store all ROIs from each of the events into persistent memory in LArCV file on disk click **Store ROIs**.

![alt text](http://www.nevis.columbia.edu/~vgenty/public/_____5.png "5")

6. We can reload the ROI's for a particular event by reloading from the same data file. You have to hit **Replot** to load the ROIs from disk!

![alt text](http://www.nevis.columbia.edu/~vgenty/public/_____6.png "6")



