#ifndef __LEE1E1PANA_H__
#define __LEE1E1PANA_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include <TTree.h>
#include <math.h>
#include <numeric>

namespace larcv {

  class LEE1e1pAna : public ProcessBase {

  public:
    
    /// Default constructor
    LEE1e1pAna(const std::string name="LEE1e1pAna");
    
    /// Default destructor
    ~LEE1e1pAna(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();
    
  private:

    TTree* _tree;

    int _entry;
    int _run;
    int _subrun;
    int _event;
    int _roid;
    int _vtxid;
    
    std::vector<double> _score0;
    std::vector<double> _score1;
    
    int _shape0;
    int _shape1;
    
    double _score_shower0;
    double _score_shower1;

    double _score_track0;
    double _score_track1;

    double _score0_e;
    double _score0_g;
    double _score0_pi;
    double _score0_mu;
    double _score0_p;

    double _score1_e;    
    double _score1_g;
    double _score1_pi;
    double _score1_mu;
    double _score1_p;

    int _npx0;
    int _npx1;

    double _q0;
    double _q1;

    double _area0;
    double _area1;

    double _len0;
    double _len1;

    int _nprotons; //< number of truth protons
    int _nothers;  //< number of other particles

    std::string _img2d_prod;
    std::string _pgraph_prod;        
    std::string _pcluster_ctor_prod;
    std::string _pcluster_img_prod;
    std::string _truth_roi_prod;  
    std::string _reco_roi_prod;    

    void ClearEvent();
    void ClearVertex();
    
  };

  /**
     \class larcv::LEE1e1pAnaFactory
     \brief A concrete factory class for larcv::LEE1e1pAna
  */
  class LEE1e1pAnaProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    LEE1e1pAnaProcessFactory() { ProcessFactory::get().add_factory("LEE1e1pAna",this); }
    /// dtor
    ~LEE1e1pAnaProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new LEE1e1pAna(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

