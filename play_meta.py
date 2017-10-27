from larcv import larcv
import ROOT
meta=larcv.EventMeta()

darray = ROOT.std.vector('double')(10,0.)
iarray = ROOT.std.vector('int')(10,0)
sarray = ROOT.std.vector('string')(10,'')
for i in xrange(darray.size()):
    darray[i] = float(i)
    iarray[i] = i*2
    sarray[i] = "abcdefghijklmnopqrstuvwxyz"[i]
meta.store("kazu_darray",darray)
meta.store("kazu_iarray",iarray)
meta.store("kazu_sarray",sarray)
meta.store("NumPlanes",3)
meta.store("DriftVelocity",0.11)
meta.store("LEE","DoesNotExist")

meta.dump()

