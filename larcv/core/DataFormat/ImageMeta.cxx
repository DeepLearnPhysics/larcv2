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
    if (x < min_x() || x >= max_x()) {
      std::stringstream ss;
      ss << "Requested col for x=" << x << " ... but the x (rows) span only " << min_x() << " => " << max_x() << "!" << std::endl;
      throw larbys(ss.str());
    }
    return (size_t)((x - min_x()) / pixel_width());
  }

  size_t ImageMeta::row(double y) const
  {
    if (y < min_y() || y >= max_y()) {
      std::stringstream ss;
      ss << "Requested col for y=" << y << " ... but the y (cols) spans only " << min_y() << " => " << max_y() << "!" << std::endl;
      throw larbys(ss.str());
    }
    return (size_t)((y - min_y()) / pixel_height());
  }

  ImageMeta ImageMeta::overlap(const ImageMeta& meta) const
  {
    auto box = BBox2D::overlap((BBox2D)meta);

    return ImageMeta(box.width(), box.height(),
                     box.height() / pixel_height(),
                     box.width() / pixel_width(),
                     box.min_x(), box.min_y(), id(), _unit);
  }

  ImageMeta ImageMeta::inclusive(const ImageMeta& meta) const
  {
    auto box = BBox2D::inclusive((BBox2D)meta);

    return ImageMeta(box.width(), box.height(),
                     box.height() / pixel_height(),
                     box.width() / pixel_width(),
                     box.min_x(), box.min_y(), id(), _unit);
  }

  std::string ImageMeta::dump() const
  {
    std::stringstream ss;
    ss << "ProjectionID " << id() << " (rows,cols) = (" << _row_count << "," << _col_count
       << ") ... Left Bottom (" << min_x() << "," << min_y()
       << ") ... Right Top (" << max_x() << "," << max_y()
       << ")" << std::endl;
    return ss.str();
  }
}

#endif
