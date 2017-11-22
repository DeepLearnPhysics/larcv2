/**
 * \file Tensor3DCompressor.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class Tensor3DCompressor
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __Tensor3DCompressor_H__
#define __Tensor3DCompressor_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class Tensor3DCompressor ... these comments are used to generate
     doxygen documentation!
  */
  class Tensor3DCompressor : public ProcessBase {

  public:

    /// Default constructor
    Tensor3DCompressor(const std::string name = "Tensor3DCompressor");

    /// Default destructor
    ~Tensor3DCompressor() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    void configure_labels(const PSet& cfg);

    enum PoolType_t {kMaxPool,kSumPool};
    std::vector<unsigned short> _pool_type_v;
    std::vector<std::string> _tensor3d_producer_v;
    std::vector<std::string> _output_producer_v;
    std::vector<size_t> _comp_factor_v;
    std::vector<float>  _scale_factor_v;
  };

  /**
     \class larcv::Tensor3DCompressorFactory
     \brief A concrete factory class for larcv::Tensor3DCompressor
  */
  class Tensor3DCompressorProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    Tensor3DCompressorProcessFactory() { ProcessFactory::get().add_factory("Tensor3DCompressor", this); }
    /// dtor
    ~Tensor3DCompressorProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Tensor3DCompressor(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

