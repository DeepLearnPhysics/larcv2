/**
 * \file PreProcessorAna.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class PreProcessorAna
 *
 * @author vgenty
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __PREPROCESSORANA_H__
#define __PREPROCESSORANA_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include "PreProcessor.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class PreProcessorAna ... these comments are used to generate
     doxygen documentation!
  */
  class PreProcessorAna : public ProcessBase {

  public:
    
    /// Default constructor
    PreProcessorAna(const std::string name="PreProcessorAna");
    
    /// Default destructor
    ~PreProcessorAna(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    void Clear();
    
    TTree *_out_tree;
    PreProcessor _PreProcessor;
    std::vector<float> _true_track_ctr_v;
    std::vector<float> _true_shower_ctr_v;
    std::vector<float> _ss_track_ctr_v;
    std::vector<float> _ss_shower_ctr_v;
    std::vector<float> _pss_track_ctr_v;
    std::vector<float> _pss_shower_ctr_v;
    std::vector<float> _ss_ratio_v;
    std::vector<float> _pss_ratio_v;
  };

  /**
     \class larcv::PreProcessorAnaFactory
     \brief A concrete factory class for larcv::PreProcessorAna
  */
  class PreProcessorAnaProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    PreProcessorAnaProcessFactory() { ProcessFactory::get().add_factory("PreProcessorAna",this); }
    /// dtor
    ~PreProcessorAnaProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new PreProcessorAna(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

