#ifndef ALGORITHMS_CXX
#define ALGORITHMS_CXX

#include "Algorithms.h"
#include "Cropping.h"
#include "larcv/core/DataFormat/Image2D.h"

namespace larcv { namespace algo { namespace crop {
	void Algorithms::test(size_t rows, size_t cols, size_t rows_overlap, size_t cols_overlap) {

		ImageMeta meta(0., 0., 3456, 6048, 1008, 3456, larcv::ProjectionID_t(0), larcv::DistanceUnit_t(0));

		std::cout<<meta.dump()<<std::endl;
		Image2D img(meta);

		Equipartition(img, rows, cols, rows_overlap, cols_overlap);

	}
}}}
#endif
