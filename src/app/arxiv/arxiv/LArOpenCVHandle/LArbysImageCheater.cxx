#ifndef __LARBYSIMAGECHEATER_CXX__
#define __LARBYSIMAGECHEATER_CXX__

#include "LArbysImageCheater.h"
#include "LArOpenCV/ImageCluster/AlgoModule/VertexCheater.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"

namespace larcv {

  static LArbysImageCheaterProcessFactory __global_LArbysImageCheaterProcessFactory__;
  
  LArbysImageCheater::LArbysImageCheater(const std::string name) :
    LArbysImage(),
    _sce(),
    _mgr(nullptr)
  {}

  void LArbysImageCheater::SetIOManager(IOManager* mgr) {
    _mgr = mgr;
    LARCV_INFO() << "Set IOManager pointer as " << _mgr << std::endl;
  }
  
  void LArbysImageCheater::Process() {
    //
    // Cheat the first algo by extracting the truth vertex position
    //
    std::string cheater_alg_name = "vertexcheater";
    LARCV_DEBUG() << "Set cheater algo name: " << cheater_alg_name << std::endl;

    auto cheater_id = _alg_mgr.GetClusterAlgID(cheater_alg_name);
    auto cheater_alg = (larocv::VertexCheater*)_alg_mgr.GetClusterAlgRW(cheater_id);

    if (!cheater_alg) throw larbys("Could not find RW cheater algo");
    if (!_mgr) throw larbys("No IOManager pointer specified");
    
    auto true_roi_producer = _true_prod;
    LARCV_DEBUG() << "Set true ROI producer name: " << true_roi_producer << std::endl;
    
    auto ev_roi = (EventROI*)_mgr->get_data(kProductROI,true_roi_producer);
    
    if(!ev_roi) throw larbys("Could not read given true ROI producer");
    if(ev_roi->ROIArray().empty()) throw larbys("Could not find given true ROI producer");

    const auto& roi = ev_roi->ROIArray().front();
    
    auto tx = roi.X();
    auto ty = roi.Y();
    auto tz = roi.Z();

    LARCV_DEBUG() << "Read (x,y,z)=("<<tx<<","<<ty<<","<<tz<<")"<<std::endl;
    
    const auto offset = _sce.GetPosOffsets(tx,ty,tz);
    
    tx = tx - offset[0] + 0.7;
    ty = ty + offset[1];
    tz = tz + offset[2];

    LARCV_DEBUG() << "SCE (x,y,z)=("<<tx<<","<<ty<<","<<tz<<")"<<std::endl;
    
    larocv::data::Vertex3D true_vertex;
    true_vertex.x = tx;
    true_vertex.y = ty;
    true_vertex.z = tz;
    
    cheater_alg->SetTrueVertex(true_vertex);

    _alg_mgr.Process();
  }

  void LArbysImageCheater::SetTrueROIProducer(const std::string& true_prod) {
    _true_prod = true_prod;
  }
  
}

#endif
