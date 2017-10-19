#ifndef __SUPERAOPDIGIT_CXX__
#define __SUPERAOPDIGIT_CXX__

#include "SuperaOpDigit.h"
#include "LAr2Image.h"
#include "ImageMetaMakerFactory.h"
#include "PulledPork3DSlicer.h"
#include "DataFormat/EventImage2D.h"

namespace larcv {

  static SuperaOpDigitProcessFactory __global_SuperaOpDigitProcessFactory__;

  SuperaOpDigit::SuperaOpDigit(const std::string name)
    : SuperaBase(name)
  {}
    
  void SuperaOpDigit::configure(const PSet& cfg)
  {
    SuperaBase::configure(cfg);
    supera::ParamsImage2D::configure(cfg);
    supera::ImageMetaMaker::configure(cfg);
    if(supera::PulledPork3DSlicer::Is(supera::ImageMetaMaker::MetaMakerPtr())) {
      LARCV_CRITICAL() << "PulledPork3DSlicer should not be used for Optical image maker!" << std::endl;
      throw larbys();
    }
  }

  void SuperaOpDigit::initialize()
  { SuperaBase::initialize(); }

  bool SuperaOpDigit::process(IOManager& mgr)
  {
    SuperaBase::process(mgr);

    auto const& meta_v = Meta();
    
    if(meta_v.empty()) {
      LARCV_CRITICAL() << "Meta not created!" << std::endl;
      throw larbys();
    }
    if(meta_v.size()!=1) {
      LARCV_CRITICAL() << "Meta > 1, not expected for OpDigit" << std::endl;
      throw larbys();
    }
    auto ev_image = (EventImage2D*)(mgr.get_data(kProductImage2D,OutImageLabel()));
    if(!ev_image) {
      LARCV_CRITICAL() << "Output image could not be created!" << std::endl;
      throw larbys();
    }
    if(!(ev_image->Image2DArray().empty())) {
      LARCV_CRITICAL() << "Output image array not empty!" << std::endl;
      throw larbys();
    }

    std::vector<larcv::Image2D> image_v;
    image_v.emplace_back(supera::OpDigit2Image2D(meta_v.front(), LArData<supera::LArOpDigit_t>(), TimeOffset()));

    for(size_t plane=0; plane<image_v.size(); ++plane) {
      auto& image = image_v[plane];
      image.compress(image.meta().rows() / RowCompressionFactor().at(plane),
		     image.meta().cols() / ColCompressionFactor().at(plane),
		     larcv::Image2D::kSum);
    }

    ev_image->Emplace(std::move(image_v));

    return true;
  }

  void SuperaOpDigit::finalize()
  {}

}
#endif
