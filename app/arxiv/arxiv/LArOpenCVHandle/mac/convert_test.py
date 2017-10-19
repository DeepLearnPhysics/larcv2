import ROOT,sys
from larlite import larlite as fmwk1
from larcv import larcv as fmwk2
from ROOT import handshake

io1=fmwk1.storage_manager(fmwk1.storage_manager.kBOTH)
io1.add_in_filename(sys.argv[1])
io1.set_out_filename('boke.root')
io1.open()

io2=fmwk2.IOManager(fmwk2.IOManager.kREAD)
io2.add_in_file(sys.argv[2])
io2.initialize()

hs=handshake.HandShaker()

ctr=0
while io1.next_event() and io2.read_entry(ctr):

    ev_pfpart  = io1.get_data(fmwk1.data.kPFParticle, "dl")
    ev_vertex  = io1.get_data(fmwk1.data.kVertex,     "dl")
    ev_shower  = io1.get_data(fmwk1.data.kShower,     "dl")
    ev_track   = io1.get_data(fmwk1.data.kTrack,      "dl")
    ev_cluster = io1.get_data(fmwk1.data.kCluster,    "dl")
    ev_hit     = io1.get_data(fmwk1.data.kHit,        "dl")
    ev_ass     = io1.get_data(fmwk1.data.kAssociation,"dl")

    ev_hit_in  = io1.get_data(fmwk1.data.kHit, "gaushit")
    ev_pgraph  = io2.get_data(fmwk2.kProductPGraph,'test')
    ev_pixel2d = io2.get_data(fmwk2.kProductPixel2D,'test_ctor')

    hs.pixel_distance_threshold(1.)
    hs.set_larlite_pointers(ev_pfpart, ev_vertex,
                            ev_shower, ev_track,
                            ev_cluster, ev_hit,
                            ev_ass)

    hs.construct(ev_pgraph, ev_pixel2d, ev_hit_in)
    io1.set_id(io1.run_id(), io1.subrun_id(), io1.event_id())
    #io1.next_event()
    #io1.go_to()
    #io2.read_entry()
    #io1.save_entry()
    ctr+=1


io1.close()
io2.finalize()


