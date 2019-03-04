#ifndef __IMAGEMETAMAKERFACTORY_CXX__
#define __IMAGEMETAMAKERFACTORY_CXX__

#include "Base/larcv_logger.h"
#include "ImageMetaMakerFactory.h"
#include "ImageMetaFromConfig.h"
#include "PulledPork3DSlicer.h"

namespace supera {
  
  ImageMetaMakerBase* CreateImageMetaMaker(const supera::Config_t& cfg)
  {
    ImageMetaMakerBase* maker = nullptr;
    try{
      LARCV_SINFO() << "Attempting to construct supera::ImageMetaFromConfig... " << std::endl;
      auto const& meta_cfg = cfg.get<supera::Config_t>("MetaConfig");
      maker = new ImageMetaFromConfig();
      maker->configure(meta_cfg);
      LARCV_SINFO() << "Successfully made supera::ImageMetaFromConfig! " << std::endl;
    }catch(...){
      if(maker) delete maker;
    }
    if(!maker) {
      try{
	LARCV_SINFO() << "Attempting to construct supera::PuledPork3DSlicer..." << std::endl;
	auto const& meta_cfg = cfg.get<supera::Config_t>("PulledPork");
	maker = new PulledPork3DSlicer();
	maker->configure(meta_cfg);
	LARCV_SINFO() << "Successfully made supera::PulledPork3DSlicer! " << std::endl;
      }catch(...){
	if(maker) delete maker;
      }
    }
    LARCV_SINFO() << "Returning supera::ImageMetaMakerBase: " << maker << std::endl;
    return maker;
  }

}
#endif
