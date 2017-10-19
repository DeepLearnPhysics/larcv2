#ifndef __SUPERAMETAMAKER_CXX__
#define __SUPERAMETAMAKER_CXX__

#include "SuperaMetaMaker.h"
#include "LAr2Image.h"
#include "ImageMetaMakerFactory.h"
#include "ImageMetaMaker.h"
#include "PulledPork3DSlicer.h"
#include "DataFormat/EventImage2D.h"

namespace larcv {

  static SuperaMetaMakerProcessFactory __global_SuperaMetaMakerProcessFactory__;

  SuperaMetaMaker::SuperaMetaMaker(const std::string name)
    : SuperaBase(name)
    , _meta_maker(nullptr)
  {}

  void SuperaMetaMaker::configure(const PSet& cfg)
  {
    SuperaBase::configure(cfg);
    if(_meta_maker) delete _meta_maker;
    _meta_maker = supera::CreateImageMetaMaker(cfg);
    supera::ImageMetaMaker::SetSharedMetaMaker(_meta_maker);
  }

  void SuperaMetaMaker::initialize()
  { SuperaBase::initialize(); }

  bool SuperaMetaMaker::process(IOManager& mgr)
  {
    SuperaBase::process(mgr);

    if(supera::PulledPork3DSlicer::Is(_meta_maker)) {
      ((supera::PulledPork3DSlicer*)(_meta_maker))->ClearEventData();
      ((supera::PulledPork3DSlicer*)(_meta_maker))->AddConstraint(LArData<supera::LArMCTruth_t>());
      ((supera::PulledPork3DSlicer*)(_meta_maker))->GenerateMeta(LArData<supera::LArSimCh_t>(),TimeOffset());
    }
    
    return true;
  }

  void SuperaMetaMaker::finalize()
  {}


}
#endif
