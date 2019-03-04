#ifndef __LARCAFFE_IMAGEMETA_CXX__
#define __LARCAFFE_IMAGEMETA_CXX__

#include "ImageMeta.h"
#include <sstream>
namespace larcv {

  size_t ImageMeta::index( size_t row, size_t col ) const {

    if ( row >= _row_count || col >= _col_count ) {
      std::stringstream ss;
      ss << "Invalid pixel index queried: (" << row << "," << col << ") but the dimension is only (" << _row_count << "," << _col_count << ")!" << std::endl;
      throw larbys(ss.str());
    }
    return ( col * _row_count + row );
  }

  size_t ImageMeta::col(double x) const
  {
    if (x < min_x() || x > max_x()) {
      std::stringstream ss;
      ss << "Requested col for x=" << x << " ... but the x (cols) span only " << min_x() << " => " << max_x() << "!" << std::endl;
      throw larbys(ss.str());
    }
    if(x == max_x()) return (cols() - 1);
    return (size_t)((x - min_x()) / pixel_width());
  }

  size_t ImageMeta::row(double y) const
  {
    if (y < min_y() || y > max_y()) {
      std::stringstream ss;
      ss << "Requested row for y=" << y << " ... but the y (rows) spans only " << min_y() << " => " << max_y() << "!" << std::endl;
      throw larbys(ss.str());
    }
    if(y == max_y()) return (rows() - 1);
    return (size_t)((y - min_y()) / pixel_height());
  }

  size_t ImageMeta::index_to_row(size_t index) const
  {
    if ( index >= _row_count * _col_count ) {
      std::stringstream ss;
      ss << "Invalid pixel index queried: (" << index << ") but the dimension is only (" << _row_count * _col_count << ")!" << std::endl;
      throw larbys(ss.str());
    }
    return ( index % _row_count  );
  }

  size_t ImageMeta::index_to_col(size_t index) const
  {
    if ( index >= _row_count * _col_count ) {
      std::stringstream ss;
      ss << "Invalid pixel index queried: (" << index << ") but the dimension is only (" << _row_count * _col_count << ")!" << std::endl;
      throw larbys(ss.str());
    }
    return ( index / _row_count );
  }

  void ImageMeta::index_to_rowcol(size_t index, size_t& row, size_t& col) const
  {
     if ( index >= _row_count * _col_count ) {
      std::stringstream ss;
      ss << "Invalid pixel index queried: (" << index << ") but the dimension is only (" << _row_count * _col_count << ")!" << std::endl;
      throw larbys(ss.str());
    }
    row = index % _row_count;
    col = index / _row_count;
  }


  ImageMeta ImageMeta::overlap(const ImageMeta& meta) const
  {
    auto box = BBox2D::overlap((BBox2D)meta);

    return ImageMeta(box,
                     box.height() / pixel_height(),
                     box.width() / pixel_width(),
                     _unit);
  }

  ImageMeta ImageMeta::inclusive(const ImageMeta& meta) const
  {
    auto box = BBox2D::inclusive((BBox2D)meta);

    return ImageMeta(box,
                     box.height() / pixel_height(),
                     box.width() / pixel_width(),
                     _unit);
  }

  std::string ImageMeta::dump() const
  {
    std::stringstream ss;
    ss << "ProjectionID " << id() << " (rows,cols) = (" << _row_count << "," << _col_count
       << ") ... Distance Unit: " << (int)(this-> unit())
       << " ... Left Bottom => Right Top " << ((BBox2D*)(this))->dump();
    return ss.str();
  }
}

#endif
