/**
 * \file ImageMeta.h
 *
 * \ingroup core_DataFormat
 * 
 * \brief Class def header for a class larcv::ImageMeta
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef __LARCV_IMAGEMETA_H__
#define __LARCV_IMAGEMETA_H__

#include <iostream>
#include "Base/larbys.h"
//#include "Base/LArCVTypes.h"
#include "DataFormatTypes.h"
namespace larcv {

  class Image2D;
  /**
     \class ImageMeta
     A simple class to store image's meta data including\n
     0) origin (left-top corner of the picture) absolute coordinate \n
     1) horizontal and vertical size (width and height) in double precision \n
     2) number of horizontal and vertical pixels \n
     It is meant to be associated with a specific cv::Mat or larcv::Image2D object \n
     (where the latter class contains ImageMeta as an attribute). For cv::Mat, there \n
     is a function ImageMeta::update to constantly update vertical/horizontal # pixels \n
     as it may change in the course of matrix operation.
  */
  class ImageMeta{

    friend class Image2D;

  public:
    
    /// Default constructor: width, height, and origin coordinate won't be modifiable 
    ImageMeta(const double width=0.,     const double height=0.,
	      const size_t row_count=0., const size_t col_count=0,
	      const double origin_x=0.,  const double origin_y=0.,
	      const PlaneID_t plane=::larcv::kINVALID_PLANE)
      : _image_id (kINVALID_INDEX)
      , _origin (origin_x,origin_y)
      , _width  (width)
      , _height (height)
      , _plane  (plane)
    {
      if( width  < 0. ) throw larbys("Width must be a positive floating point!");
      if( height < 0. ) throw larbys("Height must be a positive floating point!");
      update(row_count,col_count);
    }
    
    /// Default destructor
    ~ImageMeta(){}

    inline bool operator== (const ImageMeta& rhs) const
    {
      return ( _origin.x  == rhs._origin.x  &&
	       _origin.y  == rhs._origin.y  &&
	       _width     == rhs._width     &&
	       _height    == rhs._height    &&
	       _plane     == rhs._plane     &&
	       _row_count == rhs._row_count &&
	       _col_count == rhs._col_count );
    }

    inline bool operator!= (const ImageMeta& rhs) const
    { return !((*this) == rhs); }

    ImageIndex_t image_index() const  { return _image_id; }
    void image_index(ImageIndex_t id) { _image_id = id;   }
    /// Top-left corner point
    const Point2D& tl   () const { return _origin;                                          }
    /// Bottom-left corner point
    const Point2D  bl   () const { return Point2D(_origin.x,          _origin.y - _height); }
    /// Top-right corner point
    const Point2D  tr   () const { return Point2D(_origin.x + _width, _origin.y          ); }
    /// Bottom-right corner point
    const Point2D  br   () const { return Point2D(_origin.x + _width, _origin.y - _height); }
    /// PlaneID_t getter
    PlaneID_t plane     () const { return _plane;     }
    /// Width accessor
    double width        () const { return _width;     }
    /// Height accessor
    double height       () const { return _height;    }
    /// # rows accessor
    size_t rows         () const { return _row_count; }
    /// # columns accessor
    size_t cols         () const { return _col_count; }
    /// Pixel horizontal size
    double pixel_width  () const { return (_col_count ? _width  / (double)_col_count : 0.); } 
    /// Pixel vertical size
    double pixel_height () const { return (_row_count ? _height / (double)_row_count : 0.); }
    /// 2D length unit
    DistanceUnit_t unit       () const { return _unit; }

    /// Provide 1-D array index from row and column
    size_t index( size_t row, size_t col ) const;
    /// Provide absolute scale min x
    double min_x() const { return _origin.x; }
    /// Provide absolute scale max x
    double max_x() const { return _origin.x + _width; }
    /// Provide absolute scale min y
    double min_y() const { return _origin.y - _height; }
    /// Provide absolute scale max y
    double max_y() const { return _origin.y; }
    /// Provide absolute horizontal coordinate of the center of a specified pixel row
    double pos_x   (size_t col) const { return _origin.x + pixel_width() * col; }
    /// Provide absolute vertical coordinate of the center of a specified pixel row
    double pos_y   (size_t row) const { return _origin.y - pixel_height() * row; }
    /// Provide horizontal pixel index for a given horizontal x position (in absolute coordinate)
    size_t col (double x) const;
    /// Provide vertical pixel index for a given vertical y position (in absolute coordinate)
    size_t row (double y) const;
    /// Change # of vertical/horizontal pixels in meta data
    void update(size_t row_count, size_t col_count){
      _row_count = row_count;
      _col_count = col_count;
    }
    /// Reset origin coordinate
    void   reset_origin(double x, double y) { _origin = Point2D(x,y); }
    /// Check if there's an overlap. If so return overlapping bounding box
    ImageMeta overlap(const ImageMeta& meta) const;
    /// Construct a union bounding box
    ImageMeta inclusive(const ImageMeta& meta) const;

    /// Dump info in text
    std::string dump() const;

  protected:

    ImageIndex_t   _image_id; ///< Associated image ID (of the same producer name)
    larcv::Point2D _origin;   ///< Absolute coordinate of the left top corner of an image
    double    _width;         ///< Horizontal size of an image in double floating precision (in original coordinate unit size)
    double    _height;        ///< Vertical size of an image in double floating precision (in original coordinate unit size)
    size_t    _col_count;     ///< # of pixels in horizontal axis
    size_t    _row_count;     ///< # of pixels in vertical axis
    PlaneID_t _plane;         ///< unique plane ID number
    DistanceUnit_t  _unit;
  };

}

#endif
/** @} */ // end of doxygen group 

