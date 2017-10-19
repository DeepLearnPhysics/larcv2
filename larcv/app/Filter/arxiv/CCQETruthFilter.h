/**
 * \file CCQETruthFilter.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class CCQETruthFilter
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __CCQETRUTHFILTER_H__
#define __CCQETRUTHFILTER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include <TTree.h>
namespace larcv {

  /**
     \class ProcessBase
     User defined class CCQETruthFilter ... these comments are used to generate
     doxygen documentation!
  */
  class CCQETruthFilter : public ProcessBase {

  public:
    
    /// Default constructor
    CCQETruthFilter(const std::string name="CCQETruthFilter");
    
    /// Default destructor
    ~CCQETruthFilter(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    std::string _roi_producer;
    double _nu_energy_max;
    double _nu_energy_min;
    TTree* _tree;
    int _pdg;
    double _energy;
    double _x;
    double _y;
    double _z;
    int _type;
    double _energy_init;
    double _energy_deposit;
  };

  /**
     \class larcv::CCQETruthFilterFactory
     \brief A concrete factory class for larcv::CCQETruthFilter
  */
  class CCQETruthFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    CCQETruthFilterProcessFactory() { ProcessFactory::get().add_factory("CCQETruthFilter",this); }
    /// dtor
    ~CCQETruthFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new CCQETruthFilter(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

