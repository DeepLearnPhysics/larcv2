#ifndef __SSNETPIXELANA_CXX__
#define __SSNETPIXELANA_CXX__

#include "SSNetPixelAna.h"
#include "LArbysImageMaker.h"
#include "DataFormat/EventROI.h"
#include "LArbysUtils.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/LArProperties.h"
#include "CVUtil/CVUtil.h"
#include <array>
#include <cassert>

namespace larcv {
  
  static SSNetPixelAnaProcessFactory __global_SSNetPixelAnaProcessFactory__;
  
  SSNetPixelAna::SSNetPixelAna(const std::string name) :
    ProcessBase(name),
    _tree(nullptr)
  {}
    
  void SSNetPixelAna::configure(const PSet& cfg)
  {
    _ev_img2d_prod   = cfg.get<std::string>("EventImage2DProducer");
    _ev_trk2d_prod   = cfg.get<std::string>("TrackImage2DProducer");
    _ev_shr2d_prod   = cfg.get<std::string>("ShowerImage2DProducer"); 

    _roi_prod        = cfg.get<std::string>("ROIProducer");
    _true_roi_prod   = cfg.get<std::string>("TrueROIProducer");
    _crop_radius     = cfg.get<size_t>("CropRadius");
  }
  
  void SSNetPixelAna::initialize()
  {
    
    _tree = new TTree("EventCosmicPixelTree","");
    _tree->Branch("run",&_run,"run/I");
    _tree->Branch("subrun",&_subrun,"subrun/I");
    _tree->Branch("event",&_event,"event/I");
    _tree->Branch("entry",&_entry,"entry/I");
    
    _tree->Branch("trkpixel0",&_trkpixel0,"_trkpixel0/F");
    _tree->Branch("trkpixel1",&_trkpixel1,"_trkpixel1/F");
    _tree->Branch("trkpixel2",&_trkpixel2,"_trkpixel2/F");

    _tree->Branch("shrpixel0",&_shrpixel0,"_shrpixel0/F");
    _tree->Branch("shrpixel1",&_shrpixel1,"_shrpixel1/F");
    _tree->Branch("shrpixel2",&_shrpixel2,"_shrpixel2/F");

    _tree->Branch("npixel0",&_npixel0,"_npixel0/F");
    _tree->Branch("npixel1",&_npixel1,"_npixel1/F");
    _tree->Branch("npixel2",&_npixel2,"_npixel2/F");

    _tree->Branch("shr_ratiopixel0",&_shr_ratiopixel0,"_shr_ratiopixel0/F");
    _tree->Branch("shr_ratiopixel1",&_shr_ratiopixel1,"_shr_ratiopixel1/F");
    _tree->Branch("shr_ratiopixel2",&_shr_ratiopixel2,"_shr_ratiopixel2/F");

    _tree->Branch("trk_ratiopixel0",&_trk_ratiopixel0,"_trk_ratiopixel0/F");
    _tree->Branch("trk_ratiopixel1",&_trk_ratiopixel1,"_trk_ratiopixel1/F");
    _tree->Branch("trk_ratiopixel2",&_trk_ratiopixel2,"_trk_ratiopixel2/F");

    _tree->Branch("npixelsum",&_npixelsum,"_npixelsum/F");
    _tree->Branch("npixelavg",&_npixelavg,"_npixelavg/F");

    _tree->Branch("trkpixelsum",&_trkpixelsum,"_trkpixelsum/F");
    _tree->Branch("trkpixelavg",&_trkpixelavg,"_trkpixelavg/F");

    _tree->Branch("shrpixelsum",&_shrpixelsum,"_shrpixelsum/F");
    _tree->Branch("shrpixelavg",&_shrpixelavg,"_shrpixelavg/F");

    _tree->Branch("trk_ratiopixelsum",&_trk_ratiopixelsum,"_trk_ratiopixelsum/F");
    _tree->Branch("trk_ratiopixelavg",&_trk_ratiopixelavg,"_trk_ratiopixelavg/F");

    _tree->Branch("shr_ratiopixelsum",&_shr_ratiopixelsum,"_shr_ratiopixelsum/F");
    _tree->Branch("shr_ratiopixelavg",&_shr_ratiopixelavg,"_shr_ratiopixelavg/F");
    
    _tree->Branch("n_valid_planes",&_n_valid_planes,"_n_valid_planes/F");

  }

