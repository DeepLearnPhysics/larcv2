#ifndef __SUPERAG4HITSEGMENT_CXX__
#define __SUPERAG4HITSEGMENT_CXX__

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

    std::vector<larcv::Particle> particles;
    particles.reserve(evt->Trajectories.size());
    for(auto const& traj : evt->Trajectories) {
      larcv::Particle part = this->MakeParticle(traj);
      part.id(ev_particles->as_vector().size());
      LARCV_INFO() << "Track ID " << part.track_id() << " PDG " << part.pdg_code() << " ... "
      << "[" << part.first_step().x() << "," << part.first_step().y() << "," << part.first_step().z() << "] => "
      << "[" << part.last_step().x() << "," << part.last_step().y() << "," << part.last_step().z() << "]" << std::endl;
      particles.emplace_back(std::move(part));
      break;
    }

    larcv::VoxelSet voxelSet;
    for (auto const & detPair : evt->SegmentDetectors)
    {
      larcv::VoxelSet voxels;
      LARCV_INFO() << "Sensitive detector: " << detPair.first << std::endl;
      for (auto const & hitSeg : detPair.second)
      {
        auto newVoxels = MakeVoxels(hitSeg, ev_hittensor->meta(), particles);
        LARCV_INFO() << "  Voxel count " << voxels.size() << " value sum " << voxels.sum() << std::endl;
        for (std::size_t idx = 0; idx < newVoxels.size(); idx++)
          voxelSet.emplace(std::move(newVoxels[idx]), true);
      }
    }

    ev_particles->emplace(std::move(particles));
    ev_hittensor->emplace(std::move(voxelSet), ev_hittensor->meta());

    return true;
  }

  void SuperaG4HitSegment::finalize()
  {}

  larcv::Particle SuperaG4HitSegment::MakeParticle(const TG4Trajectory& part) 
  {
    larcv::Particle res;
    res.track_id(part.GetTrackId());
    res.pdg_code(part.GetPDGCode());
    auto const& mom = part.GetInitialMomentum();
    res.momentum(mom.Px(),mom.Py(),mom.Pz());
    auto const& start = part.Points.front().GetPosition();
    res.position(start.X()/10.,start.Y()/10.,start.Z()/10.,start.T());
    auto const& end = part.Points.back().GetPosition();
    res.end_position(end.X()/10.,end.Y()/10.,end.Z()/10.,end.T());


    // first, last steps, distance travel, energy_deposit, num_voxels

    res.energy_init(mom.E());
    res.parent_track_id(part.GetParentId());

    return res;
  }

  void SuperaG4HitSegment::FluctuateEnergy(std::vector<Voxel> &voxels)
  {
    // just placeholder for now.
    return;
  }


  std::vector<larcv::Voxel> SuperaG4HitSegment::MakeVoxels(const ::TG4HitSegment &hitSegment,
                                                           const larcv::Voxel3DMeta &meta,
                                                           const std::vector<larcv::Particle> &particles)
  {
    std::vector<larcv::Voxel> voxels;

    //std::vector<std::vector<float> > res;
    //std::vector<float> res_pt(4);
    const float epsilon = 1.e-4;
    float dist_travel = 0.;
    float energy_deposit = 0.;
    float smallest_side = std::min(meta.size_voxel_x(),std::min(meta.size_voxel_y(),meta.size_voxel_z()));
    bool first_step_set = false;
    larcv::AABBox box(meta);
    LARCV_INFO() <<"World: " << box.bounds[0] << " => " << box.bounds[1] << std::endl;

    const TLorentzVector & start = hitSegment.GetStart();
    const TLorentzVector & end = hitSegment.GetStop();

    auto const& pos_start = start.Vect();
    auto const& pos_end   = end.Vect();
    double tstart = start.T();
    double tend   = end.T();

    larcv::Vec3f pt0(pos_start);
    larcv::Vec3f pt1(pos_end);
    // change unit to cm
    pt0 /= 10.;
    pt1 /= 10.;

    larcv::Vec3f dir = pt1 - pt0;
    auto length = dir.length();
    dir /= length;
    larcv::Ray ray(pt0,dir);
    // 1) check if the start is inside
    box = larcv::AABBox(meta);
    LARCV_INFO() <<"Ray: " << ray.orig << " dir " << ray.dir << std::endl;
    bool skip = false;
    if(!box.contain(pt0)) {
      float t0,t1;
      auto cross = box.intersect(ray,t0,t1);
      if(cross==0) skip = true;
      else if(cross==2) {
        pt0 = pt0 + dir * (t0 + epsilon);
        tstart = tstart + t0/length * (tend - tstart);
        length = (pt1 - pt0).length();
        ray = larcv::Ray(pt0,dir);
      }
    }

    if(skip) {
      LARCV_INFO() << "No crossing point found..." << std::endl;
      return voxels;
    }

    voxels.reserve(voxels.size() + (size_t)(length / smallest_side));
    float t0,t1,dist_section;
    dist_section = 0.;
    size_t nx, ny, nz;
    t0=t1=0.;
    //size_t ctr=0;
    while(1) {
      //ctr += 1;
      //if(ctr>=10) break;
      // define the inspection box
      Vec3f pt = pt0 + dir * (t1 + epsilon);
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
      float dx=0;
      if(cross==1) {
        LARCV_DEBUG() << "      One crossing: " << dir * t1 << std::endl;
        dx = std::min(t1,length);
      }else {
        LARCV_DEBUG() << "      Two crossing" << dir * t0 << " => " << dir * t1 << std::endl;
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
      float energyInVoxel = dx / length * hitSegment.GetEnergyDeposit();
      voxels.emplace_back(vox_id, energyInVoxel);
      dist_travel += dx;
      dist_section += dx;
      energy_deposit += energyInVoxel;
      //LARCV_DEBUG() << "      Registering voxel id " << vox_id << " at distance fraction " << t1/length << std::endl;
      LARCV_DEBUG() << "      Registering voxel id " << vox_id << " t1 " << t1 << " length " << length << std::endl;
      if(t1>length) {
        LARCV_DEBUG() << "      Reached the segment end (t1 = " << t1 << " fractional length " << t1/length << ") ... breaking" << std::endl;
        break;
      }
      LARCV_DEBUG() << "      Updated t1 = " << t1 << " (fractional length " << t1/length << ")" << std::endl;
    }
//    // update end position
//    if(dist_section > 0) {
//      pt1 = pt0 + dist_section * dir;
//      tend = tstart + (tend - tstart) / length * dist_section;
//      particles.last_step(pt1.x, pt1.y, pt1.z, tend);
//    }

    FluctuateEnergy(voxels);

//    particles.num_voxels(voxels.size());
//    particles.distance_travel(dist_travel);
//    particles.energy_deposit(energy_deposit);
    return voxels;
  }

}

#endif
