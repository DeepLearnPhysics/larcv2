#ifndef __SEGIMAGEANA_CXX__
#define __SEGIMAGEANA_CXX__

#include "SegImageAna.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventROI.h"

namespace larcv {

  static SegImageAnaProcessFactory __global_SegImageAnaProcessFactory__;

  SegImageAna::SegImageAna(const std::string name)
    : ProcessBase(name)
  {}

  void SegImageAna::configure(const PSet& cfg)
  {
    _image_producer   = cfg.get<std::string>("ImageProducer");
    _label_producer   = cfg.get<std::string>("LabelProducer");
    _segment_producer = cfg.get<std::string>("SegmentProducer");
    _part_producer    = cfg.get<std::string>("ParticleProducer");

    _image_channel = cfg.get<size_t>("ImageChannel");
    _label_channel = cfg.get<size_t>("LabelChannel");
    _pi_thresh     = cfg.get<double>("PIThreshold");

    auto type_def = cfg.get<std::vector<unsigned short> >("ClassTypeDef");
    auto type_to_class = cfg.get<std::vector<unsigned short> >("ClassTypeList");
    if (type_to_class.empty()) {
      LARCV_CRITICAL() << "ClassTypeList needed to define classes!" << std::endl;
      throw larbys();
    }
    _roitype_to_class.clear();
    _ntypes = type_to_class.size() + 1;
    _roitype_to_class.resize(kROITypeMax, kINVALID_SIZE);
    _roitype_to_class[larcv::kROIUnknown] = 0;
    for (size_t i = 0; i < type_to_class.size(); ++i) {
      auto const& type = type_to_class[i];
      if (type >= kROITypeMax) {
        LARCV_CRITICAL() << "ClassTypeList contains type " << type << " which is not a valid ROIType_t!" << std::endl;
        throw larbys();
      }
      _roitype_to_class[type] = i + 1;
    }

    for (size_t i = 0; i < _roitype_to_class.size(); ++i) {

      if (_roitype_to_class[i] != kINVALID_SIZE) continue;

      _roitype_to_class[i] = _roitype_to_class[type_def[i]];
    }
  
  }

  void SegImageAna::initialize()
  {
    _npx_total_v.clear     ();  _npx_total_v.resize     (_ntypes, 0.);
    _npx_predicted_v.clear ();  _npx_predicted_v.resize (_ntypes, 0.);
    _npx_correct_v.clear   ();  _npx_correct_v.resize   (_ntypes, 0.);
    _prob_correct_v.clear  ();  _prob_correct_v.resize  (_ntypes, 0.);

    _tree = new TTree("segtree","");
    _tree->Branch("pi_thresh",      &_pi_thresh,    "pi_thresh/D"    );
    _tree->Branch("npx_thresh",     &_npx_thresh,   "npx_thresh/i"   );
    _tree->Branch("npx_total",      &_npx_total,    "npx_total/i"    );
    _tree->Branch("npx_correct",    &_npx_correct,  "npx_correct/i"  );
    _tree->Branch("roi_type",       &_roi_type,     "roi_type/i"     );
    _tree->Branch("prob_correct",   &_prob_correct, "prob_correct/D" );
    _tree->Branch("npx_total_v",    &_npx_total_v    );
    _tree->Branch("npx_predicted_v",&_npx_predicted_v);
    _tree->Branch("npx_correct_v",  &_npx_correct_v  );
    _tree->Branch("prob_correct_v", &_prob_correct_v );

  }

