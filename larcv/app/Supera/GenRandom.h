/**
* \file GenRandom.h
*
* \ingroup LArCVImageMaker
*
* \brief Class def header for a class GenRandom
*
* @author kterao
*/

/** \addtogroup LArCVImageMaker

@{*/
#ifndef SUPERA_GENRANDOM_H
#define SUPERA_GENRANDOM_H

#include <iostream>
#include "CLHEP/Random/RandFlat.h"

class LArSoftSuperaDriver;

namespace supera {
  /**
  \class GenRandom
  User defined class GenRandom ... these comments are used to generate
  doxygen documentation!
  */
  class GenRandom{
    friend class LArSoftSuperaDriver;
  private:

    /// Default constructor
    GenRandom() : _flatRandom(nullptr)
    {}

      /// Default destructor
      ~GenRandom(){}

      static GenRandom* _sptr;
      CLHEP::RandFlat* _flatRandom;

    public:

      static GenRandom& get() {
        if(!_sptr) _sptr = new GenRandom();
        return *_sptr;
      }

      void SetFlatGen(CLHEP::RandFlat* ptr);

      double Flat(double min, double max);

    };
  }

  #endif
  /** @} */ // end of doxygen group
