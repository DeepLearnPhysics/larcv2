#ifndef LARCV_PARTICLEMASS_CXX
#define LARCV_PARTICLEMASS_CXX

#include <sstream>
#include "ParticleMass.h"
#include "larcv/core/Base/larbys.h"

namespace larcv {

  double ParticleMass(const int pdgcode)
  {
    switch(pdgcode) {

      // electron
    case  11:
    case -11:
      return 0.510998910;
      // muon
    case  13:
    case -13:
      return 105.65838934;
      // gamma
    case 22:
      return 0.0;
      // neutrinos
    case 12:
    case 14:
      return 0.0;
      // proton
    case  2212:
    case -2212:
      return 938.272046;
      // neutron
    case  2112:
    case -2112:
      return 939.565378;
      // pi0
    case  111:
      return 134.9766;
      // pi +/-
    case  211:
    case -211:
      return 139.57018;
      // K0/K0L/K0S
    case  311:
    case  130:
    case  310:
      return 497.648;
      // K +/-
    case  321:
    case -321:
      return 493.667;
      // Delta
    case  2114:
    case  2214:
    case  2224:
    case  1114:
      return 1232.0;
      // Lambda
    case  3122:
    case -3122:
      return 1115.683;
      // Sigma+
    case  3222:
    case -3222:
      return 1189.37;
      // Sigma0
    case  3212:
    case -3212:
      return 1192.642;
      // Sigma-
    case  3112:
    case -3112:
      return 1197.449;
      // Xi0
    case  3322:
      return 1314.86;
      // Xi-/Xi+
    case  3312:
    case -3312:
      return 1321.71;
      // rho-0
    case  113:
      return 775.49;
      // rho-+
    case  123:
      return 775.4;
      // phi
    case  333:
      return 1019.445;
      // eta
    case  221:
      return 547.862;
      // eta'
    case  331:
      return 957.78;
      // deuteron
    case  1000010020:
      return 1875.6;
      // tritium
    case  1000010030:
      return 2829.87;
      // Helium 3
    case  1000020030:
      return 2829.85;
      // Helium 4
    case  1000020040:
      return 3755.53;
      // Berylium 8
    case  1000040080:
      return 7511.15;
    default:
      std::stringstream msg;
      msg << "PDG Code " << pdgcode << " is not handled yet! Please implement :) ";
      throw larbys(msg.str());
    }
    return -1;
  }

}

#endif