  bool SegImageAna::process(IOManager & mgr)
  {
    auto event_image = (EventImage2D*)(mgr.get_data(kProductImage2D,_image_producer));
    auto event_label = (EventImage2D*)(mgr.get_data(kProductImage2D,_label_producer));
    auto event_segment = (EventImage2D*)(mgr.get_data(kProductImage2D,_segment_producer));
    auto event_roi = (EventROI*)(mgr.get_data(kProductROI,_roi_producer));
    
    _roi_type = (int)(event_roi->ROIArray()[1].Type());

    if(!event_image) {
      LARCV_CRITICAL() << "Image by " << _image_producer << " not found..." << std::endl;
      throw larbys();
    }else if(event_image->Image2DArray().size() <= _image_channel) {
      LARCV_CRITICAL() << "Image by " << _image_producer 
      << " has length " << event_image->Image2DArray().size()
      << " ... too short for requesting a channel " << _image_channel << std::endl;
      throw larbys();
    }

    if(!event_label) {
      LARCV_CRITICAL() << "Label Image by " << _label_producer << " not found..." << std::endl;
      throw larbys();
    }else if(event_label->Image2DArray().size() <= _label_channel) {
      LARCV_CRITICAL() << "Label image by " << _label_producer 
      << " has length " << event_label->Image2DArray().size()
      << " ... too short for requesting a channel " << _label_channel << std::endl;
      throw larbys();
    }

    if(!event_segment) {
      LARCV_CRITICAL() << "Segment image by " << _segment_producer << " not found..." << std::endl;
      throw larbys();
    }else if(event_segment->Image2DArray().size() != _ntypes) {
      LARCV_CRITICAL() << "Segment image by " << _segment_producer
      << " has length " << event_segment->Image2DArray().size()
      << " ... not same size as specified # of channels " << _ntypes << std::endl;
      throw larbys();
    }

    // Get input image
    auto const& image = event_image->Image2DArray()[_image_channel].as_vector();
    auto const& label = event_label->Image2DArray()[_label_channel].as_vector();
    auto const& segment_v = event_segment->Image2DArray();

    if(image.size()!=label.size()) {
      LARCV_CRITICAL() << "Image and label dimension is different!" << std::endl;
      throw larbys();
    }
    for(auto const& seg : event_segment->Image2DArray()) {
      if(seg.as_vector().size() != image.size()) {
        LARCV_CRITICAL() << "Found a segment image which dimension is not same as input image!" << std::endl;
        throw larbys();
      }
    }

    // Loop over pixels to perform analysis
    _npx_thresh   = 0;
    _npx_total    = 0;
    _npx_correct  = 0;
    _prob_correct = 0;
    for(auto& v : _npx_total_v     ) v = 0;
    for(auto& v : _npx_predicted_v ) v = 0;
    for(auto& v : _npx_correct_v   ) v = 0;
    for(auto& v : _prob_correct_v  ) v = 0;

    for(size_t i=0; i<image.size(); ++i) {
      ++_npx_total;
      // If below PI threshold, ignore
      if(image[i] < _pi_thresh) continue;
      ++_npx_thresh;
      // Get the correct label
      auto const& truth_type = _roitype_to_class[(size_t)(label[i])];
      // Increment count for the truth label type
      _npx_total_v[truth_type] += 1;
      // Find the prediction
      size_t prediction_type =   0;
      float prediction_max   = -1.;
      float prediction_sum   =  0.;
      float prob     =  0.;
      for(size_t type_index = 0; type_index < _ntypes; ++type_index) {
        float score = std::exp( segment_v[type_index].as_vector()[i] );
        prediction_sum += score;
        if(truth_type == type_index) prob = score;
        if(score > prediction_max) {
          prediction_max = score;
          prediction_type = type_index;
        }
      }
      prob /= prediction_sum;             //softmax per pixel (across classes)
      prediction_max /= prediction_sum;   //max softmax score per pixel (across classes)

      // Increment a counter for a predicted type
      _npx_predicted_v[prediction_type] += 1;
      // Increment a counter for a correct prediction
      if(prediction_type == truth_type) {
        _npx_correct += 1;
        _npx_correct_v[prediction_type] += 1;
      }
      // Increment correct probability overall
      _prob_correct += prob;
      // Increment correct probability of truth label type
      _prob_correct_v[truth_type] += prob;
    }

    //normalize probability
    if ( _npx_thresh > 0 )
      _prob_correct /= (float)(_npx_thresh);
    else
      _prob_correct = 0;

    for(size_t type=0; type<_ntypes; ++type)  {

	if (_npx_total_v[type] > 0 )
	  _prob_correct_v[type] /= (float)(_npx_total_v[type]);
	else
	  _prob_correct_v[type] = 0.0;
      }
    

    _tree->Fill();
    return true;
  }

  void SegImageAna::finalize()
  {
    if ( has_ana_file() ) { _tree->Write(); }
  }

}
#endif
