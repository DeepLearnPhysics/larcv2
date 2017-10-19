/**
 * \file ImageMetaFromConfig.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ImageMetaFromConfig
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __IMAGEMETAFROMCONFIG_H__
#define __IMAGEMETAFROMCONFIG_H__

#include "FMWKInterface.h"
#include "ImageMetaMakerBase.h"

namespace supera {

  /**
     \class ImageMetaFromConfig
     User defined class ImageMetaFromConfig ... these comments are used to generate
     doxygen documentation!
  */
  class ImageMetaFromConfig : public ImageMetaMakerBase {

  public:
    
    /// Default constructor
    ImageMetaFromConfig() : ImageMetaMakerBase("ImageMetaFromConfig")
    {}
    
    /// Default destructor
    ~ImageMetaFromConfig(){}

    void configure(const supera::Config_t&);

    const std::vector<larcv::ImageMeta>& Meta() const
    { return _meta_v; }

  private:

    std::vector<larcv::ImageMeta> _meta_v;

  };

}

#endif
/** @} */ // end of doxygen group 

