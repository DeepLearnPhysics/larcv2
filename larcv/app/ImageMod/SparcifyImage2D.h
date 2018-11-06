/**
 * \file SparcifyImage2D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class SparcifyImage2D
 *
 * @author kterao
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __SPARCIFYIMAGE2D_H__
#define __SPARCIFYIMAGE2D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class SparcifyImage2D ... these comments are used to generate
     doxygen documentation!
  */
  class SparcifyImage2D : public ProcessBase {

  public:
    
    /// Default constructor
    SparcifyImage2D(const std::string name="SparcifyImage2D");
    
    /// Default destructor
    ~SparcifyImage2D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _image2d_producer;
    size_t _image2d_channel;
    std::string _output_producer;

  };

  /**
     \class larcv::SparcifyImage2DFactory
     \brief A concrete factory class for larcv::SparcifyImage2D
  */
  class SparcifyImage2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SparcifyImage2DProcessFactory() { ProcessFactory::get().add_factory("SparcifyImage2D",this); }
    /// dtor
    ~SparcifyImage2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SparcifyImage2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

