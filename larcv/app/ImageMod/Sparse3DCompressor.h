/**
 * \file Sparse3DCompressor.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class Sparse3DCompressor
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __Sparse3DCompressor_H__
#define __Sparse3DCompressor_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class Sparse3DCompressor ... these comments are used to generate
     doxygen documentation!
  */
  class Sparse3DCompressor : public ProcessBase {

  public:

    /// Default constructor
    Sparse3DCompressor(const std::string name = "Sparse3DCompressor");

    /// Default destructor
    ~Sparse3DCompressor() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    enum PoolType_t {kMaxPool,kSumPool};
    PoolType_t _pool_type;
    std::string _voxel_producer;
    std::string _output_producer;
    size_t _comp_factor;
    float  _scale_factor;
  };

  /**
     \class larcv::Sparse3DCompressorFactory
     \brief A concrete factory class for larcv::Sparse3DCompressor
  */
  class Sparse3DCompressorProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    Sparse3DCompressorProcessFactory() { ProcessFactory::get().add_factory("Sparse3DCompressor", this); }
    /// dtor
    ~Sparse3DCompressorProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Sparse3DCompressor(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

