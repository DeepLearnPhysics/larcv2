/**
 * \file OneParticleTensor.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class OneParticleTensor
 *
 * @author drinkingkazu
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __ONEPARTICLETENSOR_H__
#define __ONEPARTICLETENSOR_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"

#include <set>
namespace larcv {

  /**
     \class ProcessBase
     User defined class OneParticleTensor ... these comments are used to generate
     doxygen documentation!
  */
  class OneParticleTensor : public ProcessBase {

  public:
    
    /// Default constructor
    OneParticleTensor(const std::string name="OneParticleTensor");
    
    /// Default destructor
    ~OneParticleTensor(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    enum SelectType_t {
      kEnergyInit,
      kEnergyDeposit,
      kVoxelCount
    };

    std::string _cluster3d_producer;
    std::string _particle_producer;
    std::string _output_producer;
    SelectType_t _criteria;
    std::set<int> _pdg_s;

  };

  /**
     \class larcv::OneParticleTensorFactory
     \brief A concrete factory class for larcv::OneParticleTensor
  */
  class OneParticleTensorProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    OneParticleTensorProcessFactory() { ProcessFactory::get().add_factory("OneParticleTensor",this); }
    /// dtor
    ~OneParticleTensorProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new OneParticleTensor(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

