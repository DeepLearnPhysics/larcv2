/**
 * \file MCROIHack.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class MCROIHack
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __MCROIHACK_H__
#define __MCROIHACK_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class MCROIHack ... these comments are used to generate
     doxygen documentation!
  */
  class MCROIHack : public ProcessBase {

  public:
    
    /// Default constructor
    MCROIHack(const std::string name="MCROIHack");
    
    /// Default destructor
    ~MCROIHack(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _roi_producer;
    int _num_processed;
    int _num_skipped;
    int _num_zero_roi;
    int _num_more_roi;
  };

  /**
     \class larcv::MCROIHackFactory
     \brief A concrete factory class for larcv::MCROIHack
  */
  class MCROIHackProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    MCROIHackProcessFactory() { ProcessFactory::get().add_factory("MCROIHack",this); }
    /// dtor
    ~MCROIHackProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new MCROIHack(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

