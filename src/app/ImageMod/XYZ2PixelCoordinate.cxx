#ifndef __XYZ2PIXELCOORDINATE_CXX__
#define __XYZ2PIXELCOORDINATE_CXX__

#include "XYZ2PixelCoordinate.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
#include "larcv/core/DataFormat/BBox.h"
namespace larcv {

  static XYZ2PixelCoordinateProcessFactory __global_XYZ2PixelCoordinateProcessFactory__;

  XYZ2PixelCoordinate::XYZ2PixelCoordinate(const std::string name)
    : ProcessBase(name)
  {}
    
  void XYZ2PixelCoordinate::configure(const PSet& cfg)
  {
    _tensor3d_producer = cfg.get<std::string>("Tensor3DProducer");
    _particle_producer = cfg.get<std::string>("ParticleProducer");
    _output_producer   = cfg.get<std::string>("OutputProducer");
  }

  void XYZ2PixelCoordinate::initialize()
  {}

  bool XYZ2PixelCoordinate::process(IOManager& mgr)
  {
    auto const& event_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>(_tensor3d_producer);
    auto const& event_particle = mgr.get_data<larcv::EventParticle>(_particle_producer);

    auto const& meta = event_tensor3d.meta();
    auto const& origin = meta.origin();
    std::vector<larcv::Particle> part_v;
    part_v.resize(event_particle.as_vector().size());
    for(size_t idx = 0; idx<event_particle.as_vector().size(); ++idx) {
      auto const& p = event_particle.as_vector()[idx];
      auto& p2 = part_v[idx];

      p2 = p;
      // Nulify bounding box
      p2.boundingbox_3d(larcv::Voxel3DMeta());
      std::vector<larcv::BBox2D> bb2d_v;
      p2.boundingbox_2d(bb2d_v);
      // Correct creation point
      auto pt = p2.position();
      p2.position( (pt.x() - origin.x) / meta.size_voxel_x(),
		   (pt.y() - origin.y) / meta.size_voxel_y(),
		   (pt.z() - origin.z) / meta.size_voxel_z(),
		   pt.t() );
      
      pt = p2.end_position();
      p2.end_position( (pt.x() - origin.x) / meta.size_voxel_x(),
		       (pt.y() - origin.y) / meta.size_voxel_y(),
		       (pt.z() - origin.z) / meta.size_voxel_z(),
		       pt.t() );
      
      pt = p2.first_step();
      p2.first_step( (pt.x() - origin.x) / meta.size_voxel_x(),
		     (pt.y() - origin.y) / meta.size_voxel_y(),
		     (pt.z() - origin.z) / meta.size_voxel_z(),
		     pt.t() );

      pt = p2.last_step();
      p2.last_step( (pt.x() - origin.x) / meta.size_voxel_x(),
		    (pt.y() - origin.y) / meta.size_voxel_y(),
		    (pt.z() - origin.z) / meta.size_voxel_z(),
		    pt.t() );

      pt = p2.parent_position();
      p2.parent_position( (pt.x() - origin.x) / meta.size_voxel_x(),
			  (pt.y() - origin.y) / meta.size_voxel_y(),
			  (pt.z() - origin.z) / meta.size_voxel_z(),
			  pt.t() );

      pt = p2.ancestor_position();
      p2.ancestor_position( (pt.x() - origin.x) / meta.size_voxel_x(),
			    (pt.y() - origin.y) / meta.size_voxel_y(),
			    (pt.z() - origin.z) / meta.size_voxel_z(),
			    pt.t() );

    }

    auto& output_particle = mgr.get_data<larcv::EventParticle>(_output_producer);
    output_particle.emplace(std::move(part_v));
    return true;
  }

  void XYZ2PixelCoordinate::finalize()
  {}

}
#endif
