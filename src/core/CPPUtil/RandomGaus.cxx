#ifndef __LARCVUTIL_RANDOM_GAUS_CXX__
#define __LARCVUTIL_RANDOM_GAUS_CXX__

#include "RandomGaus.h"
#include "larcv/core/Base/larbys.h"

namespace larcv {

  RandomGaus::RandomGaus(double mean, double sigma, size_t pool_size)
    : _pool(pool_size)
    , _mean(mean)
    , _sigma(sigma)
  {if(_pool.empty()) throw larbys("Empty pool is meaningless!"); }

  RandomGaus::~RandomGaus() { if(_th.joinable()) _th.join(); }

  void RandomGaus::reset(double mean, double sigma)
  { _mean=mean; _sigma=sigma; }

  
  void RandomGaus::start_filling() {
    if(_th.joinable()) return;
    std::thread t(&larcv::RandomGaus::_fill_,this);
    _th = std::move(t);
  }

  void RandomGaus::get(std::vector<float>& container)
  {
    if(!_th.joinable()) {
      start_filling();
      _th.join();
    }
    else _th.join();
    size_t pool_size=_pool.size();
    container = std::move(_pool);
      _pool.resize(pool_size);
  }

  void RandomGaus::_fill_() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> d(_mean,_sigma);
    
    for(size_t i=0; i<_pool.size(); ++i)
      _pool[i] = (float)(d(gen));
  }
}

#endif
