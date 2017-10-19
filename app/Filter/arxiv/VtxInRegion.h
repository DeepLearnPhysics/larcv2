/**
 * \file VtxInRegion.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class VtxInRegion
 *
 * @author taritree
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __VTXINREGION_H__
#define __VTXINREGION_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include <string>

namespace larcv {

  /**
     \class ProcessBase
     User defined class VtxInRegion ... these comments are used to generate
     doxygen documentation!
  */
  class VtxInRegion : public ProcessBase {

  public:
    
    /// Default constructor
    VtxInRegion(const std::string name="VtxInRegion");
    
    /// Default destructor
    ~VtxInRegion(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();


  private:

    bool in_region(double Y,double Z);

    bool _above;
    double _buffer;
    std::string _part_producer;
    double _slope;
    double _yinter;

  };

  /**
     \class larcv::VtxInRegionFactory
     \brief A concrete factory class for larcv::VtxInRegion
  */
  class VtxInRegionProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    VtxInRegionProcessFactory() { ProcessFactory::get().add_factory("VtxInRegion",this); }
    /// dtor
    ~VtxInRegionProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new VtxInRegion(instance_name); }

  };

}

#endif
/** @} */ // end of doxygen group 

