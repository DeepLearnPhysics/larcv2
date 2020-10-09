#ifndef __SUPERAG4Trajectory_CXX__
#define __SUPERAG4Trajectory_CXX__

#include "SuperaG4Trajectory.h"
#include "GenRandom.h"
#include "geometry.h"
#include "raybox.h"
namespace larcv {

  static SuperaG4TrajectoryProcessFactory __global_SuperaG4TrajectoryProcessFactory__;

  SuperaG4Trajectory::SuperaG4Trajectory(const std::string name) : SuperaBase(name)
  { }

  void SuperaG4Trajectory::configure(const PSet& cfg)
  {
    SuperaBase::configure(cfg);
    _cluster3d_producer=cfg.get<std::string>("Cluster3DProducer");
    _particle_producer =cfg.get<std::string>("ParticleProducer");
  }

  void SuperaG4Trajectory::initialize()
  {
    SuperaBase::initialize();
  }

  bool SuperaG4Trajectory::process(IOManager& mgr)
  {
    SuperaBase::process(mgr);

    auto evt = this->GetEvent();

    auto ev_cluster3d = (EventClusterVoxel3D*)(mgr.get_data("cluster3d",_cluster3d_producer));
    auto ev_particles = (EventParticle*)(mgr.get_data("particle",_particle_producer));

    std::vector<larcv::Particle> part_v;
    part_v.reserve(evt->Trajectories.size());
    std::vector<larcv::VoxelSet> vs_v;
    vs_v.reserve(evt->Trajectories.size());
    for(auto const& traj : evt->Trajectories) {
      auto part = this->MakeParticle(traj);
      part.id(ev_particles->as_vector().size());
      auto vs = this->MakeVoxelSet(traj,ev_cluster3d->meta(),part);
      LARCV_INFO() << "Track ID " << part.track_id() << " PDG " << part.pdg_code() << " ... " 
      << "[" << part.first_step().x() << "," << part.first_step().y() << "," << part.first_step().z() << "] => "
      << "[" << part.last_step().x() << "," << part.last_step().y() << "," << part.last_step().z() << "]" << std::endl;
      LARCV_INFO() << "  Voxel count " << vs.size() << " value sum " << vs.sum() << std::endl;
      part_v.emplace_back(std::move(part));
      vs_v.emplace_back(std::move(vs));
      break;
    }

    ev_particles->emplace(std::move(part_v));
    larcv::VoxelSetArray vsa;
    vsa.emplace(std::move(vs_v));
    ev_cluster3d->emplace(std::move(vsa),ev_cluster3d->meta());
    return true;
  }

  void SuperaG4Trajectory::finalize()
  {}

  larcv::Particle SuperaG4Trajectory::MakeParticle(const TG4Trajectory& part) 
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

    /*
        inline void id              (InstanceID_t id  )  { _id = id;         }
    inline void mcst_index      (MCSTIndex_t id )    { _mcst_index = id;    }
    inline void mct_index       (MCTIndex_t id )     { _mct_index = id;     }
    inline void shape           (ShapeType_t shape ) { _shape = shape;      }
    inline void nu_current_type (short curr) {_current_type = curr; }
    inline void nu_interaction_type (short itype) {_interaction_type = itype; }
    // particle's info setter

    inline void creation_process (const std::string& proc) { _process = proc; }

    // parent info setter
    inline void parent_track_id (unsigned int id )   { _parent_trackid = id;}
    inline void parent_pdg_code (int code)           { _parent_pdg = code;  }
    inline void parent_position (const larcv::Vertex& vtx) { _parent_vtx = vtx; }
    inline void parent_position (double x, double y, double z, double t) { _parent_vtx = Vertex(x,y,z,t); }
    inline void parent_creation_process(const std::string& proc) { _parent_process = proc; }
    inline void parent_id       (InstanceID_t id)    { _parent_id = id; }
    inline void children_id     (InstanceID_t id)    { _children_id.push_back(id); }
    inline void children_id     (const std::vector<larcv::InstanceID_t>& id_v) { _children_id = id_v; }
    // ancestor info setter
    inline void ancestor_track_id (unsigned int id )   { _ancestor_trackid = id;}
    inline void ancestor_pdg_code (int code)           { _ancestor_pdg = code;  }
    inline void ancestor_position (const larcv::Vertex& vtx) { _ancestor_vtx = vtx; }
    inline void ancestor_position (double x, double y, double z, double t) { _ancestor_vtx = Vertex(x,y,z,t); }
    inline void ancestor_creation_process(const std::string& proc) { _ancestor_process = proc; }
    // group id setter
    inline void group_id(InstanceID_t id)       { _group_id = id;       }
    inline void interaction_id(InstanceID_t id) { _interaction_id = id; }
    */

