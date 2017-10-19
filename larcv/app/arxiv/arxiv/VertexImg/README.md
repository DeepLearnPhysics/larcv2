# VertexImg

This module uses ROI to make a vertex "image".  Here we mark pixels with 1 in the neighborhood of the interaction vertex.

Note that this module uses MicroBooNE detector constants from LArLite.  So only built if LARLITE is setup.

## Usage

Parameters

| Parameters | Description |
|------------|:-----------:|
| ROIProducerName    | Input ROI producer name |
| ImageProducerName  | Input image producer name |
| OutputProducerName | Output image producer name. If same as ImageProducerName, then edited in place. |
| VertexRadius       | Radius within which pixels are marked as a vertex |
| ImageTickStart     | tick number that whole event view image starts at (usually 2400) |
| MarkValue          | value given to pixels around vertex (default: 1) |
