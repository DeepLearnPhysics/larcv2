from larcv import larcv
import ROOT
# meta=larcv.EventMeta()

# darray = ROOT.std.vector('double')(10,0.)
# iarray = ROOT.std.vector('int')(10,0)
# sarray = ROOT.std.vector('string')(10,'')
# for i in xrange(darray.size()):
#     darray[i] = float(i)
#     iarray[i] = i*2
#     sarray[i] = "abcdefghijklmnopqrstuvwxyz"[i]
# meta.store("kazu_darray",darray)
# meta.store("kazu_iarray",iarray)
# meta.store("kazu_sarray",sarray)
# meta.store("NumPlanes",3)
# meta.store("DriftVelocity",0.11)
# meta.store("LEE","DoesNotExist")




o=larcv.IOManager(larcv.IOManager.kWRITE)
o.reset()
o.set_verbosity(larcv.msg.kERROR)
o.set_out_file('fake_meta.root')
if not o.initialize():
    sys.exit(1)

ctr = 1
event_meta = o.get_data("meta","2D")

n_planes = 3
x_min = ROOT.std.vector('double')(n_planes)
y_min = ROOT.std.vector('double')(n_planes)
x_max = ROOT.std.vector('double')(n_planes)
y_max = ROOT.std.vector('double')(n_planes)

x_n_pixels = ROOT.std.vector('double')(n_planes)
y_n_pixels = ROOT.std.vector('double')(n_planes)

for i in xrange(n_planes):
#     x_min[i] = 0.0
#     y_min[i] = 2400.0
#     x_max[i] = 3456
#     y_max[i] = 8448
    x_n_pixels[i] = 512
    y_n_pixels[i] = 512

x_min[0] = 360
x_min[1] = 767
x_min[2] = 711

y_min[0] = 3768
y_min[1] = 3768
y_min[2] = 3768

x_max[0] = 871
x_max[1] = 1278
x_max[2] = 1222

y_max[0] = 6857
y_max[1] = 6857
y_max[2] = 6857


event_meta.store("n_planes",n_planes)
event_meta.store("x_min", x_min)
event_meta.store("y_min", y_min)
event_meta.store("x_max", x_max)
event_meta.store("y_max", y_max)
event_meta.store("x_n_pixels", x_n_pixels)
event_meta.store("y_n_pixels", y_n_pixels)
event_meta.dump()

o.set_id(1,0,ctr)
o.save_entry()

o.finalize()

