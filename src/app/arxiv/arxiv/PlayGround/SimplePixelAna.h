/**
 * \file SimplePixelAna.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class SimplePixelAna
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SIMPLEPIXELANA_H__
#define __SIMPLEPIXELANA_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class SimplePixelAna ... these comments are used to generate
     doxygen documentation!
  */
  class SimplePixelAna : public ProcessBase {

  public:
    
    /// Default constructor
    SimplePixelAna(const std::string name="SimplePixelAna");
    
    /// Default destructor
    ~SimplePixelAna(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    std::string _image_name;       ///< Image2D producer name
    TTree* _tree;                  ///< TTree for analysis later
    unsigned short _image_index;   ///< Image index value
    float  _max_pixel;             ///< Analysis variable: Max pixel value
    float  _pixel_sum;             ///< Analysis variable: Sum of pixel values
    int    _pixel_count;           ///< Analysis variable: Pixel count above a certain threshold
    float  _pixel_count_threshold; ///< Analysis variable: Threshold to increment a pixel count
  };

  /**
     \class larcv::SimplePixelAnaFactory
     \brief A concrete factory class for larcv::SimplePixelAna
  */
  class SimplePixelAnaProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SimplePixelAnaProcessFactory() { ProcessFactory::get().add_factory("SimplePixelAna",this); }
    /// dtor
    ~SimplePixelAnaProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SimplePixelAna(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

