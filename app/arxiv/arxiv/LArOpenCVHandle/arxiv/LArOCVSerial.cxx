#ifndef LAROCVSERIAL_CXX
#define LAROCVSERIAL_CXX

#include "LArOCVSerial.h"

namespace larcv {

  void LArOCVSerial::Clear() {
    _vertex_v.clear();
    _particle_cluster_vvv.clear();
    _track_comp_vvv.clear();
    _match_pvvv.clear();
    _meta_vv.clear();
  }

}
#endif
