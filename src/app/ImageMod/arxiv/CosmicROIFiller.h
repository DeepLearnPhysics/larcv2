/**
 * \file CosmicROIFiller.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class CosmicROIFiller
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __COSMICROIFILLER_H__
#define __COSMICROIFILLER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class CosmicROIFiller ... these comments are used to generate
     doxygen documentation!
  */
  class CosmicROIFiller : public ProcessBase {

  public:
    
    /// Default constructor
    CosmicROIFiller(const std::string name="CosmicROIFiller");
    
    /// Default destructor
    ~CosmicROIFiller(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _roi_producer;
    ProductType_t _ref_type;
    std::string _ref_producer;
    int _roi_type;

  };

  /**
     \class larcv::CosmicROIFillerFactory
     \brief A concrete factory class for larcv::CosmicROIFiller
  */
  class CosmicROIFillerProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    CosmicROIFillerProcessFactory() { ProcessFactory::get().add_factory("CosmicROIFiller",this); }
    /// dtor
    ~CosmicROIFillerProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new CosmicROIFiller(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

