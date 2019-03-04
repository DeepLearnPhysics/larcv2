/**
 * \file LArbysImagePID.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class LArbysImagePID
 *
 * @author Rui
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __LARBYSIMAGEPID_H__
#define __LARBYSIMAGEPID_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include "DataFormat/EventPGraph.h"
#include "DataFormat/ROI.h"
#include "DataFormat/PGraph.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class LArbysImagePID ... these comments are used to generate
     doxygen documentation!
  */
  class LArbysImagePID : public ProcessBase {

  public:
    
    /// Default constructor
    LArbysImagePID(const std::string name="LArbysImagePID",
		   const std::vector<std::string> particle_types= {"electron", "gamma", "muon", "pion", "proton"},
		   const std::vector<std::string> interaction_types= {"1p1e", "1p1g", "1p1mu", "1p1pi", "2p"});
    
    /// Default destructor
    ~LArbysImagePID(){}

    void configure(const PSet&);

    void Clear();
    
    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    
    bool _use_shape;

    std::vector<std::string> _particle_types_v;
    std::vector<std::string> _interaction_types_v;

    std::vector<std::vector<float>> _rois_score_vv; //Store all info from ROI
    //Store only matched ROI, per vertex per roi(for 2 p cases...)
    std::vector<std::vector<std::vector<float>>> _ptype_scores_vvv;
    std::vector<int> _ptype_ctr_v;
    
    std::vector<std::pair<int, int>> _interaction_pair_v;
    
    std::vector<float> _score_sum_v;

    uint _run;
    uint _subrun;
    uint _event;
    uint _entry; 
    
    bool _verbosity;

  };

  /**
     \class larcv::LArbysImagePIDFactory
     \brief A concrete factory class for larcv::LArbysImagePID
  */
  class LArbysImagePIDProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    LArbysImagePIDProcessFactory() { ProcessFactory::get().add_factory("LArbysImagePID",this); }
    /// dtor
    ~LArbysImagePIDProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new LArbysImagePID(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

