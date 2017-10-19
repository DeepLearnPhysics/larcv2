#ifndef __LARBYSIMAGERESULT_H__
#define __LARBYSIMAGERESULT_H__

#include <TTree.h>

#include "LArbysImageAnaBase.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"

namespace larcv {

  class LArbysImageResult : public LArbysImageAnaBase {

  public:
    
    LArbysImageResult(const std::string name="LArbysImageResult");
    ~LArbysImageResult(){}

    void Configure(const PSet&);
    void Initialize();
    bool Analyze(const ::larocv::ImageClusterManager& mgr);
    void Finalize(TFile* fout=nullptr);

  private:
    
    std::string _combined_vertex_name;
    uint _combined_particle_offset;
  };    

}

#endif

