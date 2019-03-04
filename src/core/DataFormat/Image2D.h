/**
 * \file Image2D.h
 *
 * \ingroup core_DataFormat
 * 
 * \brief Class def header for an image data holder larcv::Image2D
 *
 * @author tmw, kazu
 */

/** \addtogroup core_DataFormat

    @{*/

#ifndef __LARCV_Image2D_H__
#define __LARCV_Image2D_H__

#include <vector>
#include <cstdlib>
#include "ImageMeta.h"
#include "BBox.h"

namespace larcv {

  /**
    \class Image2D
    Meant to be a storage class for an image into a ROOT file. Ultimately data is 1D array.
  */
  class Image2D {
    
  public:
    /// ctor by # rows and # columns
    Image2D(size_t row_count=0, size_t col_count=0);
    /// ctor from ImageMeta
    Image2D(const ImageMeta&);
    /// ctor from ImageMeta and 1D array data
    Image2D(const ImageMeta&, const std::vector<float>&);
    /// copy ctor
    Image2D(const Image2D&);
    
#ifndef __CINT__
    /// attribute move ctor
    Image2D(ImageMeta&&, std::vector<float>&&);
#endif
    /// dtor
    virtual ~Image2D(){}

    /// Reset contents w/ new larcv::ImageMeta
    void reset(const ImageMeta&);
    /// Various modes used to combine pixels
    enum CompressionModes_t { kSum, kAverage, kMaxPool, kOverWrite};
    /// Move origin position
    void reset_origin(double x, double y) {_meta.reset_origin(x,y);}
    /// Return image index ID number (should be unique within larcv::EventImage2D)
    ImageIndex_t index() const { return _id; }
    /// Index setter
    void index(ImageIndex_t n) { _id = n; }
    /// Size of data, equivalent of # rows x # columns
    size_t size() const { return _img.size(); }
    /// Specific pixel value getter
    float pixel(size_t row, size_t col) const;
    /// larcv::ImageMeta const reference getter
    const ImageMeta& meta() const { return _meta; }
    /// Compress image data and returns compressed data 1D array
    std::vector<float> copy_compress(size_t row_count, size_t col_count, CompressionModes_t mode=kSum) const;
    /// Mem-copy: insert num_pixel many data from src 1D array @ data index starting from (row,col)
    void copy(size_t row, size_t col, const float* src, size_t num_pixel);
    /// Mem-copy: insert num_pixel many data from src 1D array @ data index starting from (row,col)
    void copy(size_t row, size_t col, const std::vector<float>& src, size_t num_pixel=0);
    /// Fake mem-copy (loop element-wise copy): insert num_pixel many data from src 1D array @ data index starting from (row,col)
    void copy(size_t row, size_t col, const short* src, size_t num_pixel);
    /// Fake mem-copy (loop element-wise copy): insert num_pixel many data from src 1D array @ data index starting from (row,col)
    void copy(size_t row, size_t col, const std::vector<short>& src, size_t num_pixel=0);
    /// Same as copy, but perform in reverse direction of rows (useful when src is not in the same order)
    void reverse_copy(size_t row, size_t col, const std::vector<float>& src, size_t nskip=0, size_t num_pixel=0);
    /// Same as copy, but perform in reverse direction of rows (useful when src is not in the same order)
    void reverse_copy(size_t row, size_t col, const std::vector<short>& src, size_t nskip=0, size_t num_pixel=0);
    /// Crop specified region via crop_meta to generate a new larcv::Image2D
    Image2D crop(const ImageMeta& crop_meta) const;
    /// Crop specified region via bbox to generate a new larcv::Image2D
    Image2D crop(const BBox2D& bbox,const DistanceUnit_t unit) const;
    /// 1D const reference array getter
    const std::vector<float>& as_vector() const 
    { return _img; }
    /// Re-size the 1D data array w/ updated # rows and # columns
    void resize( size_t row_count, size_t col_count, float fillval=0.0 );
    /// Set pixel value via row/col specification
    void set_pixel( size_t row, size_t col, float value );
    /// Set pixel value via index specification
    void set_pixel( size_t index, float value );
    /// Paint all pixels with a specified value
    void paint(float value);
    /// Paint a row of pixels with a specified value
    void paint_row( int row, float value );
    /// Paint a column of pixels with a specified value
    void paint_col( int col, float value );
    /// Apply threshold: pixels lower than "thres" are all overwritten by lower_overwrite value
    void threshold(float thresh, bool lower);
    /// Apply threshold: make all pixels to take only 2 values, lower_overwrite or upper_overwrite 
    void binarize(float thresh, float lower_overwrite, float upper_overwrite);
    /// Clear data contents
    void clear_data();
    /// Call copy_compress internally and set itself to the result
    void compress(size_t row_count, size_t col_count, CompressionModes_t mode=kSum);
    /// Overlay with another Image2D: overlapped pixel region is merged
    void overlay(const Image2D&, CompressionModes_t mode=kSum);
    /// Move data contents out
    std::vector<float>&& move();
    /// Move data contents in
    void move(std::vector<float>&&);

    inline Image2D& operator+=(const float val)
    { for(auto& v : _img) v+= val; return (*this);}
    inline Image2D operator+(const float val) const
    { Image2D res = (*this); res+=val; return res; }
    inline Image2D& operator-=(const float val)
    { for(auto& v : _img) v-= val; return (*this);}
    inline Image2D operator-(const float val) const
    { Image2D res = (*this); res-=val; return res; }
    inline Image2D& operator*=(const float val)
    { for(auto& v : _img) v*= val; return (*this);}
    inline Image2D operator*(const float val) const
    { Image2D res = (*this); res*=val; return res; }
    inline Image2D& operator/=(const float val)
    { for(auto& v : _img) v/= val; return (*this);}
    inline Image2D operator/(const float val) const
    { Image2D res = (*this); res/=val; return res; }

    Image2D& operator +=(const std::vector<float>& rhs);
    Image2D& operator -=(const std::vector<float>& rhs);
    Image2D& operator +=(const larcv::Image2D& rhs);

    // Matrix Multiplication
    /// Matrix multiplicaition
    Image2D multiRHS( const Image2D& rhs ) const; 
    /// uniry operator for matrix multiplicaition
    Image2D& operator*=( const Image2D& rhs );
    /// binary operator for matrix multiplication
    Image2D operator*(const Image2D& rhs) const;

    /// Element-wise pixel value multiplication
    void eltwise( const Image2D& rhs );
    /// Element-wise multiplication w/ 1D array data
    void eltwise(const std::vector<float>& arr,bool allow_longer=false);
    
  private:
    std::vector<float> _img;
    ImageIndex_t _id;
    ImageMeta _meta;
    void clear();
  };

}

#endif
/** @} */ // end of doxygen group 
