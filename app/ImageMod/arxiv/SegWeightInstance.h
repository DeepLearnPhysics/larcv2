/**
 * \file SegWeightInstance.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class SegWeightInstance
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SEGWEIGHTTRACKSHOWER_H__
#define __SEGWEIGHTTRACKSHOWER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class SegWeightInstance ... these comments are used to generate
     doxygen documentation!
  */
  class SegWeightInstance : public ProcessBase {

  public:
    
    /// Default constructor
    SegWeightInstance(const std::string name="SegWeightInstance");
    
    /// Default destructor
    ~SegWeightInstance(){}

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

    std::vector<size_t> _plane_v;

    unsigned int _weight_max;

  };

  /**
     \class larcv::SegWeightInstanceFactory
     \brief A concrete factory class for larcv::SegWeightInstance
  */
  class SegWeightInstanceProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SegWeightInstanceProcessFactory() { ProcessFactory::get().add_factory("SegWeightInstance",this); }
    /// dtor
    ~SegWeightInstanceProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SegWeightInstance(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

