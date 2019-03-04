#ifndef __LABELER_CXX__
#define __LABELER_CXX__

#include "Labeler.h"

#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

  static LabelerProcessFactory __global_LabelerProcessFactory__;

  Labeler::Labeler(const std::string name)
    : ProcessBase(name)
  {}
    
  void Labeler::configure(const PSet& cfg)
  {
    _particle_producer  = cfg.get<std::string> ( "ParticleProducer" );
    _voxel3d_producer   = cfg.get<std::string> ( "Voxel3DProducer"  );
    _output_producer    = cfg.get<std::string> ( "OutputProducer"   );
  }

  void Labeler::initialize()
  {}

  bool Labeler::process(IOManager& mgr)
  {
    
    // Read in Particles
    auto const& ev_particle = mgr.get_data<larcv::EventParticle>(_particle_producer);
    auto const& particle_v  = ev_particle.as_vector  ();

    if (particle_v.size() == 0) {
      LARCV_CRITICAL() << "Input EventParticle not found by producer name "
                       << _particle_producer << std::endl;
       throw larbys();
    }

    // Read in Voxel3Ds
    auto const& ev_voxel3d = mgr.get_data<larcv::EventSparseTensor3D>(_voxel3d_producer);
    auto const& voxel3d_v  = ev_voxel3d.as_vector  ();

    if (voxel3d_v.size() == 0) {
      LARCV_CRITICAL() << "Input EventSparseTensor3D not found by producer name "
                       << _voxel3d_producer << std::endl;
       throw larbys();
    }

    // Create output Voxels lables
    auto& ev_voxel3d_output = mgr.get_data<larcv::EventSparseTensor3D>(_output_producer);
    ev_voxel3d_output.clear();


    // Set meta for output EventVoxel3D
    ev_voxel3d_output.meta(ev_voxel3d.meta());


    // First assign normal state to all voxels
    for(auto const& voxel : voxel3d_v) {
      double pos_x = ev_voxel3d.meta().pos_x(voxel.id());
      double pos_y = ev_voxel3d.meta().pos_y(voxel.id());
      double pos_z = ev_voxel3d.meta().pos_z(voxel.id());
      ev_voxel3d_output.emplace(pos_x, pos_y, pos_z, kNormal, false);
    }

    // Then replace voxels with labels we are interested in
    for(auto const& part : particle_v) {

      LARCV_DEBUG() << "Checking (Y,Z) : (" << part.y() << "," << part.z() << ")\n";

      if (part.creation_process() != "none") continue; // exclude non-primary particles 

      ev_voxel3d_output.emplace(part.position().x(),     part.position().y(),     part.position().z(),     kParticleStart, false);
      ev_voxel3d_output.emplace(part.end_position().x(), part.end_position().y(), part.end_position().z(), kParticleEnd,   false);

    }

    return true;
  }

  void Labeler::finalize()
  {}

}
#endif
