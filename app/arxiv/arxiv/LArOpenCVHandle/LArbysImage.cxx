#ifndef __LARBYSIMAGE_CXX__
#define __LARBYSIMAGE_CXX__

#include <sstream>

#include "LArbysImage.h"
#include "Base/ConfigManager.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventPGraph.h"
#include "DataFormat/EventPixel2D.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArbysUtils.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoData/InfoCollection.h"

namespace larcv {

  static LArbysImageProcessFactory __global_LArbysImageProcessFactory__;

  LArbysImage::LArbysImage(const std::string name)
    : ProcessBase(name),
      _image_cluster_cfg("aho"),
      _PreProcessor(),
      _LArbysImageMaker()
  {}
      
  void LArbysImage::configure(const PSet& cfg)
  {
    _rse_producer         = cfg.get<std::string>("RSEImageProducer","wire");
    _adc_producer         = cfg.get<std::string>("ADCImageProducer");
    _track_producer       = cfg.get<std::string>("TrackImageProducer","");
    _shower_producer      = cfg.get<std::string>("ShowerImageProducer","");
    _thrumu_producer      = cfg.get<std::string>("ThruMuImageProducer","");
    _stopmu_producer      = cfg.get<std::string>("StopMuImageProducer","");
    _channel_producer     = cfg.get<std::string>("ChStatusImageProducer","");
    _roi_producer         = cfg.get<std::string>("ROIProducer","");
    _output_producer      = cfg.get<std::string>("OutputImageProducer","");
    auto tags_datatype    = cfg.get<size_t>("CosmicTagDataType");
    _tags_datatype = (ProductType_t) tags_datatype;
    
    _mask_thrumu_pixels = cfg.get<bool>("MaskThruMu",false);
    _mask_stopmu_pixels = cfg.get<bool>("MaskStopMu",false);

    _LArbysImageMaker.Configure(cfg.get<larcv::PSet>("LArbysImageMaker"));

    _preprocess = cfg.get<bool>("PreProcess");
    if (_preprocess) {
      LARCV_INFO() << "Preprocessing image" << std::endl;
      _PreProcessor.Configure(cfg.get<larcv::PSet>("PreProcessor"));
    }
    
    _process_count = 0;
    _process_time_image_extraction = 0;
    _process_time_analyze = 0;
    _process_time_cluster_storage = 0;


    _image_cluster_cfg = ::fcllite::PSet(_alg_mgr.Name(),cfg.get_pset(_alg_mgr.Name()).data_string());
    
    _vertex_algo_name    = cfg.get<std::string>("VertexAlgoName");
    _par_algo_name       = cfg.get<std::string>("ParticleAlgoName");
    _3D_algo_name        = cfg.get<std::string>("3DAlgoName");

    _vertex_algo_vertex_offset = cfg.get<size_t>("VertexAlgoVertexOffset",0);
    _par_algo_par_offset       = cfg.get<size_t>("ParticleAlgoParticleOffset",0);
  }
  
  void LArbysImage::initialize()
  {
    _thrumu_image_v.clear();
    _stopmu_image_v.clear();
    
    _alg_mgr.Configure(_image_cluster_cfg.get_pset(_alg_mgr.Name()));
    
    const auto& data_man = _alg_mgr.DataManager();
    
    _vertex_algo_id = larocv::kINVALID_ALGO_ID;
    if (!_vertex_algo_name.empty()) {
      _vertex_algo_id = data_man.ID(_vertex_algo_name);
      if (_vertex_algo_id == larocv::kINVALID_ALGO_ID)
	throw larbys("Specified invalid vertex algorithm");
    }

    _par_algo_id = larocv::kINVALID_ALGO_ID;
    if (!_par_algo_name.empty()) {
      _par_algo_id = data_man.ID(_par_algo_name);
      if (_par_algo_id == larocv::kINVALID_ALGO_ID) {
	throw larbys("Specified invalid particle algorithm");
      }
    }

    _3D_algo_id = larocv::kINVALID_ALGO_ID;
    if (!_3D_algo_name.empty()) {
      _3D_algo_id = data_man.ID(_3D_algo_name);
      if (_3D_algo_id == larocv::kINVALID_ALGO_ID) {
	throw larbys("Specified invalid 3D info algorithm");
      }
    }

  }

