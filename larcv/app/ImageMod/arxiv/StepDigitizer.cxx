#ifndef __STEPDIGITIZER_CXX__
#define __STEPDIGITIZER_CXX__

#include "StepDigitizer.h"
#include "DataFormat/EventImage2D.h"

namespace larcv {

  static StepDigitizerProcessFactory __global_StepDigitizerProcessFactory__;

  StepDigitizer::StepDigitizer(const std::string name)
    : ProcessBase(name)
  {}
    
  void StepDigitizer::configure(const PSet& cfg)
  {
    _image_producer  = cfg.get< std::string        > ( "ImageProducer" );
    _adc_boundary_v  = cfg.get< std::vector<float> > ( "ADCBoundary"   );
    _adc_val_v       = cfg.get< std::vector<float> > ( "ADCValue"      );
    if( (_adc_boundary_v.size()+1) != _adc_val_v.size() ) {
      LARCV_CRITICAL() << "Boundary ADC values must be shorter than ADC value array by 1!" << std::endl;
      throw larbys();
    }
  }

  void StepDigitizer::initialize()
  {}

  bool StepDigitizer::process(IOManager& mgr)
  {
    auto event_image = (EventImage2D*)(mgr.get_data(kProductImage2D,_image_producer));

    std::vector<larcv::Image2D> image_v;

    event_image->Move(image_v);

    for(auto& img : image_v) {

      auto const& adc_vec = img.as_vector();

      _buffer.resize(adc_vec.size(),0.);

      for(size_t i=0; i<adc_vec.size(); ++i) {

	float val = _adc_val_v[0];
	for(size_t j=0; j<_buffer.size(); ++j) {
	  if(adc_vec[i] < _adc_boundary_v[j]) break;
	  val = _adc_val_v[j];
	}
	_buffer[i] = val;
      }

      img.copy(0,0,_buffer,_buffer.size());
    }

    event_image->Emplace(std::move(image_v));
    return true;
  }

  void StepDigitizer::finalize()
  {}

}
#endif
