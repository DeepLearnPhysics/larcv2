#ifndef __MEANSUBTRACTION_CXX__
#define __MEANSUBTRACTION_CXX__

#include "MeanSubtraction.h"
#include "DataFormat/EventImage2D.h"

namespace larcv {

  static MeanSubtractionProcessFactory __global_MeanSubtractionProcessFactory__;

  MeanSubtraction::MeanSubtraction(const std::string name)
    : ProcessBase(name)
  {}
    
  void MeanSubtraction::configure(const PSet& cfg)
  {
    auto mean_image_filename = cfg.get<std::string>("MeanImageFilename","");
    auto mean_image_producer = cfg.get<std::string>("MeanImageProducer","");
    _mean_value_v.clear();
    _mean_value_v = cfg.get<std::vector<float> >("MeanValueArray",_mean_value_v);
    _target_image_producer = cfg.get<std::string>("TargetImageProducer");

    if(_mean_value_v.empty() && (mean_image_filename.empty() || mean_image_producer.empty())) {
      LARCV_CRITICAL() << "Must provide either MeanValueArray OR (MeanImageFilename AND MeanImageProducer)!" << std::endl;
      throw larbys();
    }

    if(_mean_value_v.size() && (!mean_image_filename.empty() && !mean_image_producer.empty())) {
      LARCV_CRITICAL() << "Cannot provide BOTH MeanValueArray AND (MeanImageFilename AND MeanImageProducer)!" << std::endl;
      throw larbys();
    }

    if(!mean_image_filename.empty()) {
      LARCV_NORMAL() << "Retrieving a mean image from " << mean_image_filename
		     << " ... producer " << mean_image_producer << std::endl;
      IOManager mean_io(IOManager::kREAD);
      mean_io.add_in_file(mean_image_filename);
      mean_io.initialize();
      mean_io.read_entry(0);
      auto const event_image = (EventImage2D*)(mean_io.get_data(kProductImage2D,mean_image_producer));
      if(!event_image || event_image->Image2DArray().empty()) {
	LARCV_CRITICAL() << "Failed to retrieve mean image array!" << std::endl;
	throw larbys();
      }
      _mean_image_v = event_image->Image2DArray();
      mean_io.finalize();
    }

  }

  void MeanSubtraction::initialize()
  {}

  bool MeanSubtraction::process(IOManager& mgr)
  {

    auto target_event_image = (EventImage2D*)(mgr.get_data(kProductImage2D,_target_image_producer));

    std::vector<Image2D> target_image_v;
    target_event_image->Move(target_image_v);

    if(target_image_v.size() != _mean_image_v.size() && target_image_v.size() != _mean_value_v.size()) {
      LARCV_CRITICAL() << "# mean images and # target images do not match!" << std::endl;
      throw larbys();
    }

    if(_mean_value_v.size())
      
      for(size_t i=0; i<_mean_value_v.size(); ++i) target_image_v[i] -= _mean_value_v[i];

    else {

      for(size_t i=0; i<_mean_image_v.size(); ++i) {

	auto& target_image = target_image_v[i];
	auto& mean_image   = _mean_image_v[i];

	if(target_image.meta().rows() != mean_image.meta().rows()) {
	  LARCV_CRITICAL() << "Mean image # rows (" << mean_image.meta().rows()
			   << ") != Target image # rows (" << target_image.meta().rows()
			   << ")" << std::endl;
	  throw larbys();
	}

	if(target_image.meta().cols() != mean_image.meta().cols()) {
	  LARCV_CRITICAL() << "Mean image # cols (" << mean_image.meta().cols()
			   << ") != Target image # cols (" << target_image.meta().cols()
			   << ")" << std::endl;
	  throw larbys();
	}

	target_image -= mean_image.as_vector();

      }
    }

    target_event_image->Emplace(std::move(target_image_v));

    return true;
  }

  void MeanSubtraction::finalize()
  {}

}
#endif
