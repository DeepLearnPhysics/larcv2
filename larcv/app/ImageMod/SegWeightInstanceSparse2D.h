/**
 * \file SegWeightInstanceSparse2D.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class SegWeightInstanceSparse2D
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SEGWEIGHTINSTANCESPARSE2D_H__
#define __SEGWEIGHTINSTANCESPARSE2D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class SegWeightInstanceSparse2D ... these comments are used to generate
     doxygen documentation!
  */
  class SegWeightInstanceSparse2D : public ProcessBase {

  public:

    /// Default constructor
    SegWeightInstanceSparse2D(const std::string name = "SegWeightInstanceSparse2D");

    /// Default destructor
    ~SegWeightInstanceSparse2D() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    enum PoolType_t {
      kSumPool,
      kMaxPool,
      kAveragePool,
      kOverwrite
    };

  private:


    std::string _cluster2d_producer;

    std::string _output_producer;

    PoolType_t _pool_type;

    std::vector<size_t> _projection_v;

    double _weight_min, _weight_max;

  };

  /**
     \class larcv::SegWeightInstanceSparse2DFactory
     \brief A concrete factory class for larcv::SegWeightInstanceSparse2D
  */
  class SegWeightInstanceSparse2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SegWeightInstanceSparse2DProcessFactory() { ProcessFactory::get().add_factory("SegWeightInstanceSparse2D", this); }
    /// dtor
    ~SegWeightInstanceSparse2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SegWeightInstanceSparse2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

