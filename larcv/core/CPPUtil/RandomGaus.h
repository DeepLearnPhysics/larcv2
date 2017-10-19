/**
 * \file larcv_logger.h
 *
 * \ingroup LArCV
 * 
 * \brief logger utility class definition header file.
 *
 * @author Kazu - Nevis 2015
 */

/** \addtogroup LArCV

    @{*/

#ifndef __LARCVUTIL_RANDOM_GAUS_H__
#define __LARCVUTIL_RANDOM_GAUS_H__

#ifndef __CINT__
#include <random>
#include <thread>
#endif 
#include <cmath>
#include <vector>
#include <exception>

namespace larcv {

  /**
    \class RandomGaus
    Threaded generator for a pool of gaussian random numbers
  */
  class RandomGaus {
  public:
    /// Default ctor
    RandomGaus(double mean=1., double sigma=0.1, size_t pool_size=10000000);

    /// Move ctor
    RandomGaus(RandomGaus&& rhs) : _pool(std::move(rhs._pool))
				 , _mean(rhs._mean)
				 , _sigma(rhs._sigma)
				 , _th(std::move(rhs._th))
    {}

    /// dtor, make sure to join generator thread
    ~RandomGaus(); 
    /// Reset gaussian parameters
    void reset(double mean, double sigma);
    /// Start thread to fill gaussian random numbers
    void start_filling();
    /// Retrieve a pool of generated gaussian random numbers (if thread is still running this function will wait)
    void get(std::vector<float>& container);

  private:
    void _fill_();    
    std::vector<float> _pool;
    double _mean;
    double _sigma;
    #ifndef __CINT__
    std::thread _th;
    #endif
  }; 
}

#endif
/** @} */ // end of doxygen group
