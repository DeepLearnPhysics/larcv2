#ifndef __LARBYSIMAGEOUT_H__
#define __LARBYSIMAGEOUT_H__

#include <TTree.h>

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include "DataFormat/EventImage2D.h"
#include "DataFormat/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"
#include "LArOpenCV/ImageCluster/AlgoClass/VertexAnalysis.h"

#include "LArbysImageAna.h"
#include "LArbysImageExtract.h"

namespace larcv {

  class LArbysImageOut : public ProcessBase {

  public:
    
    LArbysImageOut(const std::string name="LArbysImageOut");
    ~LArbysImageOut(){}

    void configure(const PSet&);
    void initialize();
    bool process(IOManager& mgr);
    void finalize();

    void SetLArbysImageAna(LArbysImageAna* larana)
    { _LArbysImageAna = larana; }
    void SetLArbysImageExtract(LArbysImageExtract* larext)
    { _LArbysImageExtract = larext; }
    
  private:

    TTree* _vtx3d_tree;
    
    void ClearVertex();

    uint _run;
    uint _subrun;
    uint _event;
    uint _entry;
    
    uint _n_vtx3d;
    uint _vtx3d_n_planes;
    uint _vtx3d_type;
    uint _vtx3d_id;
    double _vtx3d_x;
    double _vtx3d_y;
    double _vtx3d_z;

    std::vector<double> _vtx2d_x_v;
    std::vector<double> _vtx2d_y_v;
    std::vector<double> _circle_x_v;
    std::vector<double> _circle_y_v;
    std::vector<uint> _ntrack_par_v;
    std::vector<uint> _nshower_par_v;
    std::vector<uint> _circle_xs_v;
    std::vector<uint> _par_multi;

    LArbysImageAna* _LArbysImageAna;
    LArbysImageExtract* _LArbysImageExtract;
    
  };

  class LArbysImageOutProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    LArbysImageOutProcessFactory() { ProcessFactory::get().add_factory("LArbysImageOut",this); }
    /// dtor
    ~LArbysImageOutProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new LArbysImageOut(instance_name); }
  };
  
}

#endif
