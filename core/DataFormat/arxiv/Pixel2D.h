/**
 * \file Pixel2D.h
 *
 * \ingroup core_DataFormat
 * 
 * \brief Class def header for a class larcv::Pixel2D
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef PIXEL2D_H
#define PIXEL2D_H

#include <iostream>
#include "DataFormatTypes.h"
namespace larcv {
  /**
     \class Pixel2D
     User defined class Pixel2D ... these comments are used to generate
     doxygen documentation!
  */
  class Pixel2D {
    
  public:
    
    /// Default constructor
    Pixel2D(size_t x=0, size_t y=0) : _x(x) , _y(y) {}
    /// Default destructor
    ~Pixel2D(){}
    
    /// X getter
    size_t X() const { return _x; }
    /// Y getter
    size_t Y() const { return _y; }
    /// Intensity getter
    float Intensity() const { return _intensity; }
    /// X setter
    void X(size_t x) { _x = x; }
    /// Y setter
    void Y(size_t y) { _y = y; }
    /// Intensity setter
    void Intensity(float i) { _intensity = i; }
    /// X Y Intensity setter
    void Set(size_t x, size_t y, float i)
    { _x = x; _y = y; _intensity = i; }
    // set width
    void Width(float w) { _width = w; };
    // get width
    float Width() const { return _width; };

    //
    // uniry operators
    //
    inline Pixel2D& operator += (float intensity)
    { _intensity += intensity; return (*this); }
    inline Pixel2D& operator -= (float intensity)
    { _intensity -= intensity; return (*this); }
    inline Pixel2D& operator *= (float factor)
    { _intensity *= factor; return (*this); }
    inline Pixel2D& operator /= (float factor)
    { _intensity /= factor; return (*this); }

    //
    // binary operators
    //
    inline bool operator == (const Pixel2D& rhs) const
    { return (_x == rhs._x && _y == rhs._y); }    
    inline bool operator <  (const Pixel2D& rhs) const
    {
      if( _x < rhs._x) return true;
      if( _x > rhs._x) return false;
      if( _y < rhs._y) return true;
      if( _y > rhs._y) return false;
      return false;
    }
    inline bool operator <= (const Pixel2D& rhs) const
    { return  ((*this) == rhs || (*this) < rhs); }
    inline bool operator >  (const Pixel2D& rhs) const
    { return !((*this) <= rhs); }
    inline bool operator >= (const Pixel2D& rhs) const
    { return !((*this) <  rhs); }

    inline bool operator == (const float& rhs) const
    { return _intensity == rhs; }
    inline bool operator <  (const float& rhs) const
    { return _intensity <  rhs; }
    inline bool operator <= (const float& rhs) const
    { return _intensity <= rhs; }
    inline bool operator >  (const float& rhs) const
    { return _intensity >  rhs; }
    inline bool operator >= (const float& rhs) const
    { return _intensity >= rhs; }

  private:
    size_t _x; ///< X position
    size_t _y; ///< Y position
    float  _intensity; ///< Pixel Intensity
    float  _width; ///< Width (when using pixel to represent a hit)
  };
}

#endif
/** @} */ // end of doxygen group 

