from larcv import larcv
import os, sys
larcv.logger.force_level(0)

#
# Constants
#
MSG_LEVEL=larcv.msg.kERROR
if 'debug' in sys.argv:
    MSG_LEVEL = larcv.msg.kDEBUG
if 'info' in sys.argv:
    MSG_LEVEL = larcv.msg.kINFO

OUT_FNAME="merger.root"
NUM_EVENT=1

ERROR_FILE_EXIST      = 1
ERROR_WRITE_INIT      = 2

# if os.path.isfile(OUT_FNAME):
#     print "Test output file (%s) already exists..." % OUT_FNAME
#     sys.exit(ERROR_FILE_EXIST)

from larcv import larcv
o=larcv.IOManager(larcv.IOManager.kWRITE)
o.reset()
o.set_verbosity(MSG_LEVEL)
o.set_out_file(OUT_FNAME)

stream1 = larcv.DataStream("DataStream1")
stream1.set_verbosity(MSG_LEVEL)
cfg1 = larcv.CreatePSetFromFile(sys.argv[2],"DataStream1")
stream1.configure(cfg1)

stream2 = larcv.DataStream("DataStream2")
stream1.set_verbosity(MSG_LEVEL)
cfg2 = larcv.CreatePSetFromFile(sys.argv[3],"DataStream2")
stream2.configure(cfg2)

p = larcv.ImageMerger()
p.set_verbosity(2)

cfg = larcv.CreatePSetFromFile(sys.argv[1],"ImageMerger")
p.InputImageHolder1(stream2)
p.InputImageHolder2(stream1)

p.configure(cfg)

mask = larcv.WireMask()
cfg3 = larcv.CreatePSetFromFile(sys.argv[4],"WireMask")
mask.configure(cfg3)

stream1.initialize()
stream2.initialize()
p.initialize()
mask.initialize()

if not o.initialize():
    sys.exit(ERROR_WRITE_INIT)

for idx in xrange(NUM_EVENT):
    
    #we have to make the image from ImageMeta if we are going to
    #use channel status as it checks image2d.plane
    meta1=larcv.ImageMeta(10,10,10,10,0,10,0)
    img1 = larcv.Image2D(meta1)
    for x in xrange(img1.as_vector().size()):
        if x%2 == 0: img1.set_pixel(x,10)
        else: img1.set_pixel(x,0)
    
    meta2=larcv.ImageMeta(10,10,10,10,0,10,0)
    img2 = larcv.Image2D(meta2)
    for x in xrange(img2.as_vector().size()):
        img2.set_pixel(x,10)
        if (x/10)%2 == 0: img2.set_pixel(x,0)

    #Input stream 1        
    event_image1_tpc = o.get_data(larcv.kProductImage2D,"stream1_tpc") # combined image is going to steal (std::move) this from me
    event_image1_pmt = o.get_data(larcv.kProductImage2D,"stream1_pmt") # at the end we have to go back and put shit in there
    event_image1_tpc.Append(img1)
    event_image1_pmt.Append(img1)

    event_roi1 = o.get_data(larcv.kProductROI,"stream1")
    bb1 = larcv.ImageMeta(2,2,2,2,2,2,0)
    roi1= larcv.ROI()
    roi1.AppendBB(bb1)
    event_roi1.Append(roi1)

    event_ch_status1 = o.get_data(larcv.kProductChStatus,"stream1")
    ch_status1 = larcv.ChStatus()
    ch_status1.Initialize(10) # 10 wires, some idiot did not plug in
    ch_status1.Status(1,0) #this one is fucked
    ch_status1.Status(2,0) #this one is fucked
    ch_status1.Plane(0)
    event_ch_status1.Insert(ch_status1)

    #Input stream 2
    event_image2_tpc = o.get_data(larcv.kProductImage2D,"stream2_tpc")
    event_image2_pmt = o.get_data(larcv.kProductImage2D,"stream2_pmt")
    event_image2_tpc.Append(img2)
    event_image2_pmt.Append(img2)

    event_roi2 = o.get_data(larcv.kProductROI,"stream2")
    bb2 = larcv.ImageMeta(2,2,2,2,0,5,0)
    roi2= larcv.ROI()
    roi2.AppendBB(bb2)
    event_roi2.Append(roi2)

    event_ch_status2 = o.get_data(larcv.kProductChStatus,"stream2")
    ch_status2 = larcv.ChStatus()
    ch_status2.Initialize(10) # 10 wires, some idiot did not plug in
    ch_status2.Status(5,0) #this one is fucked
    ch_status2.Status(6,0) #this one is fucked
    ch_status2.Status(7,0) #and this one is fucked!
    ch_status2.Plane(0)
    event_ch_status2.Insert(ch_status2)
    
    #Processing
    stream1.process(o)
    stream2.process(o)
    p.process(o)
    mask.process(o)
    o.set_id(0,0,idx)
    
    #Store Originals
    event_image1_tpc.Append(img1)
    event_image1_pmt.Append(img1)
    event_image2_tpc.Append(img2)
    event_image2_pmt.Append(img2)
    bb1 = larcv.ImageMeta(2,2,2,2,2,2,0)
    roi1= larcv.ROI()
    roi1.AppendBB(bb1)
    event_roi1.Append(roi1)
    bb2 = larcv.ImageMeta(2,2,2,2,0,5,0)
    roi2= larcv.ROI()
    roi2.AppendBB(bb2)
    event_roi2.Append(roi2)
    
    #############################################
    #Lets get the data out and make assertions
    combined_ev_tpc_img = o.get_data(larcv.kProductImage2D,"combined_tpc")
    
    assert combined_ev_tpc_img.Image2DArray().size() == 1

    combined_tpc_img = combined_ev_tpc_img.Image2DArray()[0];
    
    for r in xrange(combined_tpc_img.meta().rows()):
        for c in xrange(combined_tpc_img.meta().cols()):
            px=combined_tpc_img.pixel(r,c)
            
            #bad wires
            if c in [1,2,5,6,7]: 
                assert px == 0.0; 
                continue;

            #overlap
            if r%2 == 0 and c%2 == 1: 
                assert px == 20.0
                continue
    print "PASS event: ",idx
    o.save_entry()

    idx+=1

stream1.finalize()
stream2.finalize()
p.finalize()
o.finalize()
