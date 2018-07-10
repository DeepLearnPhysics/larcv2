#ifndef __QSUMFILTER_CXX__
#define __QSUMFILTER_CXX__

#include "QSumFilter.h"
#include "larcv/core/DataFormat/EventImage2D.h"
namespace larcv {

  static QSumFilterProcessFactory __global_QSumFilterProcessFactory__;

  QSumFilter::QSumFilter(const std::string name)
    : ProcessBase(name)
  {}

  void QSumFilter::configure(const PSet& cfg)
  {
    _image_producer = cfg.get< std::string          >("ImageProducer");
    _min_qsum_v     = cfg.get< std::vector<double>  >("MinQSum");
    _min_pixel_v    = cfg.get< std::vector<int>     >("MinPixel");
    _min_adc_v      = cfg.get< std::vector<double>  >("MinADC");
    if(_min_qsum_v.size() != _min_adc_v.size() || _min_qsum_v.size() != _min_pixel_v.size()) {
      LARCV_CRITICAL() << "Configuration parameter arrays has a length mis-match!" << std::endl;
      throw larbys();
    }
  }

  void QSumFilter::initialize()
  {}

  bool QSumFilter::process(IOManager& mgr)
  {
    auto const& ev_image = mgr.get_data<larcv::EventImage2D>(_image_producer);

    auto const& image_v = ev_image.image2d_array();

    if(image_v.size() != _min_qsum_v.size()) {
      if(image_v.empty()) {
        LARCV_INFO() << "Skipping an empty image event..." << std::endl;
        return false;
      }
      LARCV_CRITICAL() << "# planes do not agree between data and configuration parameters!" << std::endl;
      throw larbys();
    }

    for(size_t i=0; i<_min_qsum_v.size(); ++i) {

      auto const& img = image_v[i].as_vector();
      double qsum = 0;
      int pixel_ctr=0;
      for(auto const& v : img) if(v > _min_adc_v[i]) { pixel_ctr++; qsum += v; }

      bool decision = (qsum > _min_qsum_v[i] && pixel_ctr > _min_pixel_v[i]);
      LARCV_INFO() << "Plane " << i << " QSum = " << qsum << " : NPx = " << pixel_ctr
                   << " ... Charge Thres.: " << _min_qsum_v[i]
                   << " ... Pixel Thres.: " << _min_pixel_v[i]
                   << " ... " << (decision ? "store!" : "skip!") << std::endl;
      if(!decision) return decision;
    }
    return true;
  }

  void QSumFilter::finalize()
  {}

}
#endif
