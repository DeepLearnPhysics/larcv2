# ImageMod LArCV Modules

This folder contains a number of different modules that manipulate an LArCV image.
Modules that do not modify an image (such as modules to analyze an image) should be in ImageAna package.
Many are used as a preprocessor before the image is passed into caffe or tensorflow.

Short description of each module can be found in the table below, followed by detailed description later.

| Module Name | Short Description |
|-------------|:-----------------:|
| EmbedImage  | Embed image into a larger, blank, image |
| ROIMask     | Mask out parts of image using ROI |
| SegmentRelabel | Relabel segmentation map value |
| WireMask | Mask certain column pixels in an image |


## Description of each module

Please provide a detailed description of each module here. (Sorted in Alphabetical order)

List of modules:

* EmbedImage
* ROIMask
* SegmentRelabel
* WireMask

### EmbedImage

This module embeds an image of a certain size (row,col) into a larger (row',col') image. 
It attempts to be consistent about the Meta information, expanding the state width and height, along with adjusting the (x,y) origins.
The initial use case was to embed an HiRes cropped image into an image that is a nice power of 2.
This makes architectures like FCN or U-Net easier to construct.

N.B. This does somethig similar to ResizeImage. 
However, the approach to specifying transformation is different enough to make a new module.
ResizeImage requires one to respecify the meta for each image.  
EmbedImage makes a modification to the number of rows and cols, inferring the other meta parameters from the original meta.
For HiRes cropped images, this is useful because things like the origin will change from image to image.

Parameters

| Parameters | Description |
|------------|:-----------:|
| InputProducerName | name of input image2d |
| OutputProducerName | name of output image2d. if same as InputSegmentProducerName, then modification in-place (i.e. original image modified). |
| OutputRows | size of output rows (must be bigger than original) |
| OutputCols | size of output cols (must be bigger than original) |

### ROIMask

This module allows one to mask out parts of an image using an ROI.  One can either blank out the region inside or outside the ROI.

Parameters

| Parameters | Description  |
|------------|:------------:|
|InputImageProducer| (string) Input Image2D producer name. |
|OutputImageProducer| (string) Output Image2d producer name. If the same as the input producer, then modification made in-place. |
|InputROIProducer| (string) ROI producer name from which we get the masking producer |
|OutputROIProducer| (string) ROI producer name into which we store the ROI |
|MaskOutsideROI  | (bool) If true, blank out region outside the ROI. If false, blank out region inside the ROI. |
|CutOutOfBounds  | (bool, optional, default=true) If true, if ROI extends outside of image in anyway, event is skipped. Otherwise blank image inserted. |
|ROIid | (int, optional, default=0) Index of ROI array to use |
|ROILabel | (int, optional, default=2,kROIBNB) Label given to output ROI |

### SegmentRelabel

The intention of this module is to relabel the segmentation image.  
For example, we might want to relabel electrons and gammas as "showers" or relabel muon and pions as "tracks".

Parameters

| Parameters | Description |
|------------|:-----------:|
| InputSegmentProducerName | name of input segment image2d |
| OutputSegmentProducerName | name of output  segment image2d. if same as InputSegmentProducerName, then stored in-place |
| LabelMap | PSet connecting new label to a list of old labels. ex. 0:[0,1,2]. You can find example in segmentrelabel.cfg |

### WireMask

This module masks certain column pixels in an image.
Given a list of X positions in the original image coordinate, the module go look for a corresponding column index by:

target column = X position / ( image width / image column count )

The name WireMask comes from the fact this software is for LArTPC and X coordinate = wire number to date.
So there is an explicit assumption of wire number being the original X coordinate of the image before downsizing.
Also, this module "modifies" an image, rather than creating an output (this saves IO overhead).

Parameters

| Parameters | Description |
|------------|:-----------:|
| ImageProducer | name of target image2d |
| PlaneID | literally an index number of image2d array to mask|
| WireList | integer array of wire numbers to be masked|
| MaskValue | floating point used as a value for masking |






