/**
 * \file EmptyVoxelFilter.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class EmptyVoxelFilter
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __EMPTYVOXELFILTER_H__
#define __EMPTYVOXELFILTER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class EmptyVoxelFilter ... these comments are used to generate
     doxygen documentation!
  */
  class EmptyVoxelFilter : public ProcessBase {

  public:
    
    /// Default constructor
    EmptyVoxelFilter(const std::string name="EmptyVoxelFilter");
    
    /// Default destructor
    ~EmptyVoxelFilter(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    size_t _min_voxel_count;
    std::string _voxel_producer;
    float _min_voxel_value;

  };

  /**
     \class larcv::EmptyVoxelFilterFactory
     \brief A concrete factory class for larcv::EmptyVoxelFilter
  */
  class EmptyVoxelFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    EmptyVoxelFilterProcessFactory() { ProcessFactory::get().add_factory("EmptyVoxelFilter",this); }
    /// dtor
    ~EmptyVoxelFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new EmptyVoxelFilter(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

