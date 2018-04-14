#ifndef __LARCV_ALGORITHMS_CROPPING_CXX__
#define __LARCV_ALGORITHMS_CROPPING_CXX__

#include "Cropping.h"
#include <cassert>

namespace larcv {
	namespace algo{
		namespace crop{

			//std::vector<larcv::Image2D>
			void
			Equipartition(const larcv::Image2D& image,
				size_t rows, size_t cols,
				size_t rows_overlap, size_t cols_overlap)
			{
				assert(cols); assert(rows);
				auto const& meta = image.meta();

				size_t img_rows = meta.rows();
				size_t img_cols = meta.cols();

				size_t row_start = 0;
				size_t col_start = 0;
				size_t num_roi_x = 0; size_t num_roi_y = 0;

				std::vector<larcv::ImageMeta> meta_v;

				while( (row_start+rows) < img_rows ) {

					if   (num_roi_y==0) row_start = 0;
					else row_start = rows * num_roi_y - rows_overlap * (2 * num_roi_y - 1);

					num_roi_y++;
					num_roi_x = 0;
					col_start = 0;
					while( (col_start+cols) < img_cols)  {

						if   (num_roi_x==0) col_start = 0;
						else col_start = cols * num_roi_x - cols_overlap * (2 * num_roi_x - 1);

						num_roi_x++;
						std::cout << row_start << " " << col_start << std::endl;
						auto pt_min = meta.position(row_start, col_start);
						auto pt_max = pt_min;
						pt_max.x += (cols * meta.pixel_width());
						pt_max.y += (rows * meta.pixel_height());
						std::cout << "(" << pt_min.x << "," << pt_min.y << ")" 
								  << " (" << pt_max.x << "," << pt_max.y << ")" << std::endl;
						BBox2D    roi_bbox(pt_min, pt_max, meta.id());
						ImageMeta roi_meta(roi_bbox, rows, cols, meta.unit());
						meta_v.push_back(roi_meta);

						std::cout << roi_meta.dump() << std::endl;

					}
					std::cout << std::endl;
				}



			}

		}
	}
}
#endif
