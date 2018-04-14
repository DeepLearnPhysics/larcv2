#ifndef __LARCV_ALGORITHMS_CROPPING_H__
#define __LARCV_ALGORITHMS_CROPPING_H__

#include "larcv/core/DataFormat/Image2D.h"
#include <vector>

namespace larcv {
	namespace algo{
		namespace crop{

        //std::vector<larcv::Image2D>
		void
		Equipartition(const larcv::Image2D& image,
			size_t rows, size_t cols,
			size_t rows_overlap=0, size_t cols_overlap=0);

		}
	}
}
#endif
