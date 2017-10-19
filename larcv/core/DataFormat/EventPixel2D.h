/**
 * \file EventPixel2D.h
 *
 * \ingroup core_DataFormat
 * 
 * \brief Class def header for a class larcv::EventPixel2D
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef EVENTPIXEL2D_H
#define EVENTPIXEL2D_H

#include <iostream>
#include "EventBase.h"
#include "DataProductFactory.h"
#include "Voxel.h"
#include "ImageMeta.h"
namespace larcv {
  
  /**
    \class EventPixel2D
    \brief Event-wise class to store a collection of larcv::Pixel2D and larcv::Pixel2DCluster
  */
  class EventPixel2D : public EventBase,
		       public VoxelSet {
    
  public:
    
    /// Default constructor
    EventPixel2D(){}
    
    /// Default destructor
    virtual ~EventPixel2D(){}
    
    /// EventBase::clear() override
    void clear();

    /// Meta getter
    inline const ImageMeta& Meta() const
    { return _meta; }

    /// Meta setter
    inline void Meta(const ImageMeta& meta)
    { VoxelSet::Clear(); _meta = meta; }

  private:
    ImageMeta _meta;
  };
}

#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventPixel2D>() { return "pixel2d"; }
  template EventPixel2D& IOManager::get_data<larcv::EventPixel2D>(const std::string&);
  template EventPixel2D& IOManager::get_data<larcv::EventPixel2D>(const ProducerID_t);

  /**
     \class larcv::EventPixel2D
     \brief A concrete factory class for larcv::EventPixel2D
  */
  class EventPixel2DFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventPixel2DFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventPixel2D>(),this); }
    /// dtor
    ~EventPixel2DFactory() {}
    /// create method
    EventBase* create() { return new EventPixel2D; }
  };

}

#endif
/** @} */ // end of doxygen group 

