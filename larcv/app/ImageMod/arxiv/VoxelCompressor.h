/**
 * \file VoxelCompressor.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class VoxelCompressor
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __VOXELCOMPRESSOR_H__
#define __VOXELCOMPRESSOR_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class VoxelCompressor ... these comments are used to generate
     doxygen documentation!
  */
  class VoxelCompressor : public ProcessBase {

  public:
    
    /// Default constructor
    VoxelCompressor(const std::string name="VoxelCompressor");
    
    /// Default destructor
    ~VoxelCompressor(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    std::string _voxel_producer;
    std::string _output_producer;
    size_t _comp_factor;
    float  _scale_factor;
  };

  /**
     \class larcv::VoxelCompressorFactory
     \brief A concrete factory class for larcv::VoxelCompressor
  */
  class VoxelCompressorProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    VoxelCompressorProcessFactory() { ProcessFactory::get().add_factory("VoxelCompressor",this); }
    /// dtor
    ~VoxelCompressorProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new VoxelCompressor(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

