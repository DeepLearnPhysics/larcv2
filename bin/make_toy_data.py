from __future__ import print_function

from larcv import larcv
from colored_msg import colored_msg as cmsg
import os, sys
import numpy as np
import numpy.random as npr
larcv.logger.force_level(0)

#
# Constants
#
OUT_FNAME="fakedata.root"
NUM_EVENT=10
MSG_LEVEL=larcv.msg.kERROR
STEP_SIZE=0.2 # cm
DEDX = 2.1 # MeV/cm

VOX_DEF = larcv.Voxel3DMeta()
VOX_DEF.set(0.,0.,0., # xmin, ymin, zmin
            512.,512.,512., # xmax, ymax, zmax
            512,512,512, # num_vox_x, num_vox_y, num_vox_z
            larcv.kUnitCM)
def make_meta(plane):
    return larcv.ImageMeta(512.,512., # width height
                           512,512, # row, col
                           0., 0., # min x, min y
                           int(plane), larcv.kUnitWireTime)

def make_particle(pdg, start, end):
    res = larcv.Particle()
    res.PdgCode(pdg)
    res.Position(start[0],start[1],start[2],0.)
    res.EndPosition(end[0],end[1],end[2],0.)
    return res

def fill_vox(event_vox3d, start, end):

    distance  = start.distance(end)
    direction = start.direction(end)
    num_steps = distance / STEP_SIZE
    where = larcv.Point3D(start)
    edep = STEP_SIZE * DEDX
    vox = larcv.Voxel()
    step=0
    while step < num_steps:
        sys.stdout.write('    Filling voxel step %d/%d\r' % (step,num_steps))
        sys.stdout.flush()
        vox.Set( VOX_DEF.id(where), edep )
        event_vox3d.Add(vox)
        where += direction
        step += 1
    print('')

def fill_image(image2d, start3d, end3d, axis):

    start = larcv.Point2D()
    end   = larcv.Point2D()
    if axis == 0:
        start.x, start.y = (start3d.x, start3d.y)
        end.x,   end.y   = (end3d.x,   end3d.y  )
    if axis == 1:
        start.x, start.y = (start3d.z, start3d.x)
        end.x,   end.y   = (end3d.z,   end3d.x  )
    if axis == 2:
        start.x, start.y = (start3d.y, start3d.z)
        end.x,   end.y   = (end3d.y,   end3d.z  )

    distance  = start.distance(end)
    direction = start.direction(end) * STEP_SIZE
    #print direction.x,direction.y
    num_steps = distance / STEP_SIZE
    where = larcv.Point2D(start)
    edep = STEP_SIZE * DEDX
    step = 0
    while step < num_steps:
        sys.stdout.write('    Filling image plane %d step %d/%d\r' % (axis,step,num_steps))
        sys.stdout.flush()
        row = image2d.meta().row(where.y)
        col = image2d.meta().col(where.x)
        image2d.set_pixel(row,col,edep)
        where += direction
        step += 1
    print('')
#
# Configure
#
if 'debug' in sys.argv:
    MSG_LEVEL = larcv.msg.kDEBUG
if 'info' in sys.argv:
    MSG_LEVEL = larcv.msg.kINFO

o=larcv.IOManager(larcv.IOManager.kWRITE)
o.reset()
o.set_verbosity(MSG_LEVEL)
o.set_out_file(OUT_FNAME)
if not o.initialize():
    sys.exit(1)

ctr = 0
while ctr<NUM_EVENT:
    print('Event %d' % ctr)

    vtx_x = npr.uniform() * VOX_DEF.width()  + VOX_DEF.min_x()
    vtx_y = npr.uniform() * VOX_DEF.height() + VOX_DEF.min_y()
    vtx_z = npr.uniform() * VOX_DEF.depth()  + VOX_DEF.min_z()
    vtx = larcv.Point3D(vtx_x,vtx_y,vtx_z)
    print('Creation vtx: ({:g},{:g},{:g})'.format(vtx.x,vtx.y,vtx.z))

    end0_x = npr.uniform() * VOX_DEF.width()  + VOX_DEF.min_x()
    end0_y = npr.uniform() * VOX_DEF.height() + VOX_DEF.min_y()
    end0_z = npr.uniform() * VOX_DEF.depth()  + VOX_DEF.min_z()
    end0 = larcv.Point3D(end0_x,end0_y,end0_z)
    print('End point0: ({:g},{:g},{:g})'.format(end0.x,end0.y,end0.z))

    end1_x = npr.uniform() * VOX_DEF.width()  + VOX_DEF.min_x()
    end1_y = npr.uniform() * VOX_DEF.height() + VOX_DEF.min_y()
    end1_z = npr.uniform() * VOX_DEF.depth()  + VOX_DEF.min_z()
    end1 = larcv.Point3D(end1_x,end1_y,end1_z)
    print('End point1: ({:g},{:g},{:g})'.format(end1.x,end1.y,end1.z))

    mu = make_particle(13,   (vtx_x,vtx_y,vtx_z), (end0_x,end0_y,end0_z))
    p  = make_particle(2212, (vtx_x,vtx_y,vtx_z), (end1_x,end1_y,end1_z))

    # fill particle
    event_particle = o.get_data("particle","fake")
    event_particle.push_back(mu)
    event_particle.push_back(p)

    # fill voxel3d
    event_vox3d = o.get_data("voxel3d","fake")
    fill_vox(event_vox3d, vtx, end0)
    fill_vox(event_vox3d, vtx, end1)

    # fill projection image2d
    for plane in xrange(3):
        event_image2d = o.get_data("image2d","fake_plane%d" % plane)
        meta = make_meta(plane)
        img2d = larcv.Image2D(meta)
        fill_image(img2d, vtx, end0, axis=plane)
        fill_image(img2d, vtx, end1, axis=plane)
        event_image2d.Append(img2d)

    o.set_id(0,0,ctr)
    o.save_entry()
    ctr += 1
    
o.finalize()
