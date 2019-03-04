/**
 * \file Pixel2DCluster.h
 *
 * \ingroup core_DataFormat
 * 
 * \brief Class def header for a class larcv::Pixel2DCluster
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef PIXEL2DCLUSTER_H
#define PIXEL2DCLUSTER_H

#include <iostream>
#include "DataFormatTypes.h"
#include "Pixel2D.h"
#include "ImageMeta.h"
namespace larcv {

  class EventPixelCollection;

  /**
     \class Pixel2DCluster
     User defined class Pixel2DCluster ... these comments are used to generate
     doxygen documentation!
  */
  class Pixel2DCluster : public std::vector<larcv::Pixel2D> {

    friend class EventPixel2D;

  public:
    
    /// Default constructor
    Pixel2DCluster()
      : std::vector<larcv::Pixel2D>() {}
    /// Alternative constructor 1
    Pixel2DCluster(std::vector<larcv::Pixel2D>&& rhs)
      : std::vector<larcv::Pixel2D>(std::move(rhs)) {}
    /// Alternative constructor 2
    Pixel2DCluster(const std::vector<larcv::Pixel2D>& rhs)
      : std::vector<larcv::Pixel2D>(rhs) {}
    /// Default destructor
    ~Pixel2DCluster(){}

    // Pixel Intensity Sum
    double Intensity() const
    { double res = 0.;
      for(auto const& px : (*this)) { res += px.Intensity(); }
      return res;
    }

    // Cluster ID
    size_t ID() const
    { return _id; }

    // Pool among duplicate pixels
    void Pool(const PoolType_t type);

    // 2D Bounds 
    float min_x() const;
    float min_y() const;
    float max_x() const;
    float max_y() const;
    size_t argmin_x() const;
    size_t argmin_y() const;
    size_t argmax_x() const;
    size_t argmax_y() const;
    ImageMeta bounds() const;
    // Intensity bounds
    float min() const;
    float max() const;
    float std() const;
    float mean() const;
    size_t argmax() const;
    size_t argmin() const;

    //
    // uniry operator
    //
    inline Pixel2DCluster& operator += (const Pixel2D& rhs)
    { this->push_back(rhs); return (*this); }
    inline Pixel2DCluster& operator += (const Pixel2DCluster& rhs)
    { for(auto const& px : rhs) { (*this) += px; }; return (*this); }

  private:

    size_t _id;
    
  };
}

#endif
/** @} */ // end of doxygen group 

