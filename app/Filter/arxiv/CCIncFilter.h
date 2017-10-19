/**
 * \file CCIncFilter.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class CCIncFilter
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __CCINCFILTER_H__
#define __CCINCFILTER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class CCIncFilter ... these comments are used to generate
     doxygen documentation!
  */
  class CCIncFilter : public ProcessBase {

  public:
    
    /// Default constructor
    CCIncFilter(const std::string name="CCIncFilter");
    
    /// Default destructor
    ~CCIncFilter(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _roi_producer;
    double _shower_min_energy;
    double _track_min_energy;
    double _proton_min_energy;
    double _fiducial_cut_cm;
  };

  /**
     \class larcv::CCIncFilterFactory
     \brief A concrete factory class for larcv::CCIncFilter
  */
  class CCIncFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    CCIncFilterProcessFactory() { ProcessFactory::get().add_factory("CCIncFilter",this); }
    /// dtor
    ~CCIncFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new CCIncFilter(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

