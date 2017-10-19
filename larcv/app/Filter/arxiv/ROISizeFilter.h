/**
 * \file ROISizeFilter.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ROISizeFilter
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __ROISIZEFILTER_H__
#define __ROISIZEFILTER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ROISizeFilter ... these comments are used to generate
     doxygen documentation!
  */
  class ROISizeFilter : public ProcessBase {

  public:
    
    /// Default constructor
    ROISizeFilter(const std::string name="ROISizeFilter");
    
    /// Default destructor
    ~ROISizeFilter(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _roi_producer;
    std::vector<double> _min_height;
    std::vector<double> _min_width;
    std::vector<double> _min_area;

  };

  /**
     \class larcv::ROISizeFilterFactory
     \brief A concrete factory class for larcv::ROISizeFilter
  */
  class ROISizeFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ROISizeFilterProcessFactory() { ProcessFactory::get().add_factory("ROISizeFilter",this); }
    /// dtor
    ~ROISizeFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ROISizeFilter(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

