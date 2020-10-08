#ifndef __SUPERAG4HITSEGMENT_CXX__
#define __SUPERAG4HITSEGMENT_CXX__

#include <numeric>

#include "SuperaG4HitSegment.h"
#include "GenRandom.h"
#include "geometry.h"
#include "raybox.h"
namespace larcv {

  static SuperaG4HitSegmentProcessFactory __global_SuperaG4HitSegmentProcessFactory__;

  SuperaG4HitSegment::SuperaG4HitSegment(const std::string name) : SuperaBase(name)
  { }

  void SuperaG4HitSegment::configure(const PSet& cfg)
  {
    SuperaBase::configure(cfg);
    _sparsetensor3d_producer=cfg.get<std::string>("HitTensorProducer");
    _particle_producer =cfg.get<std::string>("ParticleProducer");
  }

  void SuperaG4HitSegment::initialize()
  {
    SuperaBase::initialize();
  }

  bool SuperaG4HitSegment::process(IOManager& mgr)
  {
    SuperaBase::process(mgr);

    auto evt = this->GetEvent();

    auto ev_hittensor = (EventSparseTensor3D*)(mgr.get_data("sparse3d", _sparsetensor3d_producer));
    auto ev_particles = (EventParticle*)(mgr.get_data("particle",_particle_producer));

    auto meta = ev_hittensor->meta();
    larcv::AABBox<double> box(meta);

    std::vector<larcv::Particle> particles;
    particles.reserve(evt->Trajectories.size());
    for(auto const& traj : evt->Trajectories) {
      larcv::Particle part = this->MakeParticle(traj, box);
      part.id(ev_particles->as_vector().size());
      LARCV_INFO() << "Track ID " << part.track_id() << " PDG " << part.pdg_code() << std::endl
                   << "     full extent: "
                        << "[" << part.position().x() << "," << part.position().y() << "," << part.position().z() << "] => "
                        << "[" << part.end_position().x() << "," << part.end_position().y() << "," << part.end_position().z() << "]" << std::endl
                   << "     inside bounding box:"
                        << "[" << part.first_step().x() << "," << part.first_step().y() << "," << part.first_step().z() << "] => "
                        << "[" << part.last_step().x() << "," << part.last_step().y() << "," << part.last_step().z() << "]"
                   << std::endl;
      particles.emplace_back(std::move(part));
      break;
    }

    larcv::VoxelSet voxelSet;
    for (auto const & detPair : evt->SegmentDetectors)
    {
      LARCV_DEBUG() << "Sensitive detector: " << detPair.first << std::endl;
      LARCV_DEBUG() << "  There are " << detPair.second.size() << " hits" << std::endl;
      for (std::size_t hitIdx = 0; hitIdx < detPair.second.size(); hitIdx++)
      {
        LARCV_DEBUG() << "   hit number: " << hitIdx << std::endl;
        auto const & hitSeg = detPair.second[hitIdx];
        auto newVoxels = MakeVoxels(hitSeg, ev_hittensor->meta(), particles);
        LARCV_DEBUG() << "    made " << newVoxels.size() << " voxels " << std::endl;
        for (std::size_t voxIdx = 0; voxIdx < newVoxels.size(); voxIdx++)
          voxelSet.emplace(std::move(newVoxels[voxIdx]), true);
      }
    }

    LARCV_DEBUG() << "Made " << particles.size() << " true particles" << std::endl;
    LARCV_DEBUG() << "Made " << voxelSet.size() << " voxels, with " << voxelSet.sum() << " total" << std::endl;
    ev_particles->emplace(std::move(particles));
    ev_hittensor->emplace(std::move(voxelSet), ev_hittensor->meta());
    LARCV_DEBUG() << "Done with event." << std::endl;

    return true;
  }

  void SuperaG4HitSegment::finalize()
  {}