    return res;
  }


  larcv::VoxelSet SuperaG4Trajectory::MakeVoxelSet(const ::TG4Trajectory& trj, 
    const larcv::Voxel3DMeta& meta,
    larcv::Particle& part)
  {
    larcv::VoxelSet vs;
    double mass2 = trj.GetInitialMomentum().M2();
    const double epsilon = 1.e-4;
    double dist_travel = 0.;
    double energy_deposit = 0.;
    double smallest_side = std::min(meta.size_voxel_x(),std::min(meta.size_voxel_y(),meta.size_voxel_z()));
    bool first_step_set = false;
    LARCV_INFO() << "Tracking: PDG " << trj.GetPDGCode() << " E = " << trj.GetInitialMomentum().E() << " MeV" << std::endl;
    larcv::AABBox<double> box(meta);
    LARCV_INFO() <<"World: " << box.bounds[0] << " => " << box.bounds[1] << std::endl;

    for(size_t pidx=0; (pidx+1)<trj.Points.size(); ++pidx) {
      auto const& start = trj.Points[pidx];
      auto const& end = trj.Points[pidx+1];
      auto const& pos_start = start.GetPosition();
      auto const& pos_end   = end.GetPosition();
      double tstart = pos_start.T();
      double tend   = pos_end.T();

      larcv::Vec3d pt0(pos_start);
      larcv::Vec3d pt1(pos_end);
      // change unit to cm
      pt0 /= 10.;
      pt1 /= 10.;
      LARCV_INFO() << "Segment: idx " << pidx << " ... " << pt0 << " => " << pt1 << std::endl;

      larcv::Vec3d dir = pt1 - pt0;
      auto length = dir.length();
      dir /= length;
      larcv::Ray<double> ray(pt0,dir);
      // 1) check if the start is inside
      box = larcv::AABBox<double>(meta);
      LARCV_INFO() <<"Ray: " << ray.orig << " dir " << ray.dir << std::endl;
      bool skip = false;
      if(!box.contain(pt0)) {
        double t0,t1;
        auto cross = box.intersect(ray,t0,t1);
        if(cross==0) skip = true;
        else if(cross==2) {
          pt0 = pt0 + dir * (t0 + epsilon);
          tstart = tstart + t0/length * (tend - tstart);
          length = (pt1 - pt0).length();
          ray = larcv::Ray<double>(pt0,dir);
        }
      }

      if(skip) { 
        LARCV_INFO() << "No crossing point found..." << std::endl;
        continue;
      }
      // now pt0, pt1, length, dir will remain constant within this iteration
      if(!first_step_set) {
        part.first_step(pt0.x,pt0.y,pt0.z,tstart);
        first_step_set = true;
      }
      vs.reserve(vs.size() + (size_t)(length/smallest_side));
      double energy_diff = sqrt(start.GetMomentum().Mag2() + mass2) - sqrt(end.GetMomentum().Mag2() + mass2);
      double t0,t1,dist_section;
      dist_section = 0.;
      size_t nx, ny, nz;
      t0=t1=0.;
      //size_t ctr=0;
      while(1) {
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
        vs.emplace(vox_id,(dx/length*energy_diff),true);
        dist_travel += dx;
        dist_section += dx;
        energy_deposit += dx/length*energy_diff;
        //LARCV_DEBUG() << "      Registering voxel id " << vox_id << " at distance fraction " << t1/length << std::endl;
        LARCV_DEBUG() << "      Registering voxel id " << vox_id << " t1 " << t1 << " length " << length << std::endl;
        if(t1>length) {
          LARCV_DEBUG() << "      Reached the segment end (t1 = " << t1 << " fractional length " << t1/length << ") ... breaking" << std::endl;
          break;
        }
        LARCV_DEBUG() << "      Updated t1 = " << t1 << " (fractional length " << t1/length << ")" << std::endl;
      }
      // update end position
      if(dist_section > 0) {
        pt1 = pt0 + dist_section * dir;
        tend = tstart + (tend - tstart) / length * dist_section;
        part.last_step(pt1.x, pt1.y, pt1.z, tend);
      }
    }
    part.num_voxels(vs.size());
    part.distance_travel(dist_travel);
    part.energy_deposit(energy_deposit);
    return vs;
  }
}

#endif
