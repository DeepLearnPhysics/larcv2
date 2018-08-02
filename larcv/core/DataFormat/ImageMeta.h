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
#include "larcv/core/Base/larbys.h"
#include "BBox.h"
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
  class ImageMeta : public BBox2D {

    friend class Image2D;

  public:

    /// Default constructor: width, height, and origin coordinate won't be modifiable
    /*
    ImageMeta(const double width = 0.,     const double height = 0.,
              const size_t row_count = 0., const size_t col_count = 0,
              const double origin_x = 0.,  const double origin_y = 0.,
              const ProjectionID_t id =::larcv::kINVALID_PROJECTIONID,
              const DistanceUnit_t unit = kUnitUnknown)
      : BBox2D(origin_x, origin_y, origin_x + width, origin_y + height, id)
    {
      if ( width  < 0. ) throw larbys("Width must be a positive floating point!");
      if ( height < 0. ) throw larbys("Height must be a positive floating point!");
      update(row_count, col_count);
    }
    */
    ImageMeta(double x_min = 0., double y_min = 0., double x_max = 0., double y_max = 0.,
	      size_t y_row_count = 0, size_t x_column_count = 0,
	      ProjectionID_t id = larcv::kINVALID_PROJECTIONID,
	      DistanceUnit_t unit = kUnitUnknown)
      : BBox2D(x_min, y_min, x_max, y_max, id)
      , _unit(unit)
    { update(y_row_count, x_column_count); }

    ImageMeta(const BBox2D& box,
              size_t y_row_count = 0, size_t x_column_count = 0,
              DistanceUnit_t unit = kUnitUnknown)
      : BBox2D(box), _unit(unit)
    { update(y_row_count, x_column_count); }

    /// Default destructor
    ~ImageMeta() {}

    inline bool operator== (const ImageMeta& rhs) const
    {
      return ( (BBox2D)(*this) == (BBox2D)(rhs) &&
               _row_count == rhs._row_count &&
               _col_count == rhs._col_count );
    }

    inline bool operator!= (const ImageMeta& rhs) const
    { return !((*this) == rhs); }

    /// size (#rows * #cols) accessor
    inline size_t size         () const { return _row_count * _col_count; }
    /// # rows accessor
    inline size_t rows         () const { return _row_count; }
    /// # columns accessor
    inline size_t cols         () const { return _col_count; }
    /// Pixel horizontal size
    inline double pixel_width  () const { return (_col_count ? width()  / (double)_col_count : 0.); }
    /// Pixel vertical size
    inline double pixel_height () const { return (_row_count ? height() / (double)_row_count : 0.); }
    /// 2D length unit
    inline DistanceUnit_t unit () const { return _unit; }

    /// Provide 1-D array index from row and column
    size_t index( size_t row, size_t col ) const;
    /// Provide absolute coordinate of the center of a specified pixel index
    inline Point2D position (size_t index) const { return Point2D(pos_x(index / rows()), pos_y(index % rows())); }
    /// Provide absolute coordinate of the center of a specified pixel (row,col)
    inline Point2D position (size_t row, size_t col) const { return Point2D(index(row, col)); }
    /// Provide absolute horizontal coordinate of the center of a specified pixel row
    inline double pos_x   (size_t col) const { return min_x() + pixel_width() * col; }
    /// Provide absolute vertical coordinate of the center of a specified pixel row
    inline double pos_y   (size_t row) const { return min_y() + pixel_height() * row; }
    /// Provide horizontal pixel index for a given horizontal x position (in absolute coordinate)
    size_t col (double x) const;
    /// Provide vertical pixel index for a given vertical y position (in absolute coordinate)
    size_t row (double y) const;
    /// Change # of vertical/horizontal pixels in meta data
    inline void update(size_t row_count, size_t col_count) {_row_count = row_count; _col_count = col_count;}
    /// Reset origin coordinate
    inline void reset_origin(double x, double y) { BBox2D::update(x, y, max_x(), max_y()); }
    /// Check if there's an overlap. If so return overlapping bounding box
    ImageMeta overlap(const ImageMeta& meta) const;
    /// Construct a union bounding box
    ImageMeta inclusive(const ImageMeta& meta) const;

    /// Find row that corresponds to a specified index
    size_t index_to_row(size_t index) const;
    /// Find col that corresponds to a specified index
    size_t index_to_col(size_t index) const;
    /// Find row and col that corresponds to a specified index
    void index_to_rowcol(size_t index, size_t& row, size_t& col) const;
    /// Dump info in text
    std::string dump() const;

  protected:

    ImageIndex_t   _image_id;  ///< Associated image ID (of the same producer name)
    size_t         _col_count; ///< # of pixels in horizontal axis
    size_t         _row_count; ///< # of pixels in vertical axis
    DistanceUnit_t _unit;      ///< distance unit defined in DataFormatTypes.h
  };

}

#endif
/** @} */ // end of doxygen group
