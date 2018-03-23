#ifndef __ADCSCALE_CXX__
#define __ADCSCALE_CXX__

#include "ADCScale.h"
#include "larcv/core/DataFormat/EventImage2D.h"

namespace larcv {

  static ADCScaleProcessFactory __global_ADCScaleProcessFactory__;

  ADCScale::ADCScale(const std::string name)
    : ProcessBase(name)
  {}

  void ADCScale::configure(const PSet& cfg)
  {
    _image_producer = cfg.get<std::string>         ( "ImageProducer" );
    _gaus_mean_v    = cfg.get<std::vector<double> >( "ADCScaleMean"  );
    _gaus_sigma_v   = cfg.get<std::vector<double> >( "ADCScaleSigma" );
    _per_pixel      = cfg.get<bool>( "PixelWise", true );
    if (_gaus_mean_v.size() != _gaus_sigma_v.size()) {
      LARCV_CRITICAL() << "ADCScale Mean & Sigma must be of same length!" << std::endl;
      throw larbys();
    }

    /*
    // Threaded gaussian commented out (too big pool size)
    _gaus_pool_size_v.clear();
    _gaus_pool_size_v.resize(3,9600*3456);
    _gaus_pool_size_v  = cfg.get<std::vector<size_t> >( "RandomPoolSize", _gaus_pool_size_v);

    if(_gaus_mean_v.size() != _gaus_pool_size_v.size()) {
      LARCV_CRITICAL() << "ADCScale Mean/Sigma and pool size must be of same length!" << std::endl;
      throw larbys();
    }

    _randg_v.clear();
    for(size_t i=0; i<_gaus_mean_v.size(); ++i)

      _randg_v.emplace_back(_gaus_mean_v[i], _gaus_sigma_v[i], _gaus_pool_size_v[i]);
    */
  }

  void ADCScale::initialize()
  {
    _image_id = kINVALID_SIZE;
  }

  bool ADCScale::process(IOManager& mgr)
  {
    if (_image_id == kINVALID_SIZE) {
      ProducerName_t id("image2d", _image_producer);
      _image_id = mgr.producer_id(id);
    }

    // Smear ADCs if random gaussian is provided
    if (!_gaus_mean_v.empty()) {

      auto event_image = (EventImage2D*)(mgr.get_data(_image_id));
      std::vector<larcv::Image2D> tpc_image_v;
      event_image->move(tpc_image_v);

      for (size_t i = 0; i < tpc_image_v.size(); ++i) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> d(_gaus_mean_v[i], _gaus_sigma_v[i]);

        //auto& randg = _randg_v[i];
        auto& tpc_image = tpc_image_v[i];
        // Throw warning: @ this code it "should be" index = plane id
        if (tpc_image.meta().id() != i)
          LARCV_WARNING() << "Image index != plane ID is detected... " << std::endl;

        if (_per_pixel) {
          auto const& img_vec = tpc_image.as_vector();

          for (size_t i = 0; i < img_vec.size(); ++i) {

            if (img_vec[i] < 1.) continue;
            float factor = d(gen);
            size_t col = i / tpc_image.meta().rows();
            size_t row = i - col * tpc_image.meta().rows();
            tpc_image.set_pixel(row, col, img_vec[i] * factor);
          }
        } else {
          float factor = d(gen);
          LARCV_INFO() << "Applying scaling factor " << factor << " to image " << i << std::endl;
          /*
          float min=1e9;
          float max=0;
          for(auto const& v : tpc_image.as_vector()) { if(v<min) min = v; if (v>max) max = v; }
          LARCV_INFO() << "BEFORE: min = " << min << " ... max = " << max << std::endl;
          */
          tpc_image *= factor;
          /*
          min = 1e9;
          max = 0;
          for(auto const& v : tpc_image.as_vector()) { if(v<min) min = v; if (v>max) max = v; }
          LARCV_INFO() << "AFTER: min = " << min << " ... max = " << max << std::endl;
          */
        }
      }
      event_image->emplace(std::move(tpc_image_v));
    }
    /*
    // Thread method commented out... too much memory for pool!
    if(!_randg_v.empty()) {

      auto event_image = (EventImage2D*)(mgr.get_data(id));
      std::vector<larcv::Image2D> tpc_image_v;
      event_image->Move(tpc_image_v);

      // Make sure enough generators exist (# images)
      if(tpc_image_v.size() < _randg_v.size()) {
    LARCV_CRITICAL() << "# images > # of gaussian generator... check config!" << std::endl;
    throw larbys();
      }
      // Loop over images
      for(size_t i=0; i<tpc_image_v.size(); ++i) {

    //auto& randg = _randg_v[i];
    auto& tpc_image = tpc_image_v[i];
    // Throw warning: @ this code it "should be" index = plane id
    if(tpc_image.meta().plane() != i)
    LARCV_WARNING() << "Image index != plane ID is detected... " << std::endl;
    // Prepare random image and fill from gaussian generator
    std::vector<float> rand_img;
    _randg_v[i].get(rand_img);
    // If size is too small regenerate
    if(rand_img.size() < tpc_image.size()) {
    LARCV_CRITICAL() << "Detected image size > random number pool size!" << std::endl;
    throw larbys();
    }
    // Perform elt-wise multiplication. Allow random image to be larger in size
    LARCV_INFO() << "ElementWise mult on TPC Image2D " << std::endl;
    tpc_image.eltwise(rand_img,true);
    // Start thread for gaussian random number generation
    sleep(5);

    _randg_v[i].start_filling();
      }
      event_image->Emplace(std::move(tpc_image_v));
    }
    */
    return true;
  }

  void ADCScale::finalize()
  {}

}
#endif
