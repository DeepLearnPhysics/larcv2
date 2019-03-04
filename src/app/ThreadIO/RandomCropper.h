/**
 * \file RandomCropper.h
 *
 * \ingroup APICaffe
 * 
 * \brief Class def header for a class RandomCropper
 *
 * @author kazuhiro
 */

/** \addtogroup APICaffe

    @{*/
#ifndef RANDOMCROPPER_H
#define RANDOMCROPPER_H

#include <iostream>
#include "larcv/core/Base/larcv_base.h"
#include "larcv/core/Base/PSet.h"
#include "larcv/core/DataFormat/Image2D.h"

namespace larcv {
  /**
     \class RandomCropper
     User defined class RandomCropper ... these comments are used to generate
     doxygen documentation!
  */
  class RandomCropper : public larcv_base {
    
  public:
    
    /// Default constructor
    RandomCropper(const std::string name="RandomCropper");
    
    /// Default destructor
    ~RandomCropper(){}

    void configure(const PSet& cfg);

    void set_crop_region(const size_t rows, const size_t cols);

    size_t rows() const { return _crop_rows; }

    size_t cols() const { return _crop_cols; }

    const std::vector<float> crop(const larcv::Image2D& image);
    
  private:

    size_t _target_rows;
    size_t _target_cols;
    size_t _crop_rows;
    size_t _crop_cols;
    size_t _col_offset;
    size_t _row_offset;
    bool   _randomize_crop;
    std::vector<float> _cropped_image;
  };
}

#endif
/** @} */ // end of doxygen group 

