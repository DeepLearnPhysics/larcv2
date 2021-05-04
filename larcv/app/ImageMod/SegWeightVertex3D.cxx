#ifndef __SegWeightVertex3D_CXX__
#define __SegWeightVertex3D_CXX__

#include "SegWeightVertex3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/Vertex.h"

namespace larcv {

  static SegWeightVertex3DProcessFactory __global_SegWeightVertex3DProcessFactory__;

  SegWeightVertex3D::SegWeightVertex3D(const std::string name)
    : ProcessBase(name)
  {}

  void SegWeightVertex3D::configure(const PSet& cfg)
  {
    _tensor3d_producer  = cfg.get<std::string>("Tensor3DProducer");
    _particle_producer   = cfg.get<std::string>("ParticleProducer");
    _prob_nonzero = cfg.get<float>("ProbNonZero",0.0);
    _prob_neighbors = cfg.get<float>("ProbNeighbors",-1.);
    _output_producer = cfg.get<std::string>("OutputProducer");
    _distance_neighbors = cfg.get<int>("DistanceNeighbors", 1);

    if(_distance_neighbors < 1) {
        LARCV_CRITICAL() << "DistanceNeighbors must be >= 1" << std::endl;
        throw larbys();
    }
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

  void SegWeightVertex3D::initialize()
  {}

  void SegWeightVertex3D::find_neighbors(const Voxel3DMeta& meta, 
                                           const Voxel& vox,
                                           std::vector<VoxelID_t>& result) const
  {
    result.resize(pow(_distance_neighbors * 2 + 1, 3)-1);
    size_t ctr=0;
    for(int xshift=-_distance_neighbors; xshift<=_distance_neighbors; ++xshift) {
      for(int yshift=-_distance_neighbors; yshift<=_distance_neighbors; ++yshift) {
        for(int zshift=-_distance_neighbors; zshift<=_distance_neighbors; ++zshift) {
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

  bool SegWeightVertex3D::process(IOManager& mgr)
  {
    auto const& ev_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>(_tensor3d_producer);
    auto const& meta = ev_tensor3d.meta();

    // first figure out a list of vertices
    auto const& ev_particle = mgr.get_data<larcv::EventParticle>(_particle_producer);
    auto particle_v = ev_particle.as_vector();
    std::vector<larcv::Vertex> vtx_v;
    for (size_t part_idx=0; part_idx<particle_v.size(); ++part_idx) {
	//LARCV_INFO() << particle_v[part_idx].id() << " " << particle_v[part_idx].parent_id() << std::endl;
        if ((particle_v[part_idx].parent_id() == particle_v[part_idx].id()) && (particle_v[part_idx].id() == particle_v[part_idx].group_id())) {
            // This is a primary particle
            vtx_v.push_back(particle_v[part_idx].position());
        }
    }

    larcv::VoxelSet result;

    //
    // Create "default weight" voxels
    //
    for (auto const& vox : ev_tensor3d.as_vector()) {
      result.emplace(vox.id(), _prob_nonzero, false);
    }

    //
    // Create "near vertex weight" voxels
    //
    std::vector<VoxelID_t> neighbor_voxel_id;
    int count_neighbors = 0;
    for (auto const& vtx : vtx_v) {
	LARCV_INFO() << "Vertex " << vtx.x() << " " << vtx.y() << " " << vtx.z() << std::endl;
        this->find_neighbors(meta, meta.id(vtx.as_point3d()), neighbor_voxel_id);
        for (auto const& neighbor_id : neighbor_voxel_id) {
          if (neighbor_id == kINVALID_VOXELID) continue;
          auto const& orig_vox = ev_tensor3d.find(neighbor_id);
          if (orig_vox.id() != kINVALID_VOXELID) continue;
          result.emplace(neighbor_id, _prob_neighbors, false);
          ++count_neighbors;
        }
    }
    LARCV_INFO() << "Modified " << count_neighbors << " neighbor voxels" << std::endl;


    auto& ev_output = mgr.get_data<larcv::EventSparseTensor3D>(_output_producer);
    ev_output.emplace(std::move(result), meta);

    return true;

  }

  void SegWeightVertex3D::finalize()
  {}

}
#endif
