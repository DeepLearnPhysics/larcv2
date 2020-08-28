/**
 * \file Supera.h
 *
 * \ingroup Supera
 * 
 * \brief Class def header for a class Supera
 *
 * @author kterao
 */

/** \addtogroup Supera

    @{*/
#ifndef __SUPERA_H__
#define __SUPERA_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"

#include "EDepSim/TG4Event.h"
#include "EDepSim/TG4HitSegment.h"
#include "EDepSim/TG4PrimaryVertex.h"
#include "EDepSim/TG4Trajectory.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class Supera ... these comments are used to generate
     doxygen documentation!
  */
  class Supera : public ProcessBase {

  public:
    
    /// Default constructor
    Supera(const std::string name="Supera");
    
    /// Default destructor
    ~Supera(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    void Points(const ::TG4Event& ev)
    { std::cout<<ev.Trajectories.size()<<std::endl; }
      
      
  };

  /**
     \class larcv::SuperaFactory
     \brief A concrete factory class for larcv::Supera
  */
  class SuperaProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SuperaProcessFactory() { ProcessFactory::get().add_factory("Supera",this); }
    /// dtor
    ~SuperaProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Supera(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

