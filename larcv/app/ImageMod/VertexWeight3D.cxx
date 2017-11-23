#ifndef __VERTEXWEIGHT3D_CXX__
#define __VERTEXWEIGHT3D_CXX__

#include "VertexWeight3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

  static VertexWeight3DProcessFactory __global_VertexWeight3DProcessFactory__;

  VertexWeight3D::VertexWeight3D(const std::string name)
    : ProcessBase(name)
  {}

  void VertexWeight3D::configure(const PSet& cfg)
  {
    _tensor3d_producer  = cfg.get<std::string>("Tensor3DProducer");
    _primary_producer   = cfg.get<std::string>("PrimaryProducer");
    _secondary_producer = cfg.get<std::string>("SecondaryProducer");
    _primary_scale_factor = cfg.get<float>("PrimaryWeightScaleFactor");
    _secondary_scale_factor = cfg.get<float>("SecondaryWeightScaleFactor");
    _min_threshold = cfg.get<float>("WeightMinThreshold");
    _weight_neighbors = cfg.get<bool>("WeightNeighbors",true);
    _output_producer = cfg.get<std::string>("OutputProducer");
  }

  void VertexWeight3D::initialize()
  {}

  void VertexWeight3D::find_neighbors(const Voxel3DMeta& meta,
                                      const Voxel& vox,
                                      std::vector<VoxelID_t>& result) const
  {
    result.resize(26);
    size_t ctr=0;
    for(int xshift=-1; xshift<=1; ++xshift) {
      for(int yshift=-1; yshift<=1; ++yshift) {
        for(int zshift=-1; zshift<=1; ++zshift) {
          if(xshift != 0 || yshift !=0 || zshift !=0 ) {
            result[ctr] = meta.shift(vox.id(),xshift,yshift,zshift);
            ++ctr;
          }
        }
      }
    }
    /*
    result[0] = meta.shift(vox.id(), -1, 0, 0);
    result[1] = meta.shift(vox.id(), 1, 0, 0);
    result[2] = meta.shift(vox.id(), 0, -1, 0);
    result[3] = meta.shift(vox.id(), 0, 1, 0);
    result[4] = meta.shift(vox.id(), 0, 0, -1);
    result[5] = meta.shift(vox.id(), 0, 0, 1);
    */
  }

  bool VertexWeight3D::process(IOManager& mgr)
  {

    auto const& ev_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>(_tensor3d_producer);
    auto const& meta = ev_tensor3d.meta();

    larcv::VoxelSet result;

    //
    // Create "surrounding weight" voxels
    //
    //larcv::VoxelSet surrounding_vs;
    std::vector<VoxelID_t> neighbor_voxel_id;
    for (auto const& vox : ev_tensor3d.as_vector()) {
      result.emplace(vox.id(), _min_threshold, false);
      if (_weight_neighbors) {
        this->find_neighbors(meta, vox, neighbor_voxel_id);
        for (auto const& neighbor_id : neighbor_voxel_id) {
          if (neighbor_id == kINVALID_VOXELID) continue;
          auto const& orig_vox = ev_tensor3d.find(neighbor_id);
          if (orig_vox.id() != kINVALID_VOXELID) continue;
          result.emplace(neighbor_id, _min_threshold / 2., false);
        }
      }
    }

    //
    // Secondary weight voxels
    //
    auto const& ev_secondary_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>(_secondary_producer);
    if (ev_secondary_tensor3d.meta() != ev_tensor3d.meta()) {
      LARCV_CRITICAL() << "Voxel3DMeta from " << _tensor3d_producer << " (EventSparseTensor3D)..." << std::endl
                       << ev_tensor3d.meta().dump() << std::endl
                       << " differs from " << _secondary_producer << " (EventSparseTensor3D)..." << std::endl
                       << ev_secondary_tensor3d.meta().dump() << std::endl;
      throw larbys();
    }
    for (auto const& vox : ev_secondary_tensor3d.as_vector()) {
      float val = vox.value() * _secondary_scale_factor;
      if (val < _min_threshold) continue;
      result.emplace(vox.id(), val, false);
    }

    //
    // Vertex weight voxels
    //
    auto const& ev_primary_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>(_primary_producer);
    if (ev_primary_tensor3d.meta() != ev_tensor3d.meta()) {
      LARCV_CRITICAL() << "Voxel3DMeta from " << _tensor3d_producer << " (EventSparseTensor3D)..." << std::endl
                       << ev_tensor3d.meta().dump() << std::endl
                       << " differs from " << _primary_producer << " (EventSparseTensor3D)..." << std::endl
                       << ev_primary_tensor3d.meta().dump() << std::endl;
      throw larbys();
    }
    for (auto const& vox : ev_primary_tensor3d.as_vector()) {
      float val = vox.value() * _primary_scale_factor;
      if (val < _min_threshold) continue;
      result.emplace(vox.id(), val, false);
    }

    auto& ev_output = mgr.get_data<larcv::EventSparseTensor3D>(_output_producer);
    ev_output.emplace(std::move(result), meta);

    return true;

  }

  void VertexWeight3D::finalize()
  {}

}
#endif