  void LArbysImage::get_rsee(IOManager& mgr,std::string producer,uint& run, uint& subrun, uint& event, uint& entry) {
    auto ev_image = (EventImage2D*)(mgr.get_data(kProductImage2D,producer));
    run    = (uint) ev_image->run();
    subrun = (uint) ev_image->subrun();
    event  = (uint) ev_image->event();
    entry  = (uint) mgr.current_entry();

    if (run == kINVALID_UINT)  {
      LARCV_CRITICAL() << "Invalid run number @ entry " << entry << " from " << producer << " producer" << std::endl;
      throw larbys();
    }

    if (subrun == kINVALID_UINT)  {
      LARCV_CRITICAL() << "Invalid subrun number @ entry " << entry << " from " << producer << " producer" << std::endl;
      throw larbys();
    }

    if (event == kINVALID_UINT)  {
      LARCV_CRITICAL() << "Invalid event number @ entry " << entry << " from " << producer << " producer" << std::endl;
      throw larbys();
    }

    if (entry == kINVALID_UINT)  {
      LARCV_CRITICAL() << "Invalid entry number @ entry "  << entry << " from " << producer << " producer" << std::endl;
      throw larbys();
    }
    
  }
  
  
  const std::vector<larcv::Image2D>& LArbysImage::get_image2d(IOManager& mgr, std::string producer) {
    
    if(!producer.empty()) {
      LARCV_DEBUG() << "Extracting " << producer << " Image\n" << std::endl;
      auto ev_image = (EventImage2D*)(mgr.get_data(kProductImage2D,producer));
      if(!ev_image) {
	LARCV_CRITICAL() << "Image by producer " << producer << " not found..." << std::endl;
	throw larbys();
      }
      return ev_image->Image2DArray();
    }
    LARCV_DEBUG() << "... this producer empty" << std::endl;
    return _empty_image_v;
  }

  
  bool LArbysImage::process(IOManager& mgr)
  {
    LARCV_DEBUG() << "Process index " << mgr.current_entry() << std::endl;
    uint run,subrun,event,entry;
    get_rsee(mgr,_rse_producer,run,subrun,event,entry);
    
    _alg_mgr.SetRSEE(run,subrun,event,entry);
    
    bool status = true;

    if(_roi_producer.empty()) {
      size_t pidx = 0;          
      const auto& adc_image_v    = get_image2d(mgr,_adc_producer);
      const auto& track_image_v  = get_image2d(mgr,_track_producer);
      const auto& shower_image_v = get_image2d(mgr,_shower_producer);
      const auto& chstat_image_v = get_image2d(mgr,_channel_producer);
      _LArbysImageMaker.ConstructCosmicImage(mgr, _thrumu_producer, _tags_datatype, adc_image_v, _thrumu_image_v);
      _LArbysImageMaker.ConstructCosmicImage(mgr, _stopmu_producer, _tags_datatype, adc_image_v, _stopmu_image_v);
      const auto& thrumu_image_v = _thrumu_image_v;
      const auto& stopmu_image_v = _stopmu_image_v;
      
      assert(adc_image_v.size());
      assert(track_image_v.empty()  || adc_image_v.size() == track_image_v.size());
      assert(shower_image_v.empty() || adc_image_v.size() == shower_image_v.size());
      assert(chstat_image_v.empty() || adc_image_v.size() == chstat_image_v.size());
      assert(thrumu_image_v.empty() || adc_image_v.size() == thrumu_image_v.size());
      assert(stopmu_image_v.empty() || adc_image_v.size() == stopmu_image_v.size());

      bool mask_thrumu = _mask_thrumu_pixels && !thrumu_image_v.empty();
      bool mask_stopmu = _mask_stopmu_pixels && !stopmu_image_v.empty();

      if(!mask_thrumu && !mask_stopmu) {
	LARCV_DEBUG() << "Reconstruct no mask thrumu and no mask stopmu" << std::endl;
	status = Reconstruct(adc_image_v,
			     track_image_v,shower_image_v,
			     thrumu_image_v,stopmu_image_v,
			     chstat_image_v);
	status = status && StoreParticles(mgr,adc_image_v,pidx);
	
      } // don't mask stop mu or thru mu
      else { 
	
	auto copy_adc_image_v    = adc_image_v;
	auto copy_track_image_v  = track_image_v;
	auto copy_shower_image_v = shower_image_v;
	LARCV_DEBUG() << "ADC image size " << adc_image_v.size() << std::endl;
	for(size_t plane=0; plane<adc_image_v.size(); ++plane) {
	  
	  if(mask_thrumu) {
	    LARCV_DEBUG() << "Masking thrumu plane " << plane << std::endl;
	    mask_image(copy_adc_image_v[plane], thrumu_image_v[plane]);
	    if(!copy_track_image_v.empty() ) mask_image(copy_track_image_v[plane],  thrumu_image_v[plane]);
	    if(!copy_shower_image_v.empty()) mask_image(copy_shower_image_v[plane], thrumu_image_v[plane]);
	  }
	  
	  if(mask_stopmu) {
	    LARCV_DEBUG() << "Masking stopmu plane " << plane << std::endl;
	    mask_image(copy_adc_image_v[plane], stopmu_image_v[plane]);
	    if(!copy_track_image_v.empty() ) mask_image(copy_track_image_v[plane],  stopmu_image_v[plane]);
	    if(!copy_shower_image_v.empty()) mask_image(copy_shower_image_v[plane], stopmu_image_v[plane]);
	  }

	}

	status = Reconstruct(copy_adc_image_v,
			     copy_track_image_v,copy_shower_image_v,
			     thrumu_image_v, stopmu_image_v,
			     chstat_image_v);
	status = status && StoreParticles(mgr,copy_adc_image_v,pidx);
      } // mask stop mu or thru mu
      
    }
    //ROI exists, process per crop
    else{

      size_t pidx = 0;
      
      const auto& adc_image_v    = get_image2d(mgr,_adc_producer);
      const auto& track_image_v  = get_image2d(mgr,_track_producer);
      const auto& shower_image_v = get_image2d(mgr,_shower_producer);
      const auto& chstat_image_v = get_image2d(mgr,_channel_producer);
      
      _LArbysImageMaker.ConstructCosmicImage(mgr, _thrumu_producer, _tags_datatype, adc_image_v, _thrumu_image_v);
      _LArbysImageMaker.ConstructCosmicImage(mgr, _stopmu_producer, _tags_datatype, adc_image_v, _stopmu_image_v);

      const auto& thrumu_image_v = _thrumu_image_v;
      const auto& stopmu_image_v = _stopmu_image_v;

      auto pixel_height = adc_image_v.front().meta().pixel_height();
      auto pixel_width  = adc_image_v.front().meta().pixel_width();

      LARCV_DEBUG() << "Recieved ADC image (w,h)=("<<pixel_width<<","<<pixel_height<<")"<<std::endl;

      assert(adc_image_v.size());
      assert(track_image_v.empty()  || adc_image_v.size() == track_image_v.size());
      assert(shower_image_v.empty() || adc_image_v.size() == shower_image_v.size());
      assert(chstat_image_v.empty() || adc_image_v.size() == chstat_image_v.size());
      assert(thrumu_image_v.empty() || adc_image_v.size() == thrumu_image_v.size());
      assert(stopmu_image_v.empty() || adc_image_v.size() == stopmu_image_v.size());

      LARCV_DEBUG() << "ADC image sz " << adc_image_v.size() << std::endl;
      LARCV_DEBUG() << "TRK image sz " << track_image_v.size() << std::endl;
      LARCV_DEBUG() << "SHR image sz " << shower_image_v.size() << std::endl;
      LARCV_DEBUG() << "CHS image sz " << chstat_image_v.size() << std::endl;
      LARCV_DEBUG() << "TMU image sz " << thrumu_image_v.size() << std::endl;
      LARCV_DEBUG() << "SMU image sz " << stopmu_image_v.size() << std::endl;
      
      LARCV_DEBUG() << "Extracting " << _roi_producer << " ROI\n" << std::endl;
      if( !(mgr.get_data(kProductROI,_roi_producer)) ) {
	LARCV_CRITICAL() << "ROI by producer " << _roi_producer << " not found..." << std::endl;
	throw larbys();
      }
      const auto& roi_v = ((EventROI*)(mgr.get_data(kProductROI,_roi_producer)))->ROIArray();
	
      LARCV_DEBUG() << "Got " << roi_v.size() << " ROIs" << std::endl;
      for(const auto& roi : roi_v) {
	LARCV_DEBUG() << "ROI @ " << &roi << " pidx " << pidx << std::endl;

	_current_roi = larcv::ROI();
	
	const auto& bb_v = roi.BB();

	assert(bb_v.size() == adc_image_v.size());

	std::vector<larcv::Image2D> crop_adc_image_v;
	std::vector<larcv::Image2D> crop_track_image_v;
	std::vector<larcv::Image2D> crop_shower_image_v;
	std::vector<larcv::Image2D> crop_thrumu_image_v;
	std::vector<larcv::Image2D> crop_stopmu_image_v;
	std::vector<larcv::Image2D> crop_chstat_image_v;

	for(size_t plane=0; plane<bb_v.size(); ++plane) {

	  const auto& bb           = bb_v[plane];
	  const auto& adc_image    = adc_image_v[plane];

	  LARCV_DEBUG() << "bb:   " << bb.dump();
	  LARCV_DEBUG() << "adc:  " << adc_image.meta().dump();

	  crop_adc_image_v.emplace_back(adc_image.crop(bb));

	  _current_roi.AppendBB(crop_adc_image_v.at(plane).meta());

	  LARCV_DEBUG() << "crop: " << crop_adc_image_v.back().meta().dump();
	    
	  if(!track_image_v.empty()) {
	    const auto& track_image  = track_image_v[plane];
	    crop_track_image_v.emplace_back(track_image.crop(bb));
	  }

	  if(!shower_image_v.empty()) {
	    const auto& shower_image = shower_image_v[plane];
	    crop_shower_image_v.emplace_back(shower_image.crop(bb));
	  }

	  if(!thrumu_image_v.empty()) {
	    const auto& thrumu_image = thrumu_image_v[plane];
	    crop_thrumu_image_v.emplace_back(thrumu_image.crop(bb));
	  }

	  if(!stopmu_image_v.empty()) {
	    const auto& stopmu_image = stopmu_image_v[plane];
	    crop_stopmu_image_v.emplace_back(stopmu_image.crop(bb));
	  }

	  if(!chstat_image_v.empty()) {
	    const auto& chstat_image = chstat_image_v[plane];
	    crop_chstat_image_v.emplace_back(chstat_image.crop(bb));
	  }
	    
	  if(!crop_thrumu_image_v.empty() && _mask_thrumu_pixels) {
	    LARCV_DEBUG() << "Masking thrumu plane " << plane << std::endl;
	    mask_image(crop_adc_image_v[plane], crop_thrumu_image_v[plane]);
	    if(!crop_track_image_v.empty() ) mask_image(crop_track_image_v[plane],  crop_thrumu_image_v[plane]);
	    if(!crop_shower_image_v.empty()) mask_image(crop_shower_image_v[plane], crop_thrumu_image_v[plane]);
	  }

	  if(!crop_stopmu_image_v.empty() && _mask_stopmu_pixels) {
	    LARCV_DEBUG() << "Masking stopmu plane " << plane << std::endl;
	    mask_image(crop_adc_image_v[plane], crop_stopmu_image_v[plane]);
	    if(!crop_track_image_v.empty() ) mask_image(crop_track_image_v[plane],  crop_stopmu_image_v[plane]);
	    if(!crop_shower_image_v.empty()) mask_image(crop_shower_image_v[plane], crop_stopmu_image_v[plane]);
	  }

	}
	
	status = status && Reconstruct(crop_adc_image_v,
				       crop_track_image_v,
				       crop_shower_image_v,
				       crop_thrumu_image_v,
				       crop_stopmu_image_v,
				       crop_chstat_image_v);
	
	status = status && StoreParticles(mgr,crop_adc_image_v,pidx);
      } // end loop over ROI
    } // end image fed to reco

    LARCV_DEBUG() << "return " << status << std::endl;
    return status;
  }

