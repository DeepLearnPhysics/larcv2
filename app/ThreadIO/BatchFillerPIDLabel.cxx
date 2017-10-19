#ifndef __BatchFillerPIDLabel_CXX__
#define __BatchFillerPIDLabel_CXX__

#include "BatchFillerPIDLabel.h"
#include "DataFormat/UtilFunc.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventROI.h"
#include <random>

namespace larcv {

  static BatchFillerPIDLabelProcessFactory __global_BatchFillerPIDLabelProcessFactory__;

  BatchFillerPIDLabel::BatchFillerPIDLabel(const std::string name)
    : BatchFillerTemplate<float>(name)
  {}

  void BatchFillerPIDLabel::configure(const PSet& cfg)
  {
    _roi_producer = cfg.get<std::string>("ROIProducer");

    auto type_to_class = cfg.get<std::vector<unsigned short> >("ClassTypeList");
    if(type_to_class.empty()) {
      LARCV_CRITICAL() << "ClassTypeList needed to define classes!" << std::endl;
      throw larbys();
    }
    _num_class = type_to_class.size();
    _roitype_to_class.clear();
    _roitype_to_class.resize(kROITypeMax,kINVALID_SIZE);
    for(size_t i=0; i<type_to_class.size(); ++i) {
      auto const& type = type_to_class[i];
      if(type >= kROITypeMax) {
        LARCV_CRITICAL() << "ClassTypeList contains type " << type << " which is not a valid ROIType_t!" << std::endl;
        throw larbys();
      }
      _roitype_to_class[type] = i;
    }

  }

  void BatchFillerPIDLabel::initialize()
  {}

  void BatchFillerPIDLabel::_batch_begin_() 
  {}

  void BatchFillerPIDLabel::_batch_end_()
  {
    if(logger().level() <= msg::kINFO) {
      LARCV_INFO() << "Total data size: " << batch_data().data_size() << std::endl;

      std::vector<size_t> ctr_v;
      for(auto const& v : batch_data().data()) {
        if(v>=ctr_v.size()) ctr_v.resize(v+1,0);
        ctr_v[v] += 1;
      }
      std::stringstream ss;
      ss << "Used: ";
      for(size_t i=0;i<ctr_v.size();++i)
        ss << ctr_v[i] << " of class " << i << " ... ";
      LARCV_INFO() << ss.str() << std::endl;
    }
  }

  void BatchFillerPIDLabel::finalize()
  {}

  bool BatchFillerPIDLabel::process(IOManager& mgr)
  {
    auto event_roi = (EventROI*)(mgr.get_data(kProductROI,_roi_producer));

    if(batch_data().dim().empty()) {
      std::vector<int> dim(2);
      dim[0] = batch_size();
      dim[1] = _num_class;
      set_dim(dim);
    }

    // labels
    auto const& roi_v = event_roi->ROIArray();
    ROIType_t roi_type = kROICosmic;
    for(auto const& roi : roi_v) {
      if(roi.MCSTIndex() != kINVALID_USHORT) continue;
      roi_type = roi.Type();
      if(roi_type == kROIUnknown) roi_type = PDG2ROIType(roi.PdgCode());
      LARCV_INFO() << roi.dump() << std::endl;
      break;
    }

    // Convert type to class
    size_t caffe_class = _roitype_to_class[roi_type];

    if(caffe_class == kINVALID_SIZE) {
      LARCV_CRITICAL() << "ROIType_t " << roi_type << " is not among those defined for final set of class!" << std::endl;
      for(size_t roi_index=0; roi_index<roi_v.size(); ++roi_index)
	LARCV_CRITICAL() << "Dumping ROI " << roi_index << std::endl << roi_v[roi_index].dump() << std::endl;
      throw larbys();
    }

    _entry_data.resize(_num_class,0);
    for(auto& v : _entry_data) v = 0;
    _entry_data[_roitype_to_class[roi_type]] = 1.;
    set_entry_data(_entry_data);

    return true;
  }
   
}
#endif
