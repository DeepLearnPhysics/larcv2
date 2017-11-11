#ifndef __PARTICLEBBoxFROMCLUSTER_CXX__
#define __PARTICLEBBoxFROMCLUSTER_CXX__

#include "ParticleBBoxFromCluster.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
#include "larcv/core/DataFormat/EventParticle.h"
namespace larcv {

  static ParticleBBoxFromClusterProcessFactory __global_ParticleBBoxFromClusterProcessFactory__;

  ParticleBBoxFromCluster::ParticleBBoxFromCluster(const std::string name)
    : ProcessBase(name)
  {}

  void ParticleBBoxFromCluster::configure(const PSet& cfg)
  {
    _particle_producer  = cfg.get<std::string>("ParticleProducer" );
    _output_producer    = cfg.get<std::string>("OutputProducer"   );
    _cluster2d_producer = cfg.get<std::string>("Cluster2DProducer");
    _cluster3d_producer = cfg.get<std::string>("Cluster3DProducer");
    _voxel3d_min_thresh = cfg.get<float>("Voxel3DThreshold", 0.);
    _pixel2d_min_thresh = cfg.get<float>("Pixel2DThreshold", 0.);
  }

  void ParticleBBoxFromCluster::initialize()
  {}

  bool ParticleBBoxFromCluster::process(IOManager& mgr)
  {
    auto const& ev_particle = mgr.get_data<larcv::EventParticle>(_particle_producer);
    auto part_v = ev_particle.as_vector();

    if (!_cluster3d_producer.empty()) {
      auto const& ev_cluster3d = mgr.get_data<larcv::EventClusterVoxel3D>(_cluster3d_producer);
      auto const& meta = ev_cluster3d.meta();
      auto const& cluster3d_v  = ev_cluster3d.as_vector();
      // Check the dimensionality of inputs
      if (cluster3d_v.size() != (part_v.size() + 1)) {
        LARCV_CRITICAL() << "Input EventClusterVoxel3D has length " << cluster3d_v.size()
                         << " != length of EventParticle (" << part_v.size() << ") + 1!" << std::endl;
        throw larbys();
      }
      for (size_t i = 0; i < cluster3d_v.size(); ++i) {
        auto& part = part_v[i];
        auto const& cluster3d = cluster3d_v[i];

        double xmin, xmax, ymin, ymax, zmin, zmax;
        xmax = ymax = zmax = std::numeric_limits<double>::min();
        xmin = ymin = zmin = std::numeric_limits<double>::max();
        for (auto const& vox : cluster3d.as_vector()) {
          if (vox.value() <= _voxel3d_min_thresh) continue;
          auto const pt = meta.position(vox.id());
          if (xmax < pt.x) xmax = pt.x;
          if (ymax < pt.y) ymax = pt.y;
          if (zmax < pt.z) zmax = pt.z;
          if (xmin > pt.x) xmin = pt.x;
          if (ymin > pt.y) ymin = pt.y;
          if (zmin > pt.z) zmin = pt.z;
        }

        larcv::BBox3D bbox(xmin, ymin, zmin, xmax, ymax, zmax);
        part.boundingbox_3d(bbox);
      }
    }

    if (!_cluster2d_producer.empty()) {
      auto const& ev_cluster2d = mgr.get_data<larcv::EventClusterPixel2D>(_cluster2d_producer);
      auto const& cluster2d_vv = ev_cluster2d.as_vector();
      // Check the dimensionality of inputs
      for (auto const& cluster2d_v : cluster2d_vv) {
        if (cluster2d_v.size() != (part_v.size() + 1)) {
          LARCV_CRITICAL() << "Input ClusterPixel2D (plane " << cluster2d_v.meta().id()
                           << ") has length " << cluster2d_v.size()
                           << " != length of EventParticle (" << part_v.size() << ") + 1!" << std::endl;
          throw larbys();
        }
        for (size_t i = 0; i < cluster2d_v.size(); ++i) {
          auto& part = part_v[i];
          auto const& cluster2d = cluster2d_v.as_vector()[i];
          auto const& meta = cluster2d_v.meta();

          double xmin, xmax, ymin, ymax;
          xmax = ymax = std::numeric_limits<double>::min();
          xmin = ymin = std::numeric_limits<double>::max();
          for (auto const& vox : cluster2d.as_vector()) {
            if (vox.value() <= _pixel2d_min_thresh) continue;
            auto const pt = meta.position(vox.id());
            if (xmax < pt.x) xmax = pt.x;
            if (ymax < pt.y) ymax = pt.y;
            if (xmin > pt.x) xmin = pt.x;
            if (ymin > pt.y) ymin = pt.y;
          }

          larcv::BBox2D bbox(xmin, ymin, xmax, ymax);
          part.boundingbox_2d(bbox,meta.id());
        }
      }
    }

    // Store in the output
    auto& ev_output = mgr.get_data<larcv::EventParticle>(_output_producer);
    ev_output.emplace(std::move(part_v));

    return true;

  }

  void ParticleBBoxFromCluster::finalize()
  {}

}
#endif
