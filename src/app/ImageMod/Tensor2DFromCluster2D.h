/**
 * \file Tensor2DFromCluster2D.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class Tensor2DFromCluster2D
 *
 * @author kazuhiro, cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __Tensor2DFromCluster2D_H__
#define __Tensor2DFromCluster2D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class Tensor2DFromCluster2D ... these comments are used to generate
     doxygen documentation!
  */
  class Tensor2DFromCluster2D : public ProcessBase {

  public:

    /// Default constructor
    Tensor2DFromCluster2D(const std::string name="Tensor2DFromCluster2D");

    /// Default destructor
    ~Tensor2DFromCluster2D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    void configure_labels(const PSet& cfg);

    enum class PIType_t {
      kPITypeFixedPI,
      kPITypeInputVoxel,
      kPITypeClusterIndex,
      kPITypeUndefined
    };

    std::vector<std::string> _cluster2d_producer_v;
    std::vector<std::string> _output_producer_v;
    std::vector<unsigned short> _pi_type_v;
    std::vector<float> _fixed_pi_v;
  };

  /**
     \class larcv::Tensor2DFromCluster2DFactory
     \brief A concrete factory class for larcv::Tensor2DFromCluster2D
  */
  class Tensor2DFromCluster2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    Tensor2DFromCluster2DProcessFactory() { ProcessFactory::get().add_factory("Tensor2DFromCluster2D",this); }
    /// dtor
    ~Tensor2DFromCluster2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Tensor2DFromCluster2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

