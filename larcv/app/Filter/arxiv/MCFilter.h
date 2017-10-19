/**
 * \file MCFilter.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class MCFilter
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __MCFILTER_H__
#define __MCFILTER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class MCFilter ... these comments are used to generate
     doxygen documentation!
  */
  class MCFilter : public ProcessBase {

  public:
    
    /// Default constructor
    MCFilter(const std::string name="MCFilter");
    
    /// Default destructor
    ~MCFilter(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    bool _select_ccqe;
    bool _select_cc;
    std::string _roi_producer;
  };

  /**
     \class larcv::MCFilterFactory
     \brief A concrete factory class for larcv::MCFilter
  */
  class MCFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    MCFilterProcessFactory() { ProcessFactory::get().add_factory("MCFilter",this); }
    /// dtor
    ~MCFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new MCFilter(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