  bool SSNetPixelAna::process(IOManager& mgr)
  {
    auto const ev_img2d  = (EventImage2D*)(mgr.get_data(kProductImage2D,_ev_img2d_prod));
    if (!ev_img2d) throw larbys("Invalid event image producer provided");
    
    auto const ev_trk2d  = (EventImage2D*)(mgr.get_data(kProductImage2D,_ev_trk2d_prod));
    if (!ev_trk2d) throw larbys("Invalid event trk producer provided");

    auto const ev_shr2d  = (EventImage2D*)(mgr.get_data(kProductImage2D,_ev_shr2d_prod));
    if (!ev_shr2d) throw larbys("Invalid event shr producer provided");

    auto const ev_roi = (EventROI*)(mgr.get_data(kProductROI,_roi_prod));
    if (!ev_roi) throw larbys("Invalid ROI producer provided!");
    
    if (ev_roi->ROIArray().size() == 0) {
      throw larbys("NO ROI??");
    }

    auto const ev_true_roi = (EventROI*)(mgr.get_data(kProductROI,_true_roi_prod));
    if (!ev_true_roi) throw larbys("Invalid True ROI producer provided!");
    
    const auto& roi = ev_roi->ROIArray().front();
          
    _run    = (int) ev_img2d->run();
    _subrun = (int) ev_img2d->subrun();
    _event  = (int) ev_img2d->event();
    _entry  = (int) mgr.current_entry();

    LARCV_DEBUG() << "(r,s,e,e)=("<<_run<<","<<_subrun<<","<<_event<<","<<_entry<<")"<<std::endl;
    
    std::array<size_t,3> trk_pixel_v;
    std::array<size_t,3> shr_pixel_v;

    for(auto& v : trk_pixel_v) v = 0;
    for(auto& v : shr_pixel_v) v = 0;
    
    const auto& nu_roi = ev_true_roi->ROIArray().front();

    LARCV_DEBUG() << "PDG: " << nu_roi.PdgCode() << std::endl;
    auto tx = nu_roi.X();
    auto ty = nu_roi.Y();
    auto tz = nu_roi.Z();
    auto tt = nu_roi.T();

    auto const offset = _sce.GetPosOffsets(tx,ty,tz);
    
    auto scex = tx - offset[0] + 0.7;
    auto scey = ty + offset[1];
    auto scez = tz + offset[2];
    
    auto geo  = larutil::Geometry::GetME();
    auto larp = larutil::LArProperties::GetME();
    double xyz[3];
    double wire_v[3];

    const double tick = (scex / larp->DriftVelocity() + 4) * 2. + 3200.;
      
    xyz[0] = scex;
    xyz[1] = scey;
    xyz[2] = scez;

    try {
      wire_v[0] = geo->NearestWire(xyz,0);
      wire_v[1] = geo->NearestWire(xyz,1);
      wire_v[2] = geo->NearestWire(xyz,2);
    } catch(const std::exception& e) {
      throw larbys("Could not find nearest wire");
    }


    float n_valid_planes = 0;
    for (size_t plane=0; plane<3; ++plane) {
      
      const auto& img2d = ev_img2d->Image2DArray().at(plane);
      const auto& trk2d = ev_trk2d->Image2DArray().at(plane);
      const auto& shr2d = ev_shr2d->Image2DArray().at(plane);
      
      const auto& bb = roi.BB(plane);

      auto const& wire = wire_v[plane];

      float xpixel = -1.0;
      float ypixel = -1.0;
      
      if( bb.min_x() <= wire && wire <= bb.max_x() &&
	  bb.min_y() <= tick && tick <= bb.max_y() ) {

	xpixel = wire - bb.min_x();
	ypixel = (tick - bb.min_y()) / 6.0;
	
	LARCV_DEBUG() << "Set (xpixel,ypixel)=("<<xpixel<<","<<ypixel<<")"<<std::endl;
	n_valid_planes +=1;
      } else { continue; }
    
      auto crop_img2d  = img2d.crop(bb);
      auto crop_trk2d  = trk2d.crop(bb);
      auto crop_shr2d  = shr2d.crop(bb);

      crop_img2d.threshold(10,0);
      crop_shr2d.threshold(10,0);
      crop_trk2d.threshold(10,0);
      
      size_t trk_pixels = 0;
      size_t shr_pixels = 0;

      ypixel = crop_img2d.meta().rows() - ypixel;

      // auto mat0 = as_gray_mat(crop_img2d,0,255,1);
      // auto mat1 = as_gray_mat(crop_trk2d,0,255,1);
      // auto mat2 = as_gray_mat(crop_shr2d,0,255,1);

      // cv::threshold(mat0,mat0,1,255,1);
      // cv::threshold(mat1,mat1,1,255,1);
      // cv::threshold(mat2,mat2,1,255,1);
      
      // auto mat3 = mat2.clone();
      // mat3.setTo(cv::Scalar(255.0));
      
      // mat3.at<uchar>((int)ypixel,(int)xpixel) = (uchar)0.0;

      // if (plane==0) {
      // 	cv::imwrite("img0.png"   ,mat0);
      // 	cv::imwrite("trk0.png"   ,mat1);
      // 	cv::imwrite("shr0.png"   ,mat2);
      // 	cv::imwrite("vtx0.png"   ,mat3);
      // }

      // if (plane==1) {
      // 	cv::imwrite("img1.png"   ,mat0);
      // 	cv::imwrite("trk1.png"   ,mat1);
      // 	cv::imwrite("shr1.png"   ,mat2);
      // 	cv::imwrite("vtx1.png"   ,mat3);
      // }

      // if (plane==2) {
      // 	cv::imwrite("img2.png"   ,mat0);
      // 	cv::imwrite("trk2.png"   ,mat1);
      // 	cv::imwrite("shr2.png"   ,mat2);
      // 	cv::imwrite("vtx2.png"   ,mat3);
      // }
	
      for(size_t row=0; row < crop_img2d.meta().rows(); ++row) {
	for(size_t col=0; col < crop_img2d.meta().cols(); ++col) {

	  auto trk_px   = crop_trk2d.pixel(row,col);
	  auto shr_px   = crop_shr2d.pixel(row,col);
	  
	  float dx = std::pow((float)row - (float)ypixel,2);
	  float dy = std::pow((float)col - (float)xpixel,2);
	  
	  float dd =  dx + dy;
	  dd = std::sqrt(dd);

	  if (dd < (float)_crop_radius) {
	    if (trk_px) trk_pixels++;
	    if (shr_px) shr_pixels++;
	  }

	}
      }

      shr_pixel_v[plane] = shr_pixels;
      trk_pixel_v[plane] = trk_pixels;

      // if ((shr_pixels + trk_pixels) == 0) {
      // 	LARCV_CRITICAL() << "Vertex @plane="<<plane<<" & no pixels encountered" <<std::endl;
      // 	throw larbys();
      // }
      
    }

    //
    // ROI region
    //

    _trkpixel0 = trk_pixel_v[0];
    _trkpixel1 = trk_pixel_v[1];
    _trkpixel2 = trk_pixel_v[2];

    _shrpixel0 = shr_pixel_v[0];
    _shrpixel1 = shr_pixel_v[1];
    _shrpixel2 = shr_pixel_v[2];

    _npixel0 = _trkpixel0 + _shrpixel0;
    _npixel1 = _trkpixel1 + _shrpixel1;
    _npixel2 = _trkpixel2 + _shrpixel2;
    
    _shr_ratiopixel0 = _npixel0 ? (float)_shrpixel0 / (float)_npixel0 : 0;
    _shr_ratiopixel1 = _npixel1 ? (float)_shrpixel1 / (float)_npixel1 : 0;
    _shr_ratiopixel2 = _npixel2 ? (float)_shrpixel2 / (float)_npixel2 : 0;

    _trk_ratiopixel0 = _npixel0 ? (float)_trkpixel0 / (float)_npixel0 : 0;
    _trk_ratiopixel1 = _npixel1 ? (float)_trkpixel1 / (float)_npixel1 : 0;
    _trk_ratiopixel2 = _npixel2 ? (float)_trkpixel2 / (float)_npixel2 : 0;
    
    _npixelsum = _npixel0 + _npixel1 + _npixel2;
    _npixelavg = _npixelsum / n_valid_planes;

    _trkpixelsum = _trkpixel0 + _trkpixel1 + _trkpixel2;
    _trkpixelavg = _trkpixelsum / n_valid_planes;

    _shrpixelsum = _shrpixel0 + _shrpixel1 + _shrpixel2;
    _shrpixelavg = _shrpixelsum / n_valid_planes;

    _trk_ratiopixelsum = _trk_ratiopixel0 + _trk_ratiopixel1 + _trk_ratiopixel2;
    _trk_ratiopixelavg = _trk_ratiopixelsum / n_valid_planes;

    _shr_ratiopixelsum = _shr_ratiopixel0 + _shr_ratiopixel1 + _shr_ratiopixel2;
    _shr_ratiopixelavg = _shr_ratiopixelsum / n_valid_planes;

    _n_valid_planes = n_valid_planes;
    
    _tree->Fill();
    return true;
  }

  void SSNetPixelAna::finalize()
  {
    if(has_ana_file()) {
      _tree->Write();
    }
  }

}
#endif
