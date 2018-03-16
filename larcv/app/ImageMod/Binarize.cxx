#ifndef __BINARIZE_CXX__
#define __BINARIZE_CXX__

#include "Binarize.h"
#include "larcv/core/DataFormat/EventImage2D.h"

namespace larcv {

  static BinarizeProcessFactory __global_BinarizeProcessFactory__;

  Binarize::Binarize(const std::string name)
    : ProcessBase(name)
  {

  }

  void Binarize::configure(const PSet& cfg)
  {
    fChannelThresholds   = cfg.get<std::vector<float> >( "ChannelThresholds" );
    fInputImageProducer  = cfg.get<std::string>( "InputImageProducer" );
    fOutputImageProducer = cfg.get<std::string>( "OutputImageProducer" );
    fHighADCs = cfg.get<std::vector<float> > ("HighADCs");
    fLowADCs  = cfg.get<std::vector<float> > ("LowADCs");

    if(fChannelThresholds.empty()) {
      LARCV_CRITICAL() << "Empty vector given for ChannelThresholds!" << std::endl;
      throw larbys();
    }
    if(fHighADCs.size() != fChannelThresholds.size() || fLowADCs.size() != fChannelThresholds.size()) {
      LARCV_CRITICAL() << "Input parameter length do not match!" << std::endl;
      throw larbys();
    }
  }

  void Binarize::initialize()
  {}

  bool Binarize::process(IOManager& mgr)
  {
    auto& input_imgs = mgr.get_data<larcv::EventImage2D>(fInputImageProducer);

    std::vector<larcv::Image2D> image_v;
    input_imgs.move(image_v);

    if(image_v.empty()) {
      LARCV_INFO() << "Skipping empty image event" << std::endl;
      return false;
    }
    if(image_v.size() != fChannelThresholds.size()) {
      LARCV_CRITICAL() << "Channel # do not match between input image and data!" << std::endl;
      input_imgs.emplace(std::move(image_v));
      throw larbys();
    }

    for(size_t i=0; i<image_v.size(); ++i) {

      auto const& thres = fChannelThresholds[i];
      auto const& low   = fLowADCs[i];
      auto const& high  = fHighADCs[i];

      auto& img = image_v[i];
      img.binarize(thres,low,high);

    }

    auto& output_imgs = mgr.get_data<larcv::EventImage2D>(fOutputImageProducer);
    output_imgs.emplace(std::move(image_v));

    return true;
  }

  void Binarize::finalize()
  {}

}
#endif
