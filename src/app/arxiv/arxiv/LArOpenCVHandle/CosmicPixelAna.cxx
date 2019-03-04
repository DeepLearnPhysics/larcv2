#ifndef __COSMICPIXELANA_CXX__
#define __COSMICPIXELANA_CXX__

#include "CosmicPixelAna.h"
#include "LArbysImageMaker.h"
#include "DataFormat/EventROI.h"
#include "LArbysUtils.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/LArProperties.h"
#include <array>
#include <cassert>
#include "CVUtil/CVUtil.h"

namespace larcv {
  
  static CosmicPixelAnaProcessFactory __global_CosmicPixelAnaProcessFactory__;
  
  CosmicPixelAna::CosmicPixelAna(const std::string name) :
    ProcessBase(name),
    _LArbysImageMaker(),
    _tree(nullptr)
  {}
    
  void CosmicPixelAna::configure(const PSet& cfg)
  {
    _ev_img2d_prod   = cfg.get<std::string>("EventImage2DProducer"); 
    _seg_img2d_prod  = cfg.get<std::string>("SegmentImage2DProducer");
    _thrumu_img_prod = cfg.get<std::string>("ThruMuProducer");
    _stopmu_img_prod = cfg.get<std::string>("StopMuProducer");
    _roi_prod        = cfg.get<std::string>("ROIProducer");
    _true_roi_prod   = cfg.get<std::string>("TrueROIProducer");
    _crop_radius     = cfg.get<size_t>("CropRadius");
    auto tags_datatype        = cfg.get<size_t>("CosmicTagDataType");
    _tags_datatype = (ProductType_t) tags_datatype; 
   
    _LArbysImageMaker.Configure(cfg.get<PSet>("LArbysImageMaker"));
  }
  
  void CosmicPixelAna::initialize()
  {
    
    _tree = new TTree("EventCosmicPixelTree","");
    _tree->Branch("run",&_run,"run/I");
    _tree->Branch("subrun",&_subrun,"subrun/I");
    _tree->Branch("event",&_event,"event/I");
    _tree->Branch("entry",&_entry,"entry/I");

    _tree->Branch("nupixel0",&_nupixel0,"nupixel0/I");
    _tree->Branch("nupixel1",&_nupixel1,"nupixel1/I");
    _tree->Branch("nupixel2",&_nupixel2,"nupixel2/I");

    _tree->Branch("nupixelsum",&_nupixelsum,"nupixelsum/F");
    _tree->Branch("nupixelavg",&_nupixelavg,"nupixelavg/F");

    _tree->Branch("cosmicpixel0",&_cosmicpixel0,"cosmicpixel0/I");
    _tree->Branch("cosmicpixel1",&_cosmicpixel1,"cosmicpixel1/I");
    _tree->Branch("cosmicpixel2",&_cosmicpixel2,"cosmicpixel2/I");

    _tree->Branch("cosmicpixelsum",&_cosmicpixelsum,"cosmicpixelsum/F");
    _tree->Branch("cosmicpixelavg",&_cosmicpixelavg,"cosmicpixelavg/F");
    
    _tree->Branch("ratiopixel0",&_ratiopixel0,"ratiopixel0/F");
    _tree->Branch("ratiopixel1",&_ratiopixel1,"ratiopixel1/F");
    _tree->Branch("ratiopixel2",&_ratiopixel2,"ratiopixel2/F");

    _tree->Branch("ratiopixelsum",&_ratiopixelsum,"ratiopixelsum/F");
    _tree->Branch("ratiopixelavg",&_ratiopixelavg,"ratiopixelavg/F");

    _tree->Branch("vertex_nupixel0",&_vertex_nupixel0,"vertex_nupixel0/I");
    _tree->Branch("vertex_nupixel1",&_vertex_nupixel1,"vertex_nupixel1/I");
    _tree->Branch("vertex_nupixel2",&_vertex_nupixel2,"vertex_nupixel2/I");

    _tree->Branch("vertex_nupixelsum",&_vertex_nupixelsum,"vertex_nupixelsum/F");
    _tree->Branch("vertex_nupixelavg",&_vertex_nupixelavg,"vertex_nupixelavg/F");

    _tree->Branch("vertex_cosmicpixel0",&_vertex_cosmicpixel0,"vertex_cosmicpixel0/I");
    _tree->Branch("vertex_cosmicpixel1",&_vertex_cosmicpixel1,"vertex_cosmicpixel1/I");
    _tree->Branch("vertex_cosmicpixel2",&_vertex_cosmicpixel2,"vertex_cosmicpixel2/I");

    _tree->Branch("vertex_cosmicpixelsum",&_vertex_cosmicpixelsum,"vertex_cosmicpixelsum/F");
    _tree->Branch("vertex_cosmicpixelavg",&_vertex_cosmicpixelavg,"vertex_cosmicpixelavg/F");
    
    _tree->Branch("vertex_ratiopixel0",&_vertex_ratiopixel0,"vertex_ratiopixel0/F");
    _tree->Branch("vertex_ratiopixel1",&_vertex_ratiopixel1,"vertex_ratiopixel1/F");
    _tree->Branch("vertex_ratiopixel2",&_vertex_ratiopixel2,"vertex_ratiopixel2/F");

    _tree->Branch("vertex_ratiopixelsum",&_vertex_ratiopixelsum,"vertex_ratiopixelsum/F");
    _tree->Branch("vertex_ratiopixelavg",&_vertex_ratiopixelavg,"vertex_ratiopixelavg/F");
 
    _tree->Branch("nearest_wire_error",&_nearest_wire_error,"nearest_wire_error/I");
    _tree->Branch("not_inside",&_not_inside,"not_inside/I");
  }

