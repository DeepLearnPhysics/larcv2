#ifndef __SIMPLEDIGITIZER_CXX__
#define __SIMPLEDIGITIZER_CXX__

#include "SimpleDigitizer.h"
#include "DataFormat/EventImage2D.h"
namespace larcv {

  static SimpleDigitizerProcessFactory __global_SimpleDigitizerProcessFactory__;

  SimpleDigitizer::SimpleDigitizer(const std::string name)
    : ProcessBase(name)
  {}
    
  void SimpleDigitizer::configure(const PSet& cfg)
  {
    _image_producer = cfg.get< std::string         > ( "ImageProducer" );
    _min_adc_v      = cfg.get< std::vector<float>  > ( "MinADC"        );
    _max_adc_v      = cfg.get< std::vector<float>  > ( "MaxADC"        );
    _nrange_v       = cfg.get< std::vector<size_t> > ( "NSteps"        );
    if(_min_adc_v.size() != _max_adc_v.size()) {
      LARCV_CRITICAL() << "Length of configuration parameter array is not same!" << std::endl;
      throw larbys();
    }
  }

  void SimpleDigitizer::initialize()
  {}

  bool SimpleDigitizer::process(IOManager& mgr)
  {
    auto event_image = (EventImage2D*)(mgr.get_data(kProductImage2D,_image_producer));

    std::vector<larcv::Image2D> image_v;
    event_image->Move(image_v);

    if(image_v.size() != _min_adc_v.size()) {
      LARCV_CRITICAL() << "# planes in the image and configured parameter array length do not match!" << std::endl;
      throw larbys();
    }
    
    for(size_t img_index=0; img_index<image_v.size(); ++img_index) {

      auto& img = image_v[img_index];

      auto min_adc = _min_adc_v[img_index];
      auto max_adc = _max_adc_v[img_index];
      auto nsteps  = _nrange_v[img_index];

      float step = (max_adc - min_adc) / ((float)nsteps);
      
      auto const& adc_v = img.as_vector();
      _buffer.resize(adc_v.size(),0.);

      for(size_t i=0; i<adc_v.size(); ++i) {
	if(adc_v[i] < min_adc) _buffer[i] = min_adc;
	else if(adc_v[i] > max_adc) _buffer[i] = max_adc;
	else {
	  _buffer[i] = (size_t)((adc_v[i] - min_adc + step/2.) / step);
	}
      }

      img.copy(0,0,_buffer,_buffer.size());
    }

    event_image->Emplace(std::move(image_v));

    return true;
  }

  void SimpleDigitizer::finalize()
  {}

}
#endif
