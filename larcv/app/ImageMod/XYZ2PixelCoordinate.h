/**
 * \file XYZ2PixelCoordinate.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class XYZ2PixelCoordinate
 *
 * @author kterao
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __XYZ2PIXELCOORDINATE_H__
#define __XYZ2PIXELCOORDINATE_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class XYZ2PixelCoordinate ... these comments are used to generate
     doxygen documentation!
  */
  class XYZ2PixelCoordinate : public ProcessBase {

  public:
    
    /// Default constructor
    XYZ2PixelCoordinate(const std::string name="XYZ2PixelCoordinate");
    
    /// Default destructor
    ~XYZ2PixelCoordinate(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    std::string _tensor3d_producer, _particle_producer, _output_producer;
    
  };

  /**
     \class larcv::XYZ2PixelCoordinateFactory
     \brief A concrete factory class for larcv::XYZ2PixelCoordinate
  */
  class XYZ2PixelCoordinateProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    XYZ2PixelCoordinateProcessFactory() { ProcessFactory::get().add_factory("XYZ2PixelCoordinate",this); }
    /// dtor
    ~XYZ2PixelCoordinateProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new XYZ2PixelCoordinate(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

