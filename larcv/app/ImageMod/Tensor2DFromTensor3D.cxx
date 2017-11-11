#ifndef __TENSOR2DFROMTENSOR3D_CXX__
#define __TENSOR2DFROMTENSOR3D_CXX__

#include "Tensor2DFromTensor3D.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

  static Tensor2DFromTensor3DProcessFactory __global_Tensor2DFromTensor3DProcessFactory__;

  Tensor2DFromTensor3D::Tensor2DFromTensor3D(const std::string name)
    : ProcessBase(name)
  {}
    
  void Tensor2DFromTensor3D::configure(const PSet& cfg)
  {
    _tensor3d_producer = cfg.get<std::string>("Tensor3DProducer");
    _output_producer = _tensor3d_producer + "_xyz";
    _output_producer = cfg.get<std::string>("OutputProducer", _output_producer);
    _xy = cfg.get<int>("XY", 1);
    _yz = cfg.get<int>("YZ", 1);
    _zx = cfg.get<int>("ZX", 1);
  }

  void Tensor2DFromTensor3D::initialize()
  {}

  bool Tensor2DFromTensor3D::process(IOManager& mgr)
  {
    auto const& ev_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>(_tensor3d_producer);
    auto const& meta3d = ev_tensor3d.meta();

    ProjectionID_t current_id = 0;

    ImageMeta meta_xy(meta3d.min_x(), meta3d.min_y(), meta3d.max_x(), meta3d.max_y(),
                      meta3d.num_voxel_x(), meta3d.num_voxel_y(),
                      (_xy ? current_id : larcv::kINVALID_PROJECTIONID), meta3d.unit());
    if (_xy) current_id += 1;

    ImageMeta meta_yz(meta3d.min_y(), meta3d.min_z(), meta3d.max_y(), meta3d.max_z(),
                      meta3d.num_voxel_y(), meta3d.num_voxel_z(),
                      (_yz ? current_id : larcv::kINVALID_PROJECTIONID),  meta3d.unit());
    if (_yz) current_id += 1;

    ImageMeta meta_zx(meta3d.min_z(), meta3d.min_x(), meta3d.max_z(), meta3d.max_x(),
                      meta3d.num_voxel_z(), meta3d.num_voxel_x(),
                      (_zx ? current_id : larcv::kINVALID_PROJECTIONID), meta3d.unit());

    VoxelSet vs_xy;
    VoxelSet vs_yz;
    VoxelSet vs_zx;

    LARCV_DEBUG() << "Processing " << ev_tensor3d.as_vector().size()
                  << " of 3D voxels" << std::endl;

    for (auto const& vox : ev_tensor3d.as_vector()) {
      auto const pos = meta3d.position(vox.id());
      LARCV_DEBUG() << "Voxel " << vox.id() << " value " << vox.value() << std::endl;
      if (_xy) {
        LARCV_DEBUG() << "(x,y) = (" << pos.x << "," << pos.y << ")" << std::endl
                      << meta_xy.dump() << std::endl;
        vs_xy.emplace(meta_xy.index(meta_xy.row(pos.y), meta_xy.col(pos.x)), vox.value(), true);
      }
      if (_yz) {
        LARCV_DEBUG() << "(y,z) = (" << pos.y << "," << pos.z << ")" << std::endl
                      << meta_yz.dump() << std::endl;
        vs_yz.emplace(meta_yz.index(meta_yz.row(pos.z), meta_yz.col(pos.y)), vox.value(), true);
      }
      if (_zx) {
        LARCV_DEBUG() << "(z,x) = (" << pos.z << "," << pos.x << ")" << std::endl
                      << meta_zx.dump() << std::endl;
        vs_zx.emplace(meta_zx.index(meta_zx.row(pos.x), meta_zx.col(pos.z)), vox.value(), true);
      }
    }

    auto& out_tensor2d = mgr.get_data<larcv::EventSparseTensor2D>(_output_producer);

    if (_xy) { out_tensor2d.emplace(std::move(vs_xy),std::move(meta_xy)); }
    if (_yz) { out_tensor2d.emplace(std::move(vs_yz),std::move(meta_yz)); }
    if (_zx) { out_tensor2d.emplace(std::move(vs_zx),std::move(meta_zx)); }

    return true;
  }

  void Tensor2DFromTensor3D::finalize()
  {}

}
#endif
