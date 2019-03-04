#ifndef LAROCVSERIAL_H
#define LAROCVSERIAL_H

#include "DataFormat/ImageMeta.h"


namespace larcv {

  class LArOCVSerial{

  public:
  
    LArOCVSerial(){}
    ~LArOCVSerial(){}
    void Clear();
  
    std::vector<larocv::data::Vertex3D> _vertex_v;
    std::vector<std::vector<std::vector<larocv::data::ParticleCluster> > > _particle_cluster_vvv;
    std::vector<std::vector<std::vector<larocv::data::TrackClusterCompound> > > _track_comp_vvv;
    std::vector<std::vector<std::vector<std::pair<size_t,size_t> > > > _match_pvvv;
    std::vector<std::vector<larcv::ImageMeta> > _meta_vv;
  
  };
}

#endif


