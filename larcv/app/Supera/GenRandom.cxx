#ifndef GENRANDOM_CXX
#define GENRANDOM_CXX

#include "GenRandom.h"

namespace supera {

  GenRandom* GenRandom::_sptr = nullptr;

  double GenRandom::Flat(double min, double max)
  {
    if(!_flatRandom) {
      std::cerr << "CLHEP::RandFlat pointer is not yet set!" << std::endl;
      throw std::exception();
    }
    return _flatRandom->fire(min,max);
  }

  void GenRandom::SetFlatGen(CLHEP::RandFlat* ptr)
  { _flatRandom = ptr; }

}

#endif
