/**
 * \file ImageMetaMaker.h
 *
 * \ingroup MeatSlicer
 * 
 * \brief Class def header for a class ImageMetaMaker
 *
 * @author kazuhiro
 */

/** \addtogroup MeatSlicer

    @{*/
#ifndef __IMAGEMETAMAKER_H__
#define __IMAGEMETAMAKER_H__

#include "ImageMetaMakerBase.h"

namespace larcv {
  class SuperaMetaMaker;
}

namespace supera {

  /**
     \class ImageMetaMaker
     User defined class ImageMetaMaker ... these comments are used to generate
     doxygen documentation!
  */
  class ImageMetaMaker{

    friend class larcv::SuperaMetaMaker;
    
  public:
    
    /// Default constructor
    ImageMetaMaker() : _meta_maker(nullptr)
    {}
    
    /// Default destructor
    virtual ~ImageMetaMaker(){ delete _meta_maker; }

    virtual void configure(const supera::Config_t&);
    
    const std::vector<larcv::ImageMeta>& Meta() const;

    const std::vector<size_t>& RowCompressionFactor() const;

    const std::vector<size_t>& ColCompressionFactor() const;

    inline ImageMetaMakerBase* MetaMakerPtr()
    { return _meta_maker; }

  private:

    ImageMetaMakerBase* _meta_maker;

    static ImageMetaMakerBase* _shared_meta_maker;
    inline static ImageMetaMakerBase* SharedMetaMaker() { return _shared_meta_maker; }
    inline static void SetSharedMetaMaker(ImageMetaMakerBase* ptr) { _shared_meta_maker = ptr; }

  };
}
#endif
/** @} */ // end of doxygen group 

