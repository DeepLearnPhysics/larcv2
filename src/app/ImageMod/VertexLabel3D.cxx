#ifndef __VertexLabel3D_CXX__
#define __VertexLabel3D_CXX__

#include "VertexLabel3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

  static VertexLabel3DProcessFactory __global_VertexLabel3DProcessFactory__;

  VertexLabel3D::VertexLabel3D(const std::string name)
    : ProcessBase(name)
  {}

  void VertexLabel3D::configure(const PSet& cfg)
  {
    _tensor3d_producer  = cfg.get<std::string>("Tensor3DProducer");
    _primary_producer   = cfg.get<std::string>("PrimaryProducer");
    _prob_nonzero = cfg.get<float>("ProbNonZero",0.0);
    _prob_neighbors = cfg.get<float>("ProbNeighbors",-1.);
    _output_producer = cfg.get<std::string>("OutputProducer");

    // make sure probability requested is < 1.0
    if(_prob_nonzero < 0. || _prob_nonzero > 1.){
      LARCV_CRITICAL() << "ProbNonZero must be between 0.0 => 1.0" << std::endl;
      throw larbys();
    }
    if(_prob_neighbors >1.) {
      LARCV_CRITICAL() << "ProbNeighbors must be <= 1.0" << std::endl;
      throw larbys();
    }
  }

  void VertexLabel3D::initialize()
  {}

  void VertexLabel3D::find_neighbors(const Voxel3DMeta& meta, 
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

  bool VertexLabel3D::process(IOManager& mgr)
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
      result.emplace(vox.id(), _prob_nonzero, false);
      if (_prob_neighbors>0.) {
        this->find_neighbors(meta, vox, neighbor_voxel_id);
        for (auto const& neighbor_id : neighbor_voxel_id) {
          if (neighbor_id == kINVALID_VOXELID) continue;
          auto const& orig_vox = ev_tensor3d.find(neighbor_id);
          if (orig_vox.id() != kINVALID_VOXELID) continue;
          result.emplace(neighbor_id, _prob_neighbors, false);
        }
      }
    }

    //
    // Vertex weight voxels
    //
    auto const& ev_primary_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>(_primary_producer);
    if (ev_primary_tensor3d.meta() != ev_tensor3d.meta()) {
      LARCV_CRITICAL() << "Voxel3DMeta from " << _tensor3d_producer << " (EventSparseTensor3D) "
                       << " differs from " << _primary_producer << " (EventSparseTensor3D)" << std::endl;
      throw larbys();
    }
    for (auto const& vox : ev_primary_tensor3d.as_vector()) {
      float val = vox.value();
      if(val <0. || val > 1.) {
        LARCV_CRITICAL() << "Found invalid value " << val << " for the vertex label!" << std::endl;
        throw larbys();
      }
      if (val < _prob_nonzero) continue;
      result.emplace(vox.id(), val, false);
    }

    auto& ev_output = mgr.get_data<larcv::EventSparseTensor3D>(_output_producer);
    ev_output.emplace(std::move(result), meta);

    return true;

  }

  void VertexLabel3D::finalize()
  {}

}
#endif
