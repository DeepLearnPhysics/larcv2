import ROOT
#ROOT.gSystem.Load("libLArCV")
#ROOT.gSystem.Load("libLArCVData")
from larcv import larcv
o=larcv.IOManager(larcv.IOManager.kBOTH)
o.reset()
o.set_verbosity(0)
o.add_in_file("aho.root")
o.set_out_file("baka.root")
o.initialize()

o.read_entry(0)
o.get_data(larcv.kProductImage2D,"aho")
o.get_data(larcv.kProductImage2D,"boke")
o.save_entry()

o.read_entry(1)
o.save_entry()

o.read_entry(2)
o.save_entry()

o.read_entry(3)
o.save_entry()

o.finalize()