  bool LArbysImage::StoreParticles(IOManager& iom,
				   const std::vector<Image2D>& adc_image_v,
				   size_t& pidx) {
    
    LARCV_DEBUG() << "start" << std::endl;

    // nothing to be done
    if (_vertex_algo_id == larocv::kINVALID_ALGO_ID) {
      LARCV_INFO() << "Nothing to be done..." << std::endl;
      return true;
    }
    
    const auto& adc_cvimg_orig_v = _alg_mgr.InputImages(larocv::ImageSetID_t::kImageSetWire);
    static std::vector<cv::Mat> adc_cvimg_v;
    adc_cvimg_v.clear();
    adc_cvimg_v.resize(3);

    auto event_pgraph        = (EventPGraph*)  iom.get_data(kProductPGraph,_output_producer);
    auto event_ctor_pixel    = (EventPixel2D*) iom.get_data(kProductPixel2D,_output_producer+"_ctor");
    auto event_img_pixel     = (EventPixel2D*) iom.get_data(kProductPixel2D,_output_producer+"_img");
    
    const auto& data_mgr = _alg_mgr.DataManager();
    const auto& ass_man  = data_mgr.AssManager();
    
    const auto vtx3d_array = (larocv::data::Vertex3DArray*) data_mgr.Data(_vertex_algo_id, _vertex_algo_vertex_offset);
    const auto& vtx3d_v = vtx3d_array->as_vector();
    
    const auto par_array = (larocv::data::ParticleArray*) data_mgr.Data(_par_algo_id,_par_algo_par_offset);
    const auto& par_v = par_array->as_vector();

    const auto info_3D_array = (larocv::data::Info3DArray*) data_mgr.Data(_3D_algo_id,0);
    const auto& info_3D_v = info_3D_array->as_vector();

    auto n_reco_vtx = vtx3d_v.size();
    
    LARCV_DEBUG() << "Got " << n_reco_vtx << " reconstructed vertex" << std::endl;

    for(size_t vtxid=0; vtxid< n_reco_vtx; ++vtxid) {
      
      const auto& vtx3d = vtx3d_v[vtxid];

      LARCV_DEBUG() << vtxid << ") @ (x,y,z) : ("<<vtx3d.x<<","<<vtx3d.y<<","<<vtx3d.z<<")"<<std::endl;

      for(size_t plane=0; plane<3; ++plane)
	adc_cvimg_v[plane] = adc_cvimg_orig_v[plane].clone();

      auto par_id_v = ass_man.GetManyAss(vtx3d,par_array->ID());
      if (par_id_v.empty()) {
	LARCV_DEBUG() << "No associated particles to vertex " << vtxid << std::endl;
	continue;
      }

      PGraph pgraph;
      for(const auto& par_id : par_id_v) {
	
	const auto& par = par_v.at(par_id);

	const auto info3d_id   = ass_man.GetOneAss(par,info_3D_array->ID());
	if (info3d_id == kINVALID_SIZE) throw larbys("Particle unassociated to 3D info collection");
	const auto& par_info3d = info_3D_v.at(info3d_id);
	
	// New ROI for this particle
	ROI proi;
	
	// Store the vertex
	proi.Position(vtx3d.x,vtx3d.y,vtx3d.z,kINVALID_DOUBLE);

	// Store the end point
	proi.EndPosition(par_info3d.overall_pca_end_pt.at(0),
			 par_info3d.overall_pca_end_pt.at(1),
			 par_info3d.overall_pca_end_pt.at(2),
			 kINVALID_DOUBLE);
	
	// Store the type
	if      (par.type==larocv::data::ParticleType_t::kTrack)   proi.Shape(kShapeTrack);
	else if (par.type==larocv::data::ParticleType_t::kShower)  proi.Shape(kShapeShower);
	  
	// Push the ROI into the PGraph
	LARCV_DEBUG() << " @ pg array index " << pidx << std::endl;

	for(size_t plane=0; plane<3; ++plane) 
	  proi.AppendBB(adc_image_v[plane].meta());
	
	pgraph.Emplace(std::move(proi),pidx);
	pidx++;

	// @ Each plane, store pixels and contour per matched particle
	for(size_t plane=0; plane<3; ++plane) {
	  const auto& pmeta = adc_image_v[plane].meta();
	    
	  std::vector<Pixel2D> pixel_v, ctor_v;
	    
	  const auto& pcluster = par._par_v[plane];
	  const auto& pctor = pcluster._ctor;
	  
	  const auto& img2d = adc_image_v[plane];
	  auto& cvimg = adc_cvimg_v[plane];
	  
	  if(!pctor.empty()) {
	    auto masked = larocv::MaskImage(cvimg,pctor,0,false);
	    auto par_pixel_v = larocv::FindNonZero(masked);
	    pixel_v.reserve(par_pixel_v.size());
	    cvimg = larocv::MaskImage(cvimg,pctor,0,true);
	    
	    // Store Image2D pixel values
	    pixel_v.reserve(par_pixel_v.size());
	    for (const auto& px : par_pixel_v) {
	      auto col  = cvimg.cols - px.x - 1;
	      auto row  = px.y;
	      auto gray = img2d.pixel(col,row);
	      pixel_v.emplace_back(col,row);
	      pixel_v.back().Intensity(gray);
	    }

	    // Store contour
	    ctor_v.reserve(pctor.size());
	    for(const auto& pt : pctor)  {
	      auto col  = cvimg.cols - pt.x - 1;
	      auto row  = pt.y;
	      auto gray = 1.0;
	      ctor_v.emplace_back(row,col);
	      ctor_v.back().Intensity(gray);
	    }
	  }
	  
	  Pixel2DCluster pixcluster(std::move(pixel_v));
	  event_img_pixel->Emplace(plane,std::move(pixcluster),pmeta);

	  Pixel2DCluster pixctor(std::move(ctor_v));
	  event_ctor_pixel->Emplace(plane,std::move(pixctor),pmeta);

	} // end this plane
      } // end this particle
      
      event_pgraph->Emplace(std::move(pgraph));
    } // end vertex

    LARCV_DEBUG() << "Event pgraph size " << event_pgraph->PGraphArray().size() << std::endl;
    return true;
  }
  