  larcv::Particle SuperaG4HitSegment::MakeParticle(const TG4Trajectory& traj, const larcv::AABBox<double> & bbox)
  {
    larcv::Particle res;
    res.track_id(traj.GetTrackId());
    res.pdg_code(traj.GetPDGCode());
    auto const& mom = traj.GetInitialMomentum();
    res.momentum(mom.Px(),mom.Py(),mom.Pz());
    auto const& start = traj.Points.front().GetPosition();
    res.position(start.X()/10.,start.Y()/10.,start.Z()/10.,start.T());
    auto const& end = traj.Points.back().GetPosition();
    res.end_position(end.X()/10.,end.Y()/10.,end.Z()/10.,end.T());

    // this will be the first point the trajectory crosses into the bounding box
    const auto FindVertex = [&](int step) -> std::unique_ptr<larcv::Vertex>
    {
      std::unique_ptr<larcv::Vertex> vtx;

      LARCV_DEBUG() << "Finding " << (step > 0 ? "entry" : "exit") << " point of trajectory " << traj.GetTrackId()
                    << " " << (step > 0 ? "into" : "out of") << " bounding box:" << std::endl;

      // note that when step < 0, "idx += step" will eventually cause an integer underflow because idx's type (size_t) is unsigned.
      // this is ok because traj.Points.size() will always be < the maximum integer after that underflow
      // and the loop will be cut off at the right place anyway.
      for (std::size_t idx = (step > 0) ? step : (traj.Points.size()-1 + step); idx < traj.Points.size(); idx += step)
      {
        TLorentzVector trajP1 = traj.Points[idx - step].GetPosition();
        TLorentzVector trajP2 = traj.Points[idx].GetPosition();
        trajP1.SetRho(trajP1.Mag() * 0.1);  // convert units to cm
        trajP2.SetRho(trajP2.Mag() * 0.1);  // convert units to cm

        LARCV_DEBUG() << "  Considering step (" << trajP1.X() << "," << trajP1.Y() << "," << trajP1.Z() << ")"
                      << " -> " << trajP2.X() << "," << trajP2.Y() << "," << trajP2.Z() << ")" << std::endl;

        larcv::Vec3d p1, p2;
        if (!Intersections(bbox, trajP1.Vect(), trajP2.Vect(),p1, p2))
        {
          LARCV_DEBUG() << "   --> does not intersect bounding box." << std::endl;
          continue;
        }

        LARCV_DEBUG() << "   --> intersects bounding box at (" << p1.x << "," << p1.y << "," << p1.z << ")" << std::endl;
        vtx.reset(new larcv::Vertex(p1.x, p1.y, p1.z,
                                    trajP1.T() + (p2 - p1).length() / (trajP2 - trajP1).Vect().Mag() * (trajP2.T() - trajP1.T())) );
        break;
      }
      return vtx;
    };

    auto entry = FindVertex(1);  // entry point -- step forward
    if (entry)
      res.first_step(*entry);

    auto exit = FindVertex(-1);  // exit point -- step backwards
    if (exit)
      res.last_step(*exit);

    res.energy_init(mom.E());
    res.parent_track_id(traj.GetParentId());

    double distTraveled = 0;
    for (std::size_t idx = 1; idx < traj.Points.size(); idx++)
      distTraveled += (traj.Points[idx].GetPosition().Vect() - traj.Points[idx-1].GetPosition().Vect()).Mag();
    res.distance_travel(distTraveled);

    return res;
  }

  void SuperaG4HitSegment::FluctuateEnergy(std::vector<Voxel> &)
  {
    // just placeholder for now.
    return;
  }


  std::vector<larcv::Voxel> SuperaG4HitSegment::MakeVoxels(const ::TG4HitSegment &hitSegment,
                                                           const larcv::Voxel3DMeta &meta,
                                                           std::vector<larcv::Particle> &particles)
  {
    std::vector<larcv::Voxel> voxels;
    larcv::AABBox<double> box(meta);

    const double epsilon = 1.e-3;
    double dist_travel = 0.;
    double energy_deposit = 0.;
    double smallest_side = std::min(meta.size_voxel_x(),std::min(meta.size_voxel_y(),meta.size_voxel_z()));
    LARCV_INFO() <<"World: " << box.bounds[0] << " => " << box.bounds[1] << std::endl;

    TVector3 start = hitSegment.GetStart().Vect();
    TVector3 end = hitSegment.GetStop().Vect();
    start *= 0.1;  // convert unit to cm
    end *= 0.1;

    LARCV_INFO() << "Voxelizing TG4HitSegment for primary " << hitSegment.GetPrimaryId()
                 << " from (" << start.x() << "," << start.y() << "," << start.z() << ")"
                 << " to (" << end.x() << "," << end.y() << "," << end.z() << ")"
                 << std::endl;

    larcv::Vec3d pt0, pt1;
    char crossings = Intersections(box, start, end, pt0, pt1);

    if(crossings == 0) {
      LARCV_INFO() << "No crossing point found..." << std::endl;
      return voxels;
    }

    LARCV_DEBUG() << "   Intersects with bounding box at"
                  << " (" << pt0.x << "," << pt0.y << "," << pt0.z << ")"
                  << " and (" << pt1.x << "," << pt1.y << "," << pt1.z << ")"
                  << std::endl;

    larcv::Vec3d dir = pt1 - pt0;
    double length = dir.length();
    dir.normalize();
    larcv::Ray<double> ray(pt0, dir);

    int trackId = hitSegment.GetPrimaryId();
    auto& particle = *std::find_if(particles.begin(), particles.end(),
                                   [=](const larcv::Particle & p) { return p.track_id() == static_cast<unsigned int>(trackId); });

    voxels.reserve(voxels.size() + (size_t)(length / smallest_side));
    double t0, t1, dist_section;
    dist_section = 0.;
    size_t nx, ny, nz;
    t0=t1=0.;
    //size_t ctr=0;
    while(true) {
      //ctr += 1;
      //if(ctr>=10) break;
      // define the inspection box
      Vec3d pt = pt0 + dir * (t1 + epsilon);
      LARCV_DEBUG() << "    New point: " << pt << std::endl;
      auto vox_id = meta.id((double)(pt.x), (double)(pt.y), (double)(pt.z));
      if(vox_id==larcv::kINVALID_VOXELID) break;
      meta.id_to_xyz_index(vox_id, nx, ny, nz);
      box.bounds[0].x = meta.min_x() + nx * meta.size_voxel_x();
      box.bounds[0].y = meta.min_y() + ny * meta.size_voxel_y();
      box.bounds[0].z = meta.min_z() + nz * meta.size_voxel_z();
      box.bounds[1].x = box.bounds[0].x + meta.size_voxel_x();
      box.bounds[1].y = box.bounds[0].y + meta.size_voxel_y();
      box.bounds[1].z = box.bounds[0].z + meta.size_voxel_z();
      LARCV_DEBUG() << "    Inspecting a voxel id " << vox_id << " ... " << box.bounds[0] << " => " << box.bounds[1] << std::endl;
      auto cross = box.intersect(ray,t0,t1);

      // no crossing
      if(cross==0) {
        LARCV_ERROR() << "      No crossing (not expected) ... breaking" << std::endl;
        break;
      }
      double dx=0;
      if(cross==1) {
        LARCV_DEBUG() << "      One crossing: " << pt0 + dir * t1 << std::endl;
        dx = std::min(t1,length);
      }else {
        LARCV_DEBUG() << "      Two crossing" << pt0 + dir * t0 << " => " << pt0 + dir * t1 << std::endl;
        if(t1>length) dx = length - t0;
        else dx = t1 - t0;
      }
      /*
      res_pt[0] = (nx+0.5) * meta.size_voxel_x();
      res_pt[1] = (ny+0.5) * meta.size_voxel_y();
      res_pt[2] = (nz+0.5) * meta.size_voxel_z();
      res_pt[3] = dx;
      res.push_back(res_pt);
      */
      double energyInVoxel = dx / length * hitSegment.GetEnergyDeposit();
      voxels.emplace_back(vox_id, energyInVoxel);
      dist_travel += dx;
      dist_section += dx;
      energy_deposit += energyInVoxel;
      //LARCV_DEBUG() << "      Registering voxel id " << vox_id << " at distance fraction " << t1/length << std::endl;
      LARCV_DEBUG() << "      Registering voxel id " << vox_id << " t1 =" << t1 << " (total length = " << length << ")" << std::endl;
      if(t1>length) {
        LARCV_DEBUG() << "      Reached the segment end (t1 = " << t1 << " fractional length " << t1/length << ") ... breaking" << std::endl;
        break;
      }

      LARCV_DEBUG() << "      Updated t1 = " << t1 << " (fractional length " << t1/length << ")" << std::endl;
    }

    particle.energy_deposit(particle.energy_deposit() + energy_deposit);
    particle.num_voxels(particle.num_voxels() + voxels.size());

    FluctuateEnergy(voxels);

    return voxels;
  }

