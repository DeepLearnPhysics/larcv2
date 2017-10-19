/**
 * \file Back2Back.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class Back2Back
 *
 * @author vgenty
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __NUFILTER_H__
#define __NUFILTER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include "DataFormat/EventROI.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class NuFilter ... these comments are used to generate
     doxygen documentation!
  */
  class NuFilter : public ProcessBase {

  public:
    
    /// Default constructor
    NuFilter(const std::string name="NuFilter");
    
    /// Default destructor
    ~NuFilter(){}

    void configure(const PSet&);
    void initialize();
    bool process(IOManager& mgr);
    void finalize();

  private:

    bool _mc_available;
    
    bool MCSelect(const EventROI* ev_roi);
    
    std::string _roi_producer_name;
    
    uint _nu_pdg;
    int _interaction_mode;
    
    double _min_nu_init_e;
    double _max_nu_init_e;
    double _dep_sum_lepton;
    double _dep_sum_proton;

    bool _select_signal;
    bool _select_background;
    
    struct aparticle{
      int pdg;
      int trackid;
      int ptrackid;
      bool primary;
      float depeng;
    };

    uint _n_fail_nupdg;
    uint _n_fail_ccqe;
    uint _n_fail_nuE;
    uint _n_fail_lepton_dep;
    uint _n_fail_proton_dep;
    uint _n_pass;
    uint _n_calls;
    uint _n_fail_unknowns;
    uint _n_fail_inter;
    
    TTree* _event_tree;
    
    uint _run;
    uint _subrun;
    uint _event; 
    uint _entry;
    uint _selected;
  };

  /**
     \class larcv::NuFilterFactory
     \brief A concrete factory class for larcv::NuFilter
  */
  class NuFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    NuFilterProcessFactory() { ProcessFactory::get().add_factory("NuFilter",this); }
    /// dtor
    ~NuFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new NuFilter(instance_name); }
    
  };

}

#endif
/** @} */ // end of doxygen group 

