#ifndef __TENSOR2DFROMTENSOR3D_CXX__
#define __TENSOR2DFROMTENSOR3D_CXX__

#include "Tensor2DFromTensor3D.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

  static Tensor2DFromTensor3DProcessFactory
  __global_Tensor2DFromTensor3DProcessFactory__;

  Tensor2DFromTensor3D::Tensor2DFromTensor3D(const std::string name)
    : ProcessBase(name) {}

  void Tensor2DFromTensor3D::configure_labels(const PSet& cfg)
  {
    _tensor3d_producer_v.clear();
    _output_producer_v.clear();
    _tensor3d_producer_v = cfg.get<std::vector<std::string> >("Tensor3DProducerList", _tensor3d_producer_v);
    _output_producer_v    = cfg.get<std::vector<std::string> >("OutputProducerList", _output_producer_v);

    if (_tensor3d_producer_v.empty()) {
      auto tensor3d_producer = cfg.get<std::string>("Tensor3DProducer", "");
      auto output_producer    = cfg.get<std::string>("OutputProducer", "");
      if (!tensor3d_producer.empty()) {
        _tensor3d_producer_v.push_back(tensor3d_producer);
        if (output_producer.empty()) output_producer = tensor3d_producer + "_tensor2d";
        _output_producer_v.push_back(output_producer);
      }
    }

    if (_output_producer_v.empty()) {
      _output_producer_v.resize(_tensor3d_producer_v.size(), "");
    }
    else if (_output_producer_v.size() != _tensor3d_producer_v.size()) {
      LARCV_CRITICAL() << "Tensor3DProducer and OutputProducer must have the same array length!" << std::endl;
      throw larbys();
    }
  }

  void Tensor2DFromTensor3D::configure(const PSet& cfg)
  {
    configure_labels(cfg);

    _xy_v = cfg.get<std::vector<bool> >("XYList", _xy_v);
    if (_xy_v.empty()) {
      auto xy = cfg.get<bool>("XY", true);
      _xy_v.resize(_tensor3d_producer_v.size(), xy);
    } else if (_xy_v.size() != _tensor3d_producer_v.size()) {
      LARCV_CRITICAL() << "XYList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

    _yz_v = cfg.get<std::vector<bool> >("YZList", _yz_v);
    if (_yz_v.empty()) {
      auto yz = cfg.get<bool>("YZ", true);
      _yz_v.resize(_tensor3d_producer_v.size(), yz);
    } else if (_yz_v.size() != _tensor3d_producer_v.size()) {
      LARCV_CRITICAL() << "YZList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

    _zx_v = cfg.get<std::vector<bool> >("ZXList", _zx_v);
    if (_zx_v.empty()) {
      auto zx = cfg.get<bool>("ZX", true);
      _zx_v.resize(_tensor3d_producer_v.size(), zx);
    } else if (_zx_v.size() != _tensor3d_producer_v.size()) {
      LARCV_CRITICAL() << "ZXList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

    _pool_type_v = cfg.get<std::vector<unsigned short> >("PoolTypeList", _pool_type_v);
    if (_pool_type_v.empty()) {
      auto pool_type = cfg.get<unsigned short>("PoolType", (unsigned short)(kSumPool));
      _pool_type_v.resize(_tensor3d_producer_v.size(), pool_type);
    } else if (_pool_type_v.size() != _tensor3d_producer_v.size()) {
      LARCV_CRITICAL() << "PoolTypeList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }
  }

  void Tensor2DFromTensor3D::initialize() {}

  bool Tensor2DFromTensor3D::process(IOManager & mgr) {

    for (size_t producer_index = 0; producer_index < _tensor3d_producer_v.size(); ++producer_index) {
      auto const& tensor3d_producer = _tensor3d_producer_v[producer_index];
      auto const& output_producer = _output_producer_v[producer_index];
      auto const& xy = _xy_v[producer_index];
      auto const& yz = _yz_v[producer_index];
      auto const& zx = _zx_v[producer_index];
      auto pool_type = (PoolType_t)(_pool_type_v[producer_index]);

      auto const& ev_tensor3d =
        mgr.get_data<larcv::EventSparseTensor3D>(tensor3d_producer);

      if (!ev_tensor3d.meta().valid()) {
        LARCV_CRITICAL() << "Can not find sparse3d from producer "
                         << tensor3d_producer << std::endl;
        throw larbys();
      }

      auto const& meta3d = ev_tensor3d.meta();

      ProjectionID_t current_id = 0;

      ImageMeta meta_xy(meta3d.min_x(), meta3d.min_y(), meta3d.max_x(),
                        meta3d.max_y(), meta3d.num_voxel_x(), meta3d.num_voxel_y(),
                        (xy ? current_id : larcv::kINVALID_PROJECTIONID),
                        meta3d.unit());
      if (xy) current_id += 1;

      ImageMeta meta_yz(meta3d.min_y(), meta3d.min_z(), meta3d.max_y(),
                        meta3d.max_z(), meta3d.num_voxel_y(), meta3d.num_voxel_z(),
                        (yz ? current_id : larcv::kINVALID_PROJECTIONID),
                        meta3d.unit());
      if (yz) current_id += 1;

      ImageMeta meta_zx(meta3d.min_z(), meta3d.min_x(), meta3d.max_z(),
                        meta3d.max_x(), meta3d.num_voxel_z(), meta3d.num_voxel_x(),
                        (zx ? current_id : larcv::kINVALID_PROJECTIONID),
                        meta3d.unit());

      VoxelSet vs_xy;
      VoxelSet vs_yz;
      VoxelSet vs_zx;

      LARCV_DEBUG() << "Processing " << ev_tensor3d.as_vector().size()
                    << " of 3D voxels" << std::endl;

      for (auto const& vox : ev_tensor3d.as_vector()) {
        auto const pos = meta3d.position(vox.id());
        LARCV_DEBUG() << "Voxel " << vox.id() << " value " << vox.value()
                      << std::endl;
        if (xy) {
          LARCV_DEBUG() << "(x,y) = (" << pos.x << "," << pos.y << ")" << std::endl
                        << meta_xy.dump() << std::endl;
          size_t id = meta_xy.index(meta_xy.row(pos.y), meta_xy.col(pos.x));
          switch (pool_type) {
          case kSumPool:
            vs_xy.emplace(id, vox.value(), true);
            break;
          case kMaxPool:
            auto const& orig_vox = vs_xy.find(id);
            if (orig_vox.id() == kINVALID_VOXELID || orig_vox.value() < vox.value())
              vs_xy.emplace(id, vox.value(), false);
            break;
          }
        }
        if (yz) {
          LARCV_DEBUG() << "(y,z) = (" << pos.y << "," << pos.z << ")" << std::endl
                        << meta_yz.dump() << std::endl;
          size_t id = meta_yz.index(meta_yz.row(pos.z), meta_yz.col(pos.y));
          switch (pool_type) {
          case kSumPool:
            vs_yz.emplace(id, vox.value(), true);
            break;
          case kMaxPool:
            auto const& orig_vox = vs_yz.find(id);
            if (orig_vox.id() == kINVALID_VOXELID || orig_vox.value() < vox.value())
              vs_yz.emplace(id, vox.value(), false);
            break;
          }
        }
        if (zx) {
          LARCV_DEBUG() << "(z,x) = (" << pos.z << "," << pos.x << ")" << std::endl
                        << meta_zx.dump() << std::endl;
          size_t id = meta_zx.index(meta_zx.row(pos.x), meta_zx.col(pos.z));
          switch (pool_type) {
          case kSumPool:
            vs_zx.emplace(id, vox.value(), true);
            break;
          case kMaxPool:
            auto const& orig_vox = vs_zx.find(id);
            if (orig_vox.id() == kINVALID_VOXELID || orig_vox.value() < vox.value())
              vs_zx.emplace(id, vox.value(), false);
            break;
          }
        }
      }

      auto& out_tensor2d =
        mgr.get_data<larcv::EventSparseTensor2D>(output_producer);

      if (xy) {
        out_tensor2d.emplace(std::move(vs_xy), std::move(meta_xy));
      }
      if (yz) {
        out_tensor2d.emplace(std::move(vs_yz), std::move(meta_yz));
      }
      if (zx) {
        out_tensor2d.emplace(std::move(vs_zx), std::move(meta_zx));
      }
    }
    return true;
  }

  void Tensor2DFromTensor3D::finalize() {}
}
#endif
