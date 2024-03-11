#ifndef __NEUTRINO_CXX__
#define __NEUTRINO_CXX__

#include "Neutrino.h"
#include <set>
#include <sstream>
#include "larcv/core/Base/larbys.h"

namespace larcv {


  std::string Neutrino::dump() const
  {
    std::stringstream ss;
    std::stringstream buf;
    ss  << "      \033[95m" << "Neutrino " << " (PdgCode,TrackID) = (" << pdg_code() << "," << track_id() << ")\033[00m " << std::endl;
    buf << "      ";

    ss << buf.str() << "Vertex   (x, y, z, t) = (" << x() << "," << y() << "," << z() << "," << t() << ")" << std::endl
       << buf.str() << "Momentum (px, py, pz) = (" << px() << "," << py() << "," << pz() << ")" << std::endl
       << buf.str() << "Initial Energy  = " << energy_init() << std::endl
       << buf.str() << "Deposit  Energy  = " << energy_deposit() << std::endl
       << buf.str() << "Creation Process = " << creation_process() << std::endl;

    return ss.str();
  }


  void NeutrinoSet::set(const std::vector<larcv::Neutrino>& part_v)
  {
    _part_v = part_v;
    for(size_t i=0; i<_part_v.size(); ++i)
      if(_part_v.back().id() == kINVALID_INSTANCEID)
        _part_v[i].id(i);
  }

  void NeutrinoSet::append(const larcv::Neutrino& part)
  {
    _part_v.push_back(part);
    if(_part_v.back().id() == kINVALID_INSTANCEID)
      _part_v.back().id(_part_v.size()-1);
  }

  void NeutrinoSet::emplace_back(larcv::Neutrino&& part)
  {
    _part_v.emplace_back(std::move(part));
    if(_part_v.back().id() == kINVALID_INSTANCEID)
      _part_v.back().id(_part_v.size()-1);
  }

  void NeutrinoSet::emplace(std::vector<larcv::Neutrino>&& part_v)
  {
    _part_v = std::move(part_v);
    for(size_t i=0; i<_part_v.size(); ++i) {
      if(_part_v[i].id() == kINVALID_INSTANCEID)
        _part_v[i].id(i);
    }
  }

}

#endif
