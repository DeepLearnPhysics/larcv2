#ifndef __COSMICSEGMENT_CXX__
#define __COSMICSEGMENT_CXX__

#include "CosmicSegment.h"
#include "DataFormat/EventImage2D.h"

namespace larcv {

  static CosmicSegmentProcessFactory __global_CosmicSegmentProcessFactory__;

  CosmicSegment::CosmicSegment(const std::string name)
    : ProcessBase(name)
  {}
    
  void CosmicSegment::configure(const PSet& cfg)
  {
    _input_producer  = cfg.get<std::string>("InputImageProducer");
    _output_producer = cfg.get<std::string>("OutputImageProducer");
    _adc_threshold_v = cfg.get<std::vector<float> >("ADCThreshold");
  }

  void CosmicSegment::initialize()
  {
    _input_id  = kINVALID_SIZE;
    _output_id = kINVALID_SIZE;
  }

  bool CosmicSegment::process(IOManager& mgr)
  {
    // Set IDs to be used
    if(_input_id  == kINVALID_SIZE) _input_id  = mgr.producer_id(kProductImage2D,_input_producer);
    if(_output_id == kINVALID_PRODUCER) {
      mgr.get_data(kProductImage2D,_output_producer);
      _output_id = mgr.producer_id(kProductImage2D,_output_producer);
    }

    // Get input
    auto event_input = (EventImage2D*)(mgr.get_data(_input_id));

    // Get output
    auto event_output = (EventImage2D*)(mgr.get_data(_output_id));

    auto image_v = event_input->Image2DArray();

    if(_adc_threshold_v.size() != image_v.size()) {
      LARCV_CRITICAL() << "# ADC threshold != # images!" << std::endl;
      throw larbys();
    }

    for(size_t i=0; i<image_v.size(); ++i) {

      auto& img = image_v[i];

      auto const& thres = _adc_threshold_v[i];
      
      img.binary_threshold(thres,(float)kROIUnknown,(float)kROICosmic);

    }

    event_output->Emplace(std::move(image_v));
    return true;
  }

  void CosmicSegment::finalize()
  {}

}
#endif
