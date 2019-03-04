/**
 * \file SegLabelFiveTypes.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class SegLabelFiveTypes
 *
 * @author kterao
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __SEGLABELFIVETYPES_H__
#define __SEGLABELFIVETYPES_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class SegLabelFiveTypes ... these comments are used to generate
     doxygen documentation!
  */
  class SegLabelFiveTypes : public ProcessBase {

  public:
    
    /// Default constructor
    SegLabelFiveTypes(const std::string name="SegLabelFiveTypes");
    
    /// Default destructor
    ~SegLabelFiveTypes(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    std::string _cluster3d_producer;
    std::string _particle_producer;
    std::string _output_producer;
    size_t _min_num_voxel;
  };

  /**
     \class larcv::SegLabelFiveTypesFactory
     \brief A concrete factory class for larcv::SegLabelFiveTypes
  */
  class SegLabelFiveTypesProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SegLabelFiveTypesProcessFactory() { ProcessFactory::get().add_factory("SegLabelFiveTypes",this); }
    /// dtor
    ~SegLabelFiveTypesProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SegLabelFiveTypes(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

