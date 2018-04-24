/**
 * \file MCTrackFix.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class MCTrackFix
 *
 * @author drinkingkazu
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __MCTRACKFIX_H__
#define __MCTRACKFIX_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/DataFormat/Voxel3DMeta.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class MCTrackFix ... these comments are used to generate
     doxygen documentation!
  */
  class MCTrackFix : public ProcessBase {

  public:
    
    /// Default constructor
    MCTrackFix(const std::string name="MCTrackFix");
    
    /// Default destructor
    ~MCTrackFix(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    bool TouchingWall(const Voxel3DMeta& meta3d, 
		      const Point3D& pt) const;

    std::string _particle_producer;
    std::string _cluster3d_producer;

  };

  /**
     \class larcv::MCTrackFixFactory
     \brief A concrete factory class for larcv::MCTrackFix
  */
  class MCTrackFixProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    MCTrackFixProcessFactory() { ProcessFactory::get().add_factory("MCTrackFix",this); }
    /// dtor
    ~MCTrackFixProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new MCTrackFix(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

