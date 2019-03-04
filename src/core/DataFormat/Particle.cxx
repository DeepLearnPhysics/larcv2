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
       << buf.str() << "Inittial Energy  = " << energy_init() << std::endl
       << buf.str() << "Deposit  Energy  = " << energy_deposit() << std::endl
       << buf.str() << "Creation Process = " << creation_process() << std::endl;

    ss << buf.str() << "BBox3D: " << _bb3d.dump();
    buf << "    ";
    for(size_t i=0; i<_bb2d_v.size(); ++i)
      ss << buf.str() << "Plane " << i << " BBox2D: " << _bb2d_v[i].dump();

    return ss.str();
  }

  void ParticleSet::set(const std::vector<larcv::Particle>& part_v)
  {
    _part_v = part_v;
    for(size_t i=0; i<_part_v.size(); ++i)
      _part_v[i].id(i);
  }

  void ParticleSet::append(const larcv::Particle& part)
  {
    _part_v.push_back(part);
    _part_v.back().id(_part_v.size()-1);
  }

  void ParticleSet::emplace_back(larcv::Particle&& part)
  {
    _part_v.emplace_back(std::move(part));
    _part_v.back().id(_part_v.size()-1);
  }

  void ParticleSet::emplace(std::vector<larcv::Particle>&& part_v)
  {
    _part_v = std::move(part_v);
    for(size_t i=0; i<_part_v.size(); ++i)
      _part_v[i].id(i);
  }
}

#endif