  bool LArbysImage::Reconstruct(const std::vector<larcv::Image2D>& adc_image_v,
				const std::vector<larcv::Image2D>& track_image_v,
				const std::vector<larcv::Image2D>& shower_image_v,
				const std::vector<larcv::Image2D>& thrumu_image_v,
				const std::vector<larcv::Image2D>& stopmu_image_v,
				const std::vector<larcv::Image2D>& chstat_image_v)
  {
    LARCV_DEBUG() << "start" << std::endl;
    _adc_img_mgr.clear();
    _track_img_mgr.clear();
    _shower_img_mgr.clear();
    _thrumu_img_mgr.clear();
    _stopmu_img_mgr.clear();
    _chstat_img_mgr.clear();

    _alg_mgr.ClearData();
    
    larocv::Watch watch_all, watch_one;
    watch_all.Start();
    watch_one.Start();

    LARCV_DEBUG() << "Extract adc prod: " << _adc_producer << std::endl;
    LARCV_DEBUG() << "... sz " << adc_image_v.size() << std::endl;
    for(auto& img_data : _LArbysImageMaker.ExtractImage(adc_image_v)) {
      _adc_img_mgr.emplace_back(std::move(std::get<0>(img_data)),
				std::move(std::get<1>(img_data)));
    }
    
    if(!_track_producer.empty()) {
      LARCV_DEBUG() << "Extract track prod: " << _track_producer << std::endl;
      LARCV_DEBUG() << "... sz " << track_image_v.size() << std::endl;
      for(auto& img_data : _LArbysImageMaker.ExtractImage(track_image_v))  { 
	_track_img_mgr.emplace_back(std::move(std::get<0>(img_data)),
				    std::move(std::get<1>(img_data)));
      }
    }

    if(!_shower_producer.empty()) {
      LARCV_DEBUG() << "Extract shower prod: " << _shower_producer << std::endl;
      LARCV_DEBUG() << "... sz " << shower_image_v.size() << std::endl;
      for(auto& img_data : _LArbysImageMaker.ExtractImage(shower_image_v)) {
	_shower_img_mgr.emplace_back(std::move(std::get<0>(img_data)),
				     std::move(std::get<1>(img_data)));
      }
    }

    if(!_stopmu_producer.empty()) {
      LARCV_DEBUG() << "Extract stopmu prod: " << _stopmu_producer << std::endl;
      LARCV_DEBUG() << "... sz " << stopmu_image_v.size() << std::endl;
      for(auto& img_data : _LArbysImageMaker.ExtractImage(stopmu_image_v)) {
	_thrumu_img_mgr.emplace_back(std::move(std::get<0>(img_data)),
				     std::move(std::get<1>(img_data)));
      }
    }
    if(!_thrumu_producer.empty()) {
      LARCV_DEBUG() << "Extract thrupmu prod: " << _thrumu_producer << std::endl;
      LARCV_DEBUG() << "... sz " << thrumu_image_v.size() << std::endl;      
      for(auto& img_data : _LArbysImageMaker.ExtractImage(thrumu_image_v)) {
	_stopmu_img_mgr.emplace_back(std::move(std::get<0>(img_data)),
				     std::move(std::get<1>(img_data)));
      }
    }
    if(!_channel_producer.empty()) {
      LARCV_DEBUG() << "Extract channel prod: " << _channel_producer << std::endl;
      LARCV_DEBUG() << "... sz " << chstat_image_v.size() << std::endl;
      for(auto& img_data : _LArbysImageMaker.ExtractImage(chstat_image_v)) {
	_chstat_img_mgr.emplace_back(std::move(std::get<0>(img_data)),
				     std::move(std::get<1>(img_data)));
      }
    }
    
    _process_time_image_extraction += watch_one.WallTime();

    for (size_t plane = 0; plane < _adc_img_mgr.size(); ++plane) {
      auto       & img  = _adc_img_mgr.img_at(plane);
      const auto & meta = _adc_img_mgr.meta_at(plane);
      if (!meta.num_pixel_row() || !meta.num_pixel_column()) continue;
      LARCV_DEBUG() << "... add adc @plane="<<plane<<std::endl;
      _alg_mgr.Add(img, meta, larocv::ImageSetID_t::kImageSetWire);
    }

    for (size_t plane = 0; plane < _track_img_mgr.size(); ++plane) {
      auto       & img  = _track_img_mgr.img_at(plane);
      const auto & meta = _track_img_mgr.meta_at(plane);
      if (!meta.num_pixel_row() || !meta.num_pixel_column()) continue;
      LARCV_DEBUG() << "... add track @plane="<<plane<<std::endl;
      _alg_mgr.Add(img, meta, larocv::ImageSetID_t::kImageSetTrack);
    }
    
    for (size_t plane = 0; plane < _shower_img_mgr.size(); ++plane) {
      auto       & img  = _shower_img_mgr.img_at(plane);
      const auto & meta = _shower_img_mgr.meta_at(plane);
      if (!meta.num_pixel_row() || !meta.num_pixel_column()) continue;
      LARCV_DEBUG() << "... add shower @plane="<<plane<<std::endl;
      _alg_mgr.Add(img, meta, larocv::ImageSetID_t::kImageSetShower);
    }

    for (size_t plane = 0; plane < _thrumu_img_mgr.size(); ++plane) {
      auto       & img  = _thrumu_img_mgr.img_at(plane);
      const auto & meta = _thrumu_img_mgr.meta_at(plane);
      if (!meta.num_pixel_row() || !meta.num_pixel_column()) continue;
      LARCV_DEBUG() << "... add thrumu @plane="<<plane<<std::endl;
      _alg_mgr.Add(img, meta, larocv::ImageSetID_t::kImageSetThruMu);
    }

    for (size_t plane = 0; plane < _stopmu_img_mgr.size(); ++plane) {
      auto       & img  = _stopmu_img_mgr.img_at(plane);
      const auto & meta = _stopmu_img_mgr.meta_at(plane);
      if (!meta.num_pixel_row() || !meta.num_pixel_column()) continue;
      LARCV_DEBUG() << "... add stopmu @plane="<<plane<<std::endl;
      _alg_mgr.Add(img, meta, larocv::ImageSetID_t::kImageSetStopMu);
    }

    for (size_t plane = 0; plane < _chstat_img_mgr.size(); ++plane) {
      auto       & img  = _chstat_img_mgr.img_at(plane);
      const auto & meta = _chstat_img_mgr.meta_at(plane);
      if (!meta.num_pixel_row() || !meta.num_pixel_column()) continue;
      LARCV_DEBUG() << "... add chstat @plane="<<plane<<std::endl;
      _alg_mgr.Add(img, meta, larocv::ImageSetID_t::kImageSetChStatus);
    }

    if (_preprocess) {
      // give a single plane @ a time to pre processor
      auto& adc_img_v= _alg_mgr.InputImages(larocv::ImageSetID_t::kImageSetWire);
      auto& trk_img_v= _alg_mgr.InputImages(larocv::ImageSetID_t::kImageSetTrack);
      auto& shr_img_v= _alg_mgr.InputImages(larocv::ImageSetID_t::kImageSetShower);
      auto nplanes = adc_img_v.size();
      for(size_t plane_id=0;plane_id<nplanes;++plane_id) {
	LARCV_DEBUG() << "Preprocess image set @ "<< " plane " << plane_id << std::endl;
	if (!_PreProcessor.PreProcess(adc_img_v[plane_id],trk_img_v[plane_id],shr_img_v[plane_id])) {
	  LARCV_CRITICAL() << "... could not be preprocessed, abort!" << std::endl;
	  throw larbys();
	}
      }
    }

    watch_one.Start();
    Process();
    _process_time_analyze += watch_all.WallTime();    
    ++_process_count;
    LARCV_DEBUG() << "end" << std::endl;
    return true;
  }

  void LArbysImage::Process() {
    _alg_mgr.Process();
  }

  void LArbysImage::finalize()
  {
    if ( has_ana_file() )  {
      _alg_mgr.Finalize(&(ana_file()));
    }
  }
  
}
#endif
