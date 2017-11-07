/**
 * \file SegWeightInstance2D.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class SegWeightInstance2D
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SEGWEIGHTTRACKSHOWER_H__
#define __SEGWEIGHTTRACKSHOWER_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class SegWeightInstance2D ... these comments are used to generate
     doxygen documentation!
  */
  class SegWeightInstance2D : public ProcessBase {

  public:

    /// Default constructor
    SegWeightInstance2D(const std::string name = "SegWeightInstance2D");

    /// Default destructor
    ~SegWeightInstance2D() {}

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

    std::string _label_image_producer;
    std::string _instance_image_producer;
    std::string _keypt_pixel2d_producer;

    std::string _weight_producer;

    PoolType_t _pool_type;

    bool _weight_surrounding;

    size_t _dist_surrounding;

    size_t _weight_vertex;

    std::vector<size_t> _projection_v;

    std::vector<float> _temp_weight_data;

    std::vector<float> _boundary_data;

    unsigned int _weight_max;

  };

  /**
     \class larcv::SegWeightInstance2DFactory
     \brief A concrete factory class for larcv::SegWeightInstance2D
  */
  class SegWeightInstance2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SegWeightInstance2DProcessFactory() { ProcessFactory::get().add_factory("SegWeightInstance2D", this); }
    /// dtor
    ~SegWeightInstance2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SegWeightInstance2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