  bool CosmicPixelAna::process(IOManager& mgr)
  {
    if (_ev_img2d_prod.empty()) {
      LARCV_INFO() << "No event image provided nothing to do" << std::endl;
      return false;
    }
    
    auto const ev_img2d  = (EventImage2D*)(mgr.get_data(kProductImage2D,_ev_img2d_prod));
    if (!ev_img2d) throw larbys("Invalid event image producer provided");

    if (_seg_img2d_prod.empty()) {
      LARCV_INFO() << "No segment image provided nothing to do" << std::endl;
      return false;
    }
    
    auto const seg_img2d  = (EventImage2D*)(mgr.get_data(kProductImage2D,_seg_img2d_prod));
    if (!seg_img2d) throw larbys("Invalid segment image producer provided");

    assert(ev_img2d->Image2DArray().size() == seg_img2d->Image2DArray().size());
    for(size_t img_id=0; img_id<ev_img2d->Image2DArray().size(); ++img_id) {
      const auto& ev_img  = ev_img2d->Image2DArray().at(img_id);
      const auto& seg_img = seg_img2d->Image2DArray().at(img_id);

      if (ev_img.meta().rows() != seg_img.meta().rows()) {
	LARCV_WARNING() << "Event image and segmentation image differ in row size @ plane=" << img_id
			<< " ("<<ev_img.meta().rows()<<"!="<<seg_img.meta().rows()<<")"<<std::endl;
	
	return false;
      }

      if (ev_img.meta().cols() != seg_img.meta().cols()) {
	LARCV_WARNING() << "Event image and segmentation image differ in col size @ plane=" << img_id
			<< " ("<<ev_img.meta().cols()<<"!="<<seg_img.meta().cols()<<")"<<std::endl;
	
	return false;
      }
    }
    
    // auto const ev_thrumu = (EventPixel2D*)(mgr.get_data(kProductPixel2D,_thrumu_img_prod));
    // if (!ev_thrumu) throw larbys("Invalid ThruMu producer provided!");
    
    // auto const ev_stopmu = (EventPixel2D*)(mgr.get_data(kProductPixel2D,_stopmu_img_prod));
    // if (!ev_stopmu) throw larbys("Invalid StopMu producer provided!");

    auto const ev_roi = (EventROI*)(mgr.get_data(kProductROI,_roi_prod));
    if (!ev_roi) throw larbys("Invalid ROI producer provided!");
    
    auto const ev_true_roi = (EventROI*)(mgr.get_data(kProductROI,_true_roi_prod));
    if (!ev_true_roi) throw larbys("Invalid True ROI producer provided!");
    
    const auto& roi = ev_roi->ROIArray().front();
          
    _run    = (int) ev_img2d->run();
    _subrun = (int) ev_img2d->subrun();
    _event  = (int) ev_img2d->event();
    _entry  = (int) mgr.current_entry();

    LARCV_DEBUG() << "(r,s,e,e)=("<<_run<<","<<_subrun<<","<<_event<<","<<_entry<<")"<<std::endl;
    
    std::array<size_t,3> neutrino_pixel_v;
    std::array<size_t,3> cosmic_pixel_v;

    std::array<size_t,3> vertex_neutrino_pixel_v;
    std::array<size_t,3> vertex_cosmic_pixel_v;

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
    
    LARCV_DEBUG() << "(x,y,z,t)==>sce(x,y,z,t) : ("<<tx<<","<<ty<<","<<tz<<","<<tt<<")==>("<<scex<<","<<scey<<","<<scez<<","<<tt<<")"<<std::endl;

    auto geo  = larutil::Geometry::GetME();
    auto larp = larutil::LArProperties::GetME();
    double xyz[3];
    double wire_v[3];

    const double tick = (scex / larp->DriftVelocity() + 4) * 2. + 3200.;
      
    xyz[0] = scex;
    xyz[1] = scey;
    xyz[2] = scez;
    _nearest_wire_error = 0;
    try {
      wire_v[0] = geo->NearestWire(xyz,0);
      wire_v[1] = geo->NearestWire(xyz,1);
      wire_v[2] = geo->NearestWire(xyz,2);
    } catch(const std::exception& e) {
      _tree->Fill();
      _nearest_wire_error=1;
      return true;
    }


    std::vector<larcv::Image2D> thrumu_img2d_v;
    std::vector<larcv::Image2D> stopmu_img2d_v;
    
    _LArbysImageMaker.ConstructCosmicImage(mgr,_thrumu_img_prod,_tags_datatype,seg_img2d->Image2DArray(),thrumu_img2d_v);
    _LArbysImageMaker.ConstructCosmicImage(mgr,_stopmu_img_prod,_tags_datatype,seg_img2d->Image2DArray(),stopmu_img2d_v);
    
    _not_inside = 0;
    for (size_t plane=0; plane<3; ++plane) {
      
      const auto& img2d = seg_img2d->Image2DArray().at(plane);

      const auto& thrumu_img2d = thrumu_img2d_v[plane];
      const auto& stopmu_img2d = stopmu_img2d_v[plane];
	
      const auto& bb = roi.BB(plane);

      auto const& wire = wire_v[plane];

      float xpixel = -1.0;
      float ypixel = -1.0;
      
      if( bb.min_x() <= wire && wire <= bb.max_x() &&
	  bb.min_y() <= tick && tick <= bb.max_y() ) {

	xpixel = wire - bb.min_x();
	ypixel = (tick - bb.min_y()) / 6.0;
	
	LARCV_DEBUG() << "Set (xpixel,ypixel)=("<<xpixel<<","<<ypixel<<")"<<std::endl;
      } else {
	_tree->Fill();
	_not_inside = 1;
	return true;
      }
    
      auto crop_img2d  = img2d.crop(bb);
      auto crop_thrumu = thrumu_img2d.crop(bb);
      auto crop_stopmu = stopmu_img2d.crop(bb);
      
      auto crop_cosmic_img = crop_thrumu;
      crop_cosmic_img += crop_stopmu;

      crop_cosmic_img.eltwise(crop_img2d);
      
      size_t cosmic_pixels = 0;
      size_t neutrino_pixels = 0;

      size_t vertex_cosmic_pixels = 0;
      size_t vertex_neutrino_pixels = 0;
      
      assert(crop_cosmic_img.as_vector().size() == crop_img2d.as_vector().size());

      float x;
      float y;
      float d;

      x = kINVALID_FLOAT;
      y = kINVALID_FLOAT;
      d = kINVALID_FLOAT;

      LARCV_DEBUG()<<"loop @ plane="<<plane<<"(row,col)==("<<crop_cosmic_img.meta().rows()<<","<<crop_cosmic_img.meta().cols()<<")"<<std::endl;
      LARCV_DEBUG() << std::endl;


      ypixel = crop_cosmic_img.meta().rows() - ypixel;

      /*
      auto mat0 = as_gray_mat(crop_img2d,0,255,1);
      auto mat1 = as_gray_mat(crop_cosmic_img,0,255,1);

      cv::threshold(mat0,mat0,1,255,1);
      cv::threshold(mat1,mat1,1,255,1);

      auto mat2 = mat1.clone();
      mat2.setTo(cv::Scalar(255.0));
      
      mat2.at<uchar>((int)ypixel,(int)xpixel) = (uchar)0.0;

      if (plane==0) {
	cv::imwrite("out0.png"   ,mat0);
	cv::imwrite("cosmic0.png",mat1);
	cv::imwrite("vtx0.png"   ,mat2);
      }

      if (plane==1) {
	cv::imwrite("out1.png"   ,mat0);
	cv::imwrite("cosmic1.png",mat1);
	cv::imwrite("vtx1.png"   ,mat2);
      }

      if (plane==2) {
	cv::imwrite("out2.png"   ,mat0);
	cv::imwrite("cosmic2.png",mat1);
	cv::imwrite("vtx2.png"   ,mat2);
      }
      */
	
      for(size_t row=0; row < crop_cosmic_img.meta().rows(); ++row) {
	for(size_t col=0; col < crop_cosmic_img.meta().cols(); ++col) {
	  auto cosmic_px   = crop_cosmic_img.pixel(row,col);
	  auto neutrino_px = crop_img2d.pixel(row,col);
	  
	  if (cosmic_px)   cosmic_pixels++;
	  if (neutrino_px) neutrino_pixels++;

	  float dx = std::pow((float)row - (float)ypixel,2);
	  float dy = std::pow((float)col - (float)xpixel,2);
	  
	  float dd =  dx + dy;
	  dd = std::sqrt(dd);

	  if (dd < d) d = dd;
	  if (dx < x) x = dx;
	  if (dy < y) y = dy;
	  
	  if (dd < (float)_crop_radius) {
	    if (cosmic_px)   vertex_cosmic_pixels++;
	    if (neutrino_px) vertex_neutrino_pixels++;
	  }
	}
      }

      LARCV_DEBUG() << "@ plane="<<plane << " min d was " << d << std::endl;
      LARCV_DEBUG() << "@ plane="<<plane << " min x was " << x << std::endl;
      LARCV_DEBUG() << "@ plane="<<plane << " min y was " << y << std::endl;
      LARCV_DEBUG() << "@ plane="<<plane << " vtx co px " << vertex_cosmic_pixels << std::endl;
      LARCV_DEBUG() << "@ plane="<<plane << " vtx nu px " << vertex_neutrino_pixels << std::endl;

      if (d > (float)_crop_radius) throw larbys("Vertex not in ROI");

      cosmic_pixel_v[plane] = cosmic_pixels;
      neutrino_pixel_v[plane] = neutrino_pixels;
      
      vertex_cosmic_pixel_v[plane] = vertex_cosmic_pixels;
      vertex_neutrino_pixel_v[plane] = vertex_neutrino_pixels;
    }

    // if (_entry==41) std::exit(1);
      

    
    //
    // ROI region
    //
    _nupixel0 = neutrino_pixel_v[0];
    _nupixel1 = neutrino_pixel_v[1];
    _nupixel2 = neutrino_pixel_v[2];
    
    _cosmicpixel0 = cosmic_pixel_v[0];
    _cosmicpixel1 = cosmic_pixel_v[1];
    _cosmicpixel2 = cosmic_pixel_v[2];

    _ratiopixel0 = _nupixel0 ? (float)_cosmicpixel0 / (float)_nupixel0 : 0;
    _ratiopixel1 = _nupixel1 ? (float)_cosmicpixel1 / (float)_nupixel1 : 0;
    _ratiopixel2 = _nupixel2 ? (float)_cosmicpixel2 / (float)_nupixel2 : 0;

    _nupixelsum = _nupixel0 + _nupixel1 + _nupixel2;
    _nupixelavg = _nupixelsum / 3.0;

    _cosmicpixelsum = _cosmicpixel0 + _cosmicpixel1 + _cosmicpixel2;
    _cosmicpixelavg = _cosmicpixelsum / 3.0;

    _ratiopixelsum = _ratiopixel0 + _ratiopixel1 + _ratiopixel2;
    _ratiopixelavg = _ratiopixelsum / 3.0;
    
    //
    // Vertex Region
    //
    _vertex_nupixel0 = vertex_neutrino_pixel_v[0];
    _vertex_nupixel1 = vertex_neutrino_pixel_v[1];
    _vertex_nupixel2 = vertex_neutrino_pixel_v[2];
    
    _vertex_cosmicpixel0 = vertex_cosmic_pixel_v[0];
    _vertex_cosmicpixel1 = vertex_cosmic_pixel_v[1];
    _vertex_cosmicpixel2 = vertex_cosmic_pixel_v[2];

    _vertex_ratiopixel0 = _vertex_nupixel0 ? (float)_vertex_cosmicpixel0 / (float)_vertex_nupixel0 : 0;
    _vertex_ratiopixel1 = _vertex_nupixel1 ? (float)_vertex_cosmicpixel1 / (float)_vertex_nupixel1 : 0;
    _vertex_ratiopixel2 = _vertex_nupixel2 ? (float)_vertex_cosmicpixel2 / (float)_vertex_nupixel2 : 0;

    _vertex_nupixelsum = _vertex_nupixel0 + _vertex_nupixel1 + _vertex_nupixel2;
    _vertex_nupixelavg = _vertex_nupixelsum / 3.0;

    _vertex_cosmicpixelsum = _vertex_cosmicpixel0 + _vertex_cosmicpixel1 + _vertex_cosmicpixel2;
    _vertex_cosmicpixelavg = _vertex_cosmicpixelsum / 3.0;

    _vertex_ratiopixelsum = _vertex_ratiopixel0 + _vertex_ratiopixel1 + _vertex_ratiopixel2;
    _vertex_ratiopixelavg = _vertex_ratiopixelsum / 3.0;

    _tree->Fill();
    return true;
  }

  void CosmicPixelAna::finalize()
  {
    if(has_ana_file()) {
      _tree->Write();
    }
  }

}
#endif
