/**
 * \file NNCosine.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class NNCosine
 *
 * @author twongjirad
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __NNCOSINE_H__
#define __NNCOSINE_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include <vector>
#include <string>

namespace larcv {

  /**
     \class ProcessBase
     User defined class NNCosine ... these comments are used to generate
     doxygen documentation!
  */
  class NNCosine : public ProcessBase {

  public:
    
    /// Default constructor
    NNCosine(const std::string name="NNCosine");
    
    /// Default destructor
    ~NNCosine(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    std::string finput_producer;
    std::string foutput_producer;
    std::vector<float> fthresholds;
    std::vector<int> fminhits;
    std::vector<float> fradii;
    bool fmakehitmarker;
    std::string fhitmarkerpro;

  };

  /**
     \class larcv::NNCosineFactory
     \brief A concrete factory class for larcv::NNCosine
  */
  class NNCosineProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    NNCosineProcessFactory() { ProcessFactory::get().add_factory("NNCosine",this); }
    /// dtor
    ~NNCosineProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new NNCosine(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

