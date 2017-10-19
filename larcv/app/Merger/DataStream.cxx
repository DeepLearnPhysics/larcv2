#ifndef __DATASTREAM_CXX__
#define __DATASTREAM_CXX__

#include "DataStream.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventChStatus.h"
#include "DataFormat/EventROI.h"

namespace larcv {

  static DataStreamProcessFactory __global_DataStreamProcessFactory__;

  DataStream::DataStream(const std::string name)
    : ImageHolder(name)
  {}
    
  void DataStream::configure(const PSet& cfg)
  {
    _tpc_image_producer = cfg.get<std::string>("TPCImageProducer");
    _pmt_image_producer = cfg.get<std::string>("PMTImageProducer");
    _ch_status_producer = cfg.get<std::string>("ChStatusProducer","");
    _roi_producer = cfg.get<std::string>("ROIProducer");
    _segment_producer = cfg.get<std::string>("SegmentImageProducer","");

    LARCV_INFO() << "Configured" << std::endl;
  }

  void DataStream::initialize()
  {
    LARCV_INFO() << "Initialized" << std::endl;
  }

  bool DataStream::process(IOManager& mgr)
  {
    LARCV_INFO() << "Clearing attributes..." << std::endl;
    _ch_status_m.clear();
    _tpc_image_v.clear();
    _tpc_segment_v.clear();
    _roi_v.clear();
    _pmt_image = Image2D();
    
    // Retrieve ChStatus
    if(!_ch_status_producer.empty()) {

      LARCV_INFO() << "Reading-in ChStatus " << _ch_status_producer << std::endl;

      auto event_chstatus = (EventChStatus*)(mgr.get_data(kProductChStatus,_ch_status_producer));

      if(!event_chstatus || event_chstatus->ChStatusMap().empty()) return false;

      LARCV_INFO() << "Copying ChStatus " << _ch_status_producer << std::endl;
      _ch_status_m = event_chstatus->ChStatusMap();
    }
    

    // Retrieve TPC Image
    LARCV_INFO() << "Reading-in TPC Image2D " << _tpc_image_producer << std::endl;
    auto event_tpc_image = (EventImage2D*)(mgr.get_data(kProductImage2D,_tpc_image_producer));

    if(!event_tpc_image || event_tpc_image->Image2DArray().empty()) return false;

    LARCV_INFO() << "Copying TPC Image2D " << _tpc_image_producer << std::endl;
    event_tpc_image->Move(_tpc_image_v);
    
    // Retrieve PMT Image
    LARCV_INFO() << "Reading-in PMT Image2D " << _pmt_image_producer << std::endl;
    auto event_pmt_image = (EventImage2D*)(mgr.get_data(kProductImage2D,_pmt_image_producer));

    if(!event_pmt_image || event_pmt_image->Image2DArray().empty()) return false;

    LARCV_INFO() << "Copying PMT Image2D " << _pmt_image_producer << std::endl;
    std::vector<larcv::Image2D> tmp_v;
    event_pmt_image->Move(tmp_v);
    if(tmp_v.size())
      _pmt_image = std::move(tmp_v[0]);
    
    //Retrieve the ROI
    if(!_roi_producer.empty()) {
      LARCV_INFO() << "Reading-in ROI " << _roi_producer << std::endl;
      auto event_roi = (EventROI*)(mgr.get_data(kProductROI,_roi_producer));
      if(event_roi and !event_roi->ROIArray().empty())
	_roi_v = event_roi->ROIArray();
    }

    //Retrieve the segmenation image if exists
    if(!_segment_producer.empty()) {
      //segment_proc->move_segment(_tpc_segment_v);
      auto event_segment = (EventImage2D*)(mgr.get_data(kProductImage2D,_segment_producer));
      _tpc_segment_v = event_segment->Image2DArray();
    }
    
    // Retrieve event id
    retrieve_id(event_tpc_image);

    return true;
  }

  void DataStream::finalize()
  {}

}
#endif
