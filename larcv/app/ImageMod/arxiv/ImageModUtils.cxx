#ifndef __IMAGEMODUTILS_CXX__
#define __IMAGEMODUTILS_CXX__

#include "ImageModUtils.h"

namespace larcv {

  Image2D cluster_to_image2d(const Pixel2DCluster& pcluster,
			     size_t target_rows, size_t target_cols)
  {
    double origin_x = 0.;
    if(!target_cols) {
      double min_x = pcluster.min_x();
      double max_x = pcluster.max_x();
      origin_x = min_x;
      target_cols = (max_x - min_x + 1);
    }else{
      // find charge centroid
      double mean_x=0;
      double nonzero_npt=0;
      for(auto const& px : pcluster) {
	if(px.Intensity() <= 0) continue;
	mean_x += px.X();
	nonzero_npt += 1.;
      }
      mean_x /= nonzero_npt;
      // compute the origin from centroid position
      origin_x = mean_x - ((double)target_cols)/2.;
    }
    double origin_y = 0.;
    if(!target_rows) {
      double min_y = pcluster.min_y();
      double max_y = pcluster.max_y();
      origin_y = max_y;
      target_rows = (max_y - min_y + 1);
    }else{
      // find charge centroid
      double mean_y=0;
      double nonzero_npt=0;
      for(auto const& px : pcluster) {
	if(px.Intensity() <= 0) continue;
	mean_y += px.Y();
	nonzero_npt += 1.;
      }
      mean_y /= nonzero_npt;
      // compute the origin from centroid position
      origin_y = mean_y + ((double)target_rows)/2.;
    }

    ImageMeta meta(target_cols, target_rows,
		   target_rows, target_cols,
		   origin_x, origin_y,
		   larcv::kINVALID_PLANE);

    Image2D res(meta);
    for(auto const& px : pcluster) {
      int col = (int)((double)(px.X()) - origin_x + 0.5);
      if(col < 0 || col >= (int)(target_cols)) continue;

      int row = (int)(origin_y - (double)(px.Y()) - 0.5);
      if(row < 0 || row >= (int)(target_rows)) continue;

      res.set_pixel(row,col,px.Intensity());
    }
    
    return res;
  }
}

#endif
