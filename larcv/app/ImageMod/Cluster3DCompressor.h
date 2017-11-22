/**
 * \file Cluster3DCompressor.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class Cluster3DCompressor
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __Cluster3DCompressor_H__
#define __Cluster3DCompressor_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class Cluster3DCompressor ... these comments are used to generate
     doxygen documentation!
  */
  class Cluster3DCompressor : public ProcessBase {

  public:

    /// Default constructor
    Cluster3DCompressor(const std::string name = "Cluster3DCompressor");

    /// Default destructor
    ~Cluster3DCompressor() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    void configure_labels(const PSet& cfg);

    enum PoolType_t {kMaxPool,kSumPool};
    PoolType_t _pool_type;
    std::vector<unsigned short> _pool_type_v;
    std::vector<std::string> _cluster3d_producer_v;
    std::vector<std::string> _output_producer_v;
    std::vector<size_t> _comp_factor_v;
    std::vector<float>  _scale_factor_v;
  };

  /**
     \class larcv::Cluster3DCompressorFactory
     \brief A concrete factory class for larcv::Cluster3DCompressor
  */
  class Cluster3DCompressorProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    Cluster3DCompressorProcessFactory() { ProcessFactory::get().add_factory("Cluster3DCompressor", this); }
    /// dtor
    ~Cluster3DCompressorProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Cluster3DCompressor(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

