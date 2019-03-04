#ifndef __LARBYSIMAGEEXTRACT_CXX__
#define __LARBYSIMAGEEXTRACT_CXX__

#include "LArbysImageExtract.h"

namespace larcv {

  static LArbysImageExtractProcessFactory __global_LArbysImageExtractProcessFactory__;

  LArbysImageExtract::LArbysImageExtract(const std::string name)
    : ProcessBase(name),
      _LArbysImageMaker()
  {}
    
  void LArbysImageExtract::configure(const PSet& cfg)
  {
    _adc_producer    = cfg.get<std::string>("ADCImageProducer");
    _track_producer  = cfg.get<std::string>("TrackImageProducer","");
    _shower_producer = cfg.get<std::string>("ShowerImageProducer","");

    _thrumu_producer = cfg.get<std::string>("ThruMuProducer","");
    _stopmu_producer = cfg.get<std::string>("StopMuProducer","");

    _reco_roi_producer = cfg.get<std::string>("RecoROIProducer","");
    _true_roi_producer = cfg.get<std::string>("TrueROIProducer","");
    
    _LArbysImageMaker.Configure(cfg.get<larcv::PSet>("LArbysImageMaker"));
    _PreProcessor.Configure(cfg.get<larcv::PSet>("PreProcessor"));
  }

  void LArbysImageExtract::initialize()
  {}

  bool LArbysImageExtract::process(IOManager& mgr)
  {

    const auto ev_adc = (EventImage2D*)mgr.get_data(kProductImage2D,_adc_producer);
    _ev_adc = *ev_adc;
    
    EventImage2D* ev_trk = nullptr;
    if (!_track_producer.empty())  {
      ev_trk = (EventImage2D*)mgr.get_data(kProductImage2D,_track_producer);
      _ev_trk = *ev_trk;
    }

    EventImage2D* ev_shr = nullptr;
    if (!_shower_producer.empty()) {
      ev_shr = (EventImage2D*)mgr.get_data(kProductImage2D,_shower_producer);
      _ev_shr = *ev_shr;
    }

    EventPixel2D* ev_thrumu_pix = nullptr;
    if (!_thrumu_producer.empty()) {
      ev_thrumu_pix = (EventPixel2D*)mgr.get_data(kProductPixel2D,_thrumu_producer);
      _ev_thrumu_pix = *ev_thrumu_pix;

      _ev_thrumu.clear();
      for(size_t plane=0; plane < 3; ++plane) {
	const auto& adc_img2d = ev_adc->Image2DArray().at(plane);
	auto thrumuimg2d = _LArbysImageMaker.ConstructCosmicImage(_ev_thrumu_pix,adc_img2d,plane,100.0);
	_ev_thrumu.Emplace(std::move(thrumuimg2d));
      }
      
    }

    EventPixel2D* ev_stopmu_pix = nullptr;
    if (!_stopmu_producer.empty()) {
      ev_stopmu_pix = (EventPixel2D*)mgr.get_data(kProductPixel2D,_stopmu_producer);
      _ev_stopmu_pix = *ev_stopmu_pix;

      _ev_stopmu.clear();
      for(size_t plane=0; plane < 3; ++plane) {
	const auto& adc_img2d = ev_adc->Image2DArray().at(plane);
	auto stopmuimg2d = _LArbysImageMaker.ConstructCosmicImage(_ev_stopmu_pix,adc_img2d,plane,100.0);
	_ev_stopmu.Emplace(std::move(stopmuimg2d));
      }
      
    }

    EventROI* ev_reco_roi = nullptr;
    if(!_reco_roi_producer.empty()) {
      ev_reco_roi = (EventROI*)mgr.get_data(kProductROI,_reco_roi_producer);
      if(!ev_reco_roi) throw larbys("Unable to fetch ROI producer");
      _reco_roi_v = ev_reco_roi->ROIArray();
    }


    EventROI* ev_true_roi = nullptr;
    if(!_reco_roi_producer.empty()) {
      ev_true_roi = (EventROI*)mgr.get_data(kProductROI,_true_roi_producer);
      if(!ev_true_roi) throw larbys("Unable to fetch ROI producer");
      _true_roi_v = ev_true_roi->ROIArray();
    }

    
    FillcvMat();
    return true;
  }

  void LArbysImageExtract::FillcvMat(larcv::ROI* roi) {
    
    auto ev_adc    = _ev_adc.Image2DArray();
    auto ev_trk    = _ev_trk.Image2DArray();
    auto ev_shr    = _ev_shr.Image2DArray();
    auto ev_thrumu = _ev_thrumu.Image2DArray();
    auto ev_stopmu = _ev_stopmu.Image2DArray();
      
    if(roi) {
      for(size_t plane=0;plane<3;++plane) {
	auto& meta = roi->BB(plane);

	// crop the adc image
	auto& adc_img = ev_adc.at(plane);
	adc_img = adc_img.crop(meta);
	
	// crop the trk image
	if(ev_trk.size()) {
	  auto& trk_img = ev_trk.at(plane);
	  trk_img = trk_img.crop(meta);
	}
	
	// crop the shr image
	if(ev_shr.size()) {
	  auto& shr_img = ev_shr.at(plane);
	  shr_img = shr_img.crop(meta);
	}

	// crop the thrumu image
	if(ev_thrumu.size()) {
	  auto& thrumu_img = ev_thrumu.at(plane);
	  thrumu_img = thrumu_img.crop(meta);
	}

	// crop the stopmu image
	if(ev_stopmu.size()) {
	  auto& stopmu_img = ev_stopmu.at(plane);
	  stopmu_img = stopmu_img.crop(meta);
	}
      }
      
    }
    
    _adc_mat_v.clear();
    _adc_meta_v.clear();
    _adc_mat_v.reserve(3);
    _adc_meta_v.reserve(3);
    
    for(auto img_data : _LArbysImageMaker.ExtractImage(ev_adc)) {
      _adc_mat_v.emplace_back(std::move(std::get<0>(img_data)));
      _adc_meta_v.emplace_back(std::move(std::get<1>(img_data)));
    }
    
    _track_mat_v  = _LArbysImageMaker.ExtractMat(ev_trk);
    _shower_mat_v = _LArbysImageMaker.ExtractMat(ev_shr);
    _thrumu_mat_v = _LArbysImageMaker.ExtractMat(ev_thrumu);
    _stopmu_mat_v = _LArbysImageMaker.ExtractMat(ev_stopmu);
    
  }
  
  void LArbysImageExtract::finalize()
  {}

}
#endif
