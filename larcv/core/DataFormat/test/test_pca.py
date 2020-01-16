from larcv import larcv
import numpy as np

npts = 10
example = np.repeat(np.arange(npts, dtype=np.float32)[:, None], 3, axis=1)
#voxel_set = larcv.as_tensor3d(example)
voxel_set = larcv.VoxelSet()
meta = larcv.Voxel3DMeta()
meta.set(0, 0, 0, npts, npts, npts, npts, npts, npts)
for pt in example:
    voxel_set.add(larcv.Voxel(meta.id(pt[0], pt[1], pt[2]), 0.))
tensor = larcv.SparseTensor3D()
tensor.set(voxel_set, meta)
pca = tensor.pca()
print(pca.x, pca.y, pca.z)
