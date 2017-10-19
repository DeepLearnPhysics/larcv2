/**
 * \file EmptyImageFilter.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class EmptyImageFilter
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __EMPTYIMAGEFILTER_H__
#define __EMPTYIMAGEFILTER_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class EmptyImageFilter ... these comments are used to generate
     doxygen documentation!
  */
  class EmptyImageFilter : public ProcessBase {

  public:
    
    /// Default constructor
    EmptyImageFilter(const std::string name="EmptyImageFilter");
    
    /// Default destructor
    ~EmptyImageFilter(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    std::string _image_producer;

  };

  /**
     \class larcv::EmptyImageFilterFactory
     \brief A concrete factory class for larcv::EmptyImageFilter
  */
  class EmptyImageFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    EmptyImageFilterProcessFactory() { ProcessFactory::get().add_factory("EmptyImageFilter",this); }
    /// dtor
    ~EmptyImageFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new EmptyImageFilter(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

