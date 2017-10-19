#ifndef __EXAMPLEANA_CXX__
#define __EXAMPLEANA_CXX__

#include "ExampleAna.h"
#include "LArOpenCV/ImageCluster/AlgoClass/PixelChunk.h"
#include "LArbysUtils.h"

#define PI 3.14159265

namespace larcv {

  static ExampleAnaProcessFactory __global_ExampleAnaProcessFactory__;

  ExampleAna::ExampleAna(const std::string name)
    : ProcessBase(name), _LArbysImageMaker(), _tree(nullptr)
  { }
  
  void ExampleAna::configure(const PSet& cfg)
  {

    _LArbysImageMaker.Configure(cfg.get<larcv::PSet>("LArbysImageMaker"));
    
    _img_prod           = cfg.get<std::string>("ImageProducer"       ,"tpc");
    _pgraph_prod        = cfg.get<std::string>("PGraphProducer"      ,"test");
    _pcluster_ctor_prod = cfg.get<std::string>("Pixel2DCtorProducer" ,"test_ctor");
    _pcluster_img_prod  = cfg.get<std::string>("Pixel2DImageProducer","test_img");
    _reco_roi_prod      = cfg.get<std::string>("RecoROIProducer"     ,"croimerge");
    
    _trk_img_prod = cfg.get<std::string>("TrackImageProducer" ,"");
    _shr_img_prod = cfg.get<std::string>("ShowerImageProducer","");
  }
  
  void ExampleAna::initialize()
  {

    //
    // Event Tree (per vertex)
    //
    _tree = new TTree("SignalTree","SignalTree");
    _tree->Branch("entry",&_entry,"entry/I");
    _tree->Branch("run",&_run,"run/I");
    _tree->Branch("subrun",&_subrun,"subrun/I");
    _tree->Branch("event",&_event,"event/I");

  }

  bool ExampleAna::process(IOManager& mgr)
  {
    _ev_img_v      = (EventImage2D*)mgr.get_data(kProductImage2D,_img_prod);

    if(!_trk_img_prod.empty())
      _ev_trk_img_v      = (EventImage2D*)mgr.get_data(kProductImage2D,_trk_img_prod);
    if(!_shr_img_prod.empty())
      _ev_shr_img_v      = (EventImage2D*)mgr.get_data(kProductImage2D,_shr_img_prod);
    
    _ev_roi_v      = (EventROI*)    mgr.get_data(kProductROI,_reco_roi_prod);
    _ev_pgraph_v   = (EventPGraph*) mgr.get_data(kProductPGraph,_pgraph_prod);
    _ev_pcluster_v = (EventPixel2D*)mgr.get_data(kProductPixel2D,_pcluster_img_prod);
    _ev_ctor_v     = (EventPixel2D*)mgr.get_data(kProductPixel2D,_pcluster_ctor_prod);

    if(!_trk_img_prod.empty())
      _ev_trk_img_v = (EventImage2D*)mgr.get_data(kProductImage2D,_trk_img_prod);
    if(!_shr_img_prod.empty())
      _ev_shr_img_v = (EventImage2D*)mgr.get_data(kProductImage2D,_shr_img_prod);
    
    _run    = _ev_pgraph_v->run();
    _subrun = _ev_pgraph_v->subrun();
    _event  = _ev_pgraph_v->event();
    _entry  = mgr.current_entry();


    Analyze();
    
    return true;
  }

  void ExampleAna::finalize()
  {
    _tree->Write();
  }  

