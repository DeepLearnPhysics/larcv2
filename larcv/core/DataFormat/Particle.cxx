#ifndef __PARTICLE_CXX__
#define __PARTICLE_CXX__

#include "Particle.h"
#include <set>
#include <sstream>
#include "larcv/core/Base/larbys.h"

namespace larcv {

  const BBox2D& Particle::boundingbox_2d(ProjectionID_t id) const
  {
    std::stringstream ss;
    if(id < _bb2d_v.size()) return _bb2d_v[id];
    ss << "BBox2D not stored for ProjectionID_t " << id << std::endl;
    throw larbys(ss.str());
  }

  std::string Particle::dump() const
  {
    std::stringstream ss;
    std::stringstream buf;
    ss  << "      \033[95m" << "Particle " << " (PdgCode,TrackID) = (" << pdg_code() << "," << track_id() << ")\033[00m "
	<< "... with Parent (" << parent_pdg_code() << "," << parent_track_id() << ")" << std::endl;
    buf << "      ";

    ss << buf.str() << "Vertex   (x, y, z, t) = (" << x() << "," << y() << "," << z() << "," << t() << ")" << std::endl
       << buf.str() << "Momentum (px, py, pz) = (" << px() << "," << py() << "," << pz() << ")" << std::endl
       << buf.str() << "Final Momentum (end_px, end_py, end_pz) = (" << end_px() << "," << end_py() << "," << end_pz() << ")" << std::endl
       << buf.str() << "Initial Energy  = " << energy_init() << std::endl
       << buf.str() << "Deposit  Energy  = " << energy_deposit() << std::endl
       << buf.str() << "Creation Process = " << creation_process() << std::endl
       << buf.str() << "Instance ID = " << id() << std::endl
       << buf.str() << "Group ID    = " << group_id() << std::endl
       << buf.str() << "Interaction ID = " << interaction_id() << std::endl;
    /*
    ss << buf.str() << "BBox3D: " << _bb3d.dump();
    buf << "    ";
    for(size_t i=0; i<_bb2d_v.size(); ++i)
      ss << buf.str() << "Plane " << i << " BBox2D: " << _bb2d_v[i].dump();
    */
    return ss.str();
  }

  void ParticleSet::set(const std::vector<larcv::Particle>& part_v)
  {
    _part_v = part_v;
    for(size_t i=0; i<_part_v.size(); ++i)
      if(_part_v.back().id() == kINVALID_INSTANCEID)
        _part_v[i].id(i);
  }

  void ParticleSet::append(const larcv::Particle& part)
  {
    _part_v.push_back(part);
    if(_part_v.back().id() == kINVALID_INSTANCEID)
      _part_v.back().id(_part_v.size()-1);
  }

  void ParticleSet::emplace_back(larcv::Particle&& part)
  {
    _part_v.emplace_back(std::move(part));
    if(_part_v.back().id() == kINVALID_INSTANCEID)
      _part_v.back().id(_part_v.size()-1);
  }

  void ParticleSet::emplace(std::vector<larcv::Particle>&& part_v)
  {
    _part_v = std::move(part_v);
    for(size_t i=0; i<_part_v.size(); ++i) {
      if(_part_v[i].id() == kINVALID_INSTANCEID)
        _part_v[i].id(i);
    }
  }

  void ParticleSet::merge(const ParticleSet& ps,
    const double dx, const double dy, const double dz, const double dt)
  {
    InstanceID_t id_offset,pid_offset,gid_offset,iid_offset;
    id_offset=pid_offset=gid_offset=iid_offset=0;

    unsigned int tid_offset, ptid_offset, atid_offset;
    tid_offset=ptid_offset=atid_offset=0;

    for(auto const& p : _part_v) {
      if(p.id() != larcv::kINVALID_INSTANCEID)
        id_offset = std::max(id_offset,(InstanceID_t)(p.id()+1));
      if(p.parent_id() != larcv::kINVALID_INSTANCEID)
        pid_offset = std::max(pid_offset,(InstanceID_t)(p.parent_id()+1));
      if(p.group_id() != larcv::kINVALID_INSTANCEID)
        gid_offset = std::max(gid_offset,(InstanceID_t)(p.group_id()+1));
      if(p.interaction_id() != larcv::kINVALID_INSTANCEID)
        iid_offset = std::max(iid_offset,(InstanceID_t)(p.interaction_id()+1));
      if(p.track_id() != larcv::kINVALID_UINT)
        tid_offset = std::max(tid_offset,(unsigned int)(p.track_id()+1));
      if(p.parent_track_id() != larcv::kINVALID_UINT)
        ptid_offset = std::max(ptid_offset,(unsigned int)(p.parent_track_id()+1));
      if(p.ancestor_track_id() != larcv::kINVALID_UINT)
        atid_offset = std::max(atid_offset,(unsigned int)(p.ancestor_track_id()+1));
    }

    for(auto p : ps.as_vector()) {
      // Shift ID
      if(p.id() != larcv::kINVALID_INSTANCEID)
        p.id(p.id()+id_offset);
      if(p.parent_id() != larcv::kINVALID_INSTANCEID)
        p.parent_id(p.parent_id()+pid_offset);
      if(p.group_id() != larcv::kINVALID_INSTANCEID)
        p.group_id(p.group_id()+gid_offset);
      if(p.interaction_id() != larcv::kINVALID_INSTANCEID)
        p.interaction_id(p.interaction_id()+iid_offset);
      if(p.track_id() != larcv::kINVALID_UINT)
        p.track_id(p.track_id()+tid_offset);
      if(p.parent_track_id() != larcv::kINVALID_UINT)
        p.parent_track_id(p.parent_track_id()+ptid_offset);
      if(p.ancestor_track_id() != larcv::kINVALID_UINT)
        p.ancestor_track_id(p.ancestor_track_id()+atid_offset);

      // Shift position
      p.position(p.x()+dx, p.y()+dy, p.z()+dz, p.t()+dt);
      p.end_position(p.end_position().x()+dx,
                     p.end_position().y()+dy,
                     p.end_position().z()+dz,
                     p.end_position().t()+dt
                     );
      p.first_step(p.first_step().x()+dx, p.first_step().y()+dy, 
                   p.first_step().z()+dz, p.first_step().t()+dt
                   );
      p.last_step(p.last_step().x()+dx, p.last_step().y()+dy, 
                  p.last_step().z()+dz, p.last_step().t()+dt
                  );
      p.parent_position(p.parent_position().x()+dx,
                        p.parent_position().y()+dy,
                        p.parent_position().z()+dz,
                        p.parent_position().t()+dt
                        );
      p.ancestor_position(p.ancestor_position().x()+dx,
                          p.ancestor_position().y()+dy,
                          p.ancestor_position().z()+dz,
                          p.ancestor_position().t()+dt
                          );
      _part_v.emplace_back(std::move(p));
    }
  }

}

#endif
