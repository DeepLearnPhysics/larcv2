#ifndef __IMAGEMETAMAKERFACTORY_H__
#define __IMAGEMETAMAKERFACTORY_H__

#include "ImageMetaMakerBase.h"

namespace supera {

  ImageMetaMakerBase* CreateImageMetaMaker(const supera::Config_t& cfg);

}
#endif
