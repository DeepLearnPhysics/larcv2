/**
 * \file SegWeightTrackShower.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class SegWeightTrackShower
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
     User defined class SegWeightTrackShower ... these comments are used to generate
     doxygen documentation!
  */
  class SegWeightTrackShower : public ProcessBase {

  public:
    
    /// Default constructor
    SegWeightTrackShower(const std::string name="SegWeightTrackShower");
    
    /// Default destructor
    ~SegWeightTrackShower(){}

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

    std::string _label_producer;

    std::string _weight_producer;

    std::string _keypt_pixel2d_producer;

    PoolType_t _pool_type;

    bool _weight_surrounding;

    size_t _dist_surrounding;

    size_t _weight_vertex;

    std::vector<size_t> _plane_v;

    unsigned int _weight_max;

  };

  /**
     \class larcv::SegWeightTrackShowerFactory
     \brief A concrete factory class for larcv::SegWeightTrackShower
  */
  class SegWeightTrackShowerProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SegWeightTrackShowerProcessFactory() { ProcessFactory::get().add_factory("SegWeightTrackShower",this); }
    /// dtor
    ~SegWeightTrackShowerProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SegWeightTrackShower(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