  template <typename T>
  char SuperaG4HitSegment::Intersections(const AABBox<T> &bbox,
                                         const TVector3 &startPoint,
                                         const TVector3 &stopPoint,
                                         Vec3<T> &entryPoint,
                                         Vec3<T> &exitPoint) const
  {
    TVector3 displVec = (stopPoint - startPoint);
    TVector3 dir = displVec.Unit();
    larcv::Ray<T> ray(startPoint, dir);

    bool startContained = bbox.contain(startPoint);
    bool stopContained = bbox.contain(stopPoint);

    if (startContained)
      entryPoint = startPoint;
    if (stopContained)
      exitPoint = stopPoint;

    if(!startContained || !stopContained)
    {
      double t0, t1;
      int cross = bbox.intersect(ray, t0, t1);
      if (cross > 0)
      {
        const T epsilon = 0.0001;
        if (!startContained)
          entryPoint = startPoint + (t0 + epsilon) * dir;

        if (!stopContained)
          exitPoint = startPoint + (t1 - epsilon) * dir;
      }

      LARCV_DEBUG() << "Number of crossings:" << cross
                    << " for bounding box and ray between "
                    << "(" << startPoint.x() << "," << startPoint.y() << "," << startPoint.z() << ")"
                    << " and (" <<  stopPoint.x() << "," << stopPoint.y() << "," << stopPoint.z() << ")" << std::endl;
      LARCV_DEBUG() << "Start point contained?: " << startContained << ".  Stop point contained?: " << stopContained << std::endl;

      if (cross == 1 && startContained == stopContained)
      {
        LARCV_ERROR() << "Unexpected number of crossings (" << cross << ")"
                      << " for bounding box and ray between "
                      << "(" << startPoint.x() << "," << startPoint.y() << "," << startPoint.z() << ")"
                      << " and (" <<  stopPoint.x() << "," << stopPoint.y() << "," << stopPoint.z() << ")" << std::endl;
        LARCV_ERROR() << "Start point contained?: " << startContained << ".  Stop point contained?: " << stopContained << std::endl;
      }

      return cross;
    }

    return 2;
  }

  // instantiate the template for the type(s) we care about
  template char SuperaG4HitSegment::Intersections(const AABBox<double> &bbox,
                                                  const TVector3 &startPoint,
                                                  const TVector3 &stopPoint,
                                                  Vec3d &entryPoint,
                                                  Vec3d &exitPoint) const;

}

#endif
