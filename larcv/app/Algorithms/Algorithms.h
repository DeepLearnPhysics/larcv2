/**
 * \file Algorithms.h
 *
 * \ingroup Algorithms
 * 
 * \brief Class def header for a class Algorithms
 *
 * @author drinkingkazu
 */

/** \addtogroup Algorithms

    @{*/
#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <iostream>

/**
   \class Algorithms
   User defined class Algorithms ... these comments are used to generate
   doxygen documentation!
 */
namespace larcv { namespace algo { namespace crop {
	class Algorithms{

	public:

  /// Default constructor
		Algorithms(){}

  /// Default destructor
		~Algorithms(){}

		void test(size_t rows, size_t cols, size_t rows_overlap, size_t cols_overlap);

	};
}}}

#endif
/** @} */ // end of doxygen group 

