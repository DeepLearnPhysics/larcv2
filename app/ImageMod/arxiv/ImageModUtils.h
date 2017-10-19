/**
 * \file ImageModUtils.h
 *
 * \ingroup Package_Name
 * 
 * \brief header for utility functions
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __IMAGEMODUTILS_H__
#define __IMAGEMODUTILS_H__

#include "DataFormat/Image2D.h"
#include "DataFormat/Pixel2DCluster.h"

namespace larcv {

  Image2D cluster_to_image2d(const Pixel2DCluster& pcluster, size_t target_rows=0, size_t target_cols=0);

}

#endif
/** @} */ // end of doxygen group 

