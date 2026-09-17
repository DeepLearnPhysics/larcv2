"""Exercise the LArCV features required by downstream visualization tools."""

import tempfile
from pathlib import Path

import numpy as np

from larcv import larcv


meta = larcv.Voxel3DMeta()
meta.set(0.0, 0.0, 0.0, 2.0, 2.0, 2.0, 2, 2, 2)

tensor = larcv.SparseTensor3D()
tensor.meta(meta)
tensor.emplace(0, 4.5, False)

points = np.empty((1, 4), dtype=np.float32)
larcv.fill_3d_pcloud(tensor, points)
assert points.shape == (1, 4)

with tempfile.TemporaryDirectory() as directory:
    path = str(Path(directory) / "smoke.root")

    writer = larcv.IOManager(larcv.IOManager.kWRITE)
    writer.set_out_file(path)
    assert writer.initialize()
    event = writer.get_data("sparse3d", "test")
    event.meta(meta)
    event.emplace(0, 4.5, False)
    writer.set_id(1, 2, 3)
    assert writer.save_entry()
    writer.finalize()

    reader = larcv.IOManager(larcv.IOManager.kREAD)
    reader.add_in_file(path)
    assert reader.initialize()
    assert reader.get_n_entries() == 1
    assert reader.read_entry(0)
    loaded = reader.get_data("sparse3d", "test")
    assert loaded.as_vector().size() == 1
    assert abs(loaded.as_vector().at(0).value() - 4.5) < 1.0e-6
    reader.finalize()

print("ROOT/LArCV I/O and NumPy bridge: OK")
