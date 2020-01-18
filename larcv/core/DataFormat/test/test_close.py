# Simple test to run on SparseTensor3D::close and SparseTensor3D::within
from larcv import larcv
import numpy as np

npts = 10
example1 = np.repeat(np.arange(npts, dtype=np.float32)[:, None], 3, axis=1)
example2 =  np.repeat(np.arange(npts, dtype=np.float32)[:, None], 3, axis=1)
example2[:, 2] = example2[:, 2] + 1

example1 = example1 + 0.5
example2 = example2 + 0.5
print(example1, example2)

#voxel_set = larcv.as_tensor3d(example)
voxel_set1 = larcv.VoxelSet()
voxel_set2 = larcv.VoxelSet()
meta = larcv.Voxel3DMeta()
meta.set(0, 0, 0, npts+1, npts+1, npts+1, npts+1, npts+1, npts+1)
for pt in example1:
    #print(pt, meta.id(pt[0], pt[1], pt[2]), meta.position(meta.id(pt[0], pt[1], pt[2])).x)
    voxel_set1.add(larcv.Voxel(meta.id(pt[0], pt[1], pt[2]), 0.))
for pt in example2:
    #print(pt, meta.id(pt[0], pt[1], pt[2]))
    voxel_set2.add(larcv.Voxel(meta.id(pt[0], pt[1], pt[2]), 0.))
tensor1 = larcv.SparseTensor3D()
tensor1.set(voxel_set1, meta)

tensor2 = larcv.SparseTensor3D()
tensor2.set(voxel_set2, meta)

v = tensor1.close(meta.id(example2[1, 0], example2[1, 1], example2[1, 2]), 1.5)
p = meta.position(v.id())
assert p.x == 1.5
assert p.y == 1.5
assert p.z == 1.5
print(v, p.x, p.y, p.z)

# Also test SparseTensor3D::within
b = tensor1.within(meta.id(example2[1, 0], example2[1, 1], example2[1, 2]), 1.5)
assert b == True
b2 = tensor1.within(meta.id(example2[1, 0], example2[1, 1], example2[1, 2]), 0.9)
assert b2 == False
