/**
 * \file ImageMetaMakerBase.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ImageMetaMakerBase
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __IMAGEMETAMAKERBASE_H__
#define __IMAGEMETAMAKERBASE_H__

#include "Base/larcv_base.h"
#include "DataFormat/ImageMeta.h"
#include "FMWKInterface.h"

namespace supera {

  /**
     \class ImageMetaMakerBase
     User defined class ImageMetaMakerBase ... these comments are used to generate
     doxygen documentation!
  */
  class ImageMetaMakerBase : public larcv::larcv_base {

  public:
    
    /// Default constructor
    ImageMetaMakerBase(std::string name="NoName")
      : larcv_base(name)
    {}
    
    /// Default destructor
    virtual ~ImageMetaMakerBase(){}

    virtual void configure(const supera::Config_t&);

    virtual const std::vector<larcv::ImageMeta>& Meta() const = 0;
    const std::vector<size_t>& RowCompressionFactor() const { return _comp_rows; }
    const std::vector<size_t>& ColCompressionFactor() const { return _comp_cols; }

  private:

    std::vector<size_t> _comp_rows;
    std::vector<size_t> _comp_cols;
  };

}

#endif
/** @} */ // end of doxygen group 

