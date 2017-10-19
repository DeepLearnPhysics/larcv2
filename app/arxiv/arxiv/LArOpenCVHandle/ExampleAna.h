#ifndef __EXAMPLEANA_H__
#define __EXAMPLEANA_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include "LArbysImageMaker.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "TTree.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventROI.h"
#include "DataFormat/EventPGraph.h"
#include "DataFormat/EventPixel2D.h"

namespace larcv {

  class ExampleAna : public ProcessBase {

  public:
    
    /// Default constructor
    ExampleAna(const std::string name="ExampleAna");
    
    /// Default destructor
    ~ExampleAna(){}

    void configure(const PSet&);
    void initialize();
    bool process(IOManager& mgr);
    void finalize();

  private:
    
    EventImage2D* _ev_img_v;
    EventImage2D* _ev_trk_img_v;
    EventImage2D* _ev_shr_img_v;
    EventROI* _ev_roi_v;
    EventPGraph* _ev_pgraph_v;
    EventPixel2D* _ev_pcluster_v;
    EventPixel2D* _ev_ctor_v;

    int _run;
    int _subrun;
    int _event;
    int _entry;

    std::string _img_prod;       
    std::string _reco_roi_prod;
    std::string _pgraph_prod;
    std::string _pcluster_img_prod;
    std::string _pcluster_ctor_prod; 
    std::string _trk_img_prod;
    std::string _shr_img_prod;
    
    LArbysImageMaker _LArbysImageMaker;
    
    void Analyze();

    TTree* _tree;
    
  };

  /**
     \class larcv::ExampleAnaFactory
     \brief A concrete factory class for larcv::ExampleAna
  */
  class ExampleAnaProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ExampleAnaProcessFactory() { ProcessFactory::get().add_factory("ExampleAna",this); }
    /// dtor
    ~ExampleAnaProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ExampleAna(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