  void ExampleAna::Analyze() {
    
    auto const& ctor_m = _ev_ctor_v->Pixel2DClusterArray();
    auto const& pcluster_m = _ev_pcluster_v->Pixel2DClusterArray();

    std::vector<ImageMeta> all_meta_v;
    std::vector<cv::Mat>   adc_cvimg_v;
    std::vector<cv::Mat>   trk_cvimg_v;
    std::vector<cv::Mat>   shr_cvimg_v;
    
    for(auto const& pgraph : _ev_pgraph_v->PGraphArray()) {
      auto const& roi_v = pgraph.ParticleArray();

      for(size_t plane=0; plane<3; ++plane) {
	auto meta = roi_v.front().BB(plane);
      
	auto meta_iter = std::find(std::begin(all_meta_v),std::end(all_meta_v),meta);
	if ( meta_iter != all_meta_v.end()) continue;
	
	auto adc_img2d = _ev_img_v->Image2DArray().at(plane).crop(meta);
	auto trk_img2d = _ev_trk_img_v->Image2DArray().at(plane).crop(meta);
	auto shr_img2d = _ev_shr_img_v->Image2DArray().at(plane).crop(meta);

	auto adc_cvimg = _LArbysImageMaker.ExtractMat(adc_img2d);
	adc_cvimg = larocv::Transpose(adc_cvimg);
	adc_cvimg = larocv::Flip(adc_cvimg,0);
	
	auto trk_cvimg = _LArbysImageMaker.ExtractMat(trk_img2d);
	trk_cvimg = larocv::Transpose(trk_cvimg);
	trk_cvimg = larocv::Flip(trk_cvimg,0);

	auto shr_cvimg = _LArbysImageMaker.ExtractMat(shr_img2d);
	shr_cvimg = larocv::Transpose(shr_cvimg);
	shr_cvimg = larocv::Flip(shr_cvimg,0);

	all_meta_v.emplace_back(std::move(meta));
	adc_cvimg_v.emplace_back(std::move(adc_cvimg));
	trk_cvimg_v.emplace_back(std::move(shr_cvimg));
	shr_cvimg_v.emplace_back(std::move(trk_cvimg));
      }
    }
    

    for(size_t pgraph_id = 0; pgraph_id < _ev_pgraph_v->PGraphArray().size(); ++pgraph_id) {

      std::stringstream base_ss;

      base_ss << _entry << "_pgraph_id";

      std::string base_str = base_ss.str();
      
      LARCV_DEBUG() << "@pgraph=" << pgraph_id << std::endl;
      
      auto const& pgraph = _ev_pgraph_v->PGraphArray().at(pgraph_id);
      auto const& roi_v = pgraph.ParticleArray();
      auto const& cluster_idx_v = pgraph.ClusterIndexArray();

      // Get the 3D vertex position
      auto vtx_x = pgraph.ParticleArray().front().X();
      auto vtx_y = pgraph.ParticleArray().front().Y();
      auto vtx_z = pgraph.ParticleArray().front().Z();

      // Get the 3D end point
      auto end_x = pgraph.ParticleArray().front().EndPosition().X();
      auto end_y = pgraph.ParticleArray().front().EndPosition().Y();
      auto end_z = pgraph.ParticleArray().front().EndPosition().Z();

      LARCV_DEBUG() << "vertex=(" << vtx_x << "," << vtx_y << "," << vtx_z << ")" << std::endl;
      LARCV_DEBUG() << "end   =(" << end_x << "," << end_y << "," << end_z << ")" << std::endl;
	    
      auto nparticles = cluster_idx_v.size();
      
      // Loop per plane, get the particle contours and images for this plane
      for(size_t plane=0; plane<3; ++plane) {

	std::stringstream plane_ss;
	
	LARCV_DEBUG() << "@plane=" <<plane << std::endl;
	
	auto iter_pcluster = pcluster_m.find(plane);
	if(iter_pcluster == pcluster_m.end()) continue;
	
	auto iter_ctor = ctor_m.find(plane);
	if(iter_ctor == ctor_m.end()) continue;
	
	// Retrieve the particle images and particle contours on this plane
	const auto& pcluster_v = (*iter_pcluster).second;
	const auto& ctor_v = (*iter_ctor).second;

	// Get this planes meta
	auto meta = roi_v.front().BB(plane);
	
	auto meta_iter = std::find(std::begin(all_meta_v),std::end(all_meta_v),meta);
	if ( meta_iter == all_meta_v.end()) throw larbys("Where is this meta?");

	auto meta_id = meta_iter - all_meta_v.begin();
	auto& adc_cvimg = adc_cvimg_v.at(meta_id);
	auto& trk_cvimg = trk_cvimg_v.at(meta_id);
	auto& shr_cvimg = shr_cvimg_v.at(meta_id);

	double x_pixel,y_pixel;
	x_pixel = y_pixel = kINVALID_DOUBLE;
	
	Project3D(meta,
		  vtx_x,vtx_y,vtx_z,0, // (x,y,z,t)
		  plane,
		  x_pixel,y_pixel);    // result
	
	y_pixel = meta.rows() - y_pixel;

	auto adc_cvimg_bw = larocv::Threshold(adc_cvimg,10,255);
	
	plane_ss << base_str << "_plane_"<<plane<<"_adc.png";
	cv::imwrite(plane_ss.str().c_str(),adc_cvimg_bw);
	
	// For each particle, get the contour and image on this plane (from pcluster_v/ctor_v)
	for(size_t par_id=0; par_id < cluster_idx_v.size(); ++par_id) {
	  
	  auto cluster_idx = cluster_idx_v[par_id];

	  const auto& pcluster = pcluster_v.at(cluster_idx);
	  const auto& pctor    = ctor_v.at(cluster_idx);

	  // There is no particle cluster on this plane
	  if (pctor.empty()) continue;

	  // Convert the Pixel2DArray of contour points to a GEO2D_Contour_t
	  larocv::GEO2D_Contour_t ctor;
	  ctor.resize(pctor.size());
	  for(size_t i=0;i<ctor.size();++i) {
	    ctor[i].x = pctor[i].X();
	    ctor[i].y = pctor[i].Y();
	  }

	  


	  
	} // end this particle
      } // end this plane
      
      _tree->Fill();
    } // end this vertex

    return;
    
  }
  
}
#endif
