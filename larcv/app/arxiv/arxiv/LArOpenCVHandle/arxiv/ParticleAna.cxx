#ifndef __PARTICLEANA_CXX__
#define __PARTICLEANA_CXX__

#include "ParticleAna.h"
#include "LArOpenCV/ImageCluster/AlgoClass/PixelChunk.h"
#include "LArbysUtils.h"
#include <numeric>
#include <unordered_map>

#define PI 3.14159265

namespace larcv {

  static ParticleAnaProcessFactory __global_ParticleAnaProcessFactory__;

  ParticleAna::ParticleAna(const std::string name)
    : ProcessBase(name), _LArbysImageMaker()
  {
    _signal_tree   = nullptr;
    _particle_tree = nullptr;
    _angle_tree    = nullptr;
    _dqdx_tree     = nullptr;

    _ev_img_v      = nullptr;
    _ev_trk_img_v  = nullptr;
    _ev_shr_img_v  = nullptr;
    _ev_roi_v      = nullptr;
    _ev_pgraph_v   = nullptr;
    _ev_pcluster_v = nullptr;
    _ev_ctor_v     = nullptr;
  }
  
  void ParticleAna::configure(const PSet& cfg)
  {

    _analyze_signal = cfg.get<bool>("AnalyzeSignal",false);
    if (_analyze_signal) {}
    
    _analyze_particle = cfg.get<bool>("AnalyzeParticle",false);
    if(_analyze_particle) { }

    _analyze_dqdx     = cfg.get<bool>("AnalyzedQdX",false);
    if(_analyze_dqdx) {
      _maskradius     = cfg.get<double>("MaskCircleRadius");
      _bins           = cfg.get<int>("dQdxBins");
      _adc_threshold  = cfg.get<float>("ADCThreshold");
    }
    
    _analyze_angle    = cfg.get<bool>("AnalyzeAngle",false);
    if(_analyze_angle) {
      _open_angle_cut = cfg.get<float>("OpenAngleCut");
      _pradius        = cfg.get<double>("PclusterRadius");
    }

    _LArbysImageMaker.Configure(cfg.get<larcv::PSet>("LArbysImageMaker"));
    
    _img_prod           = cfg.get<std::string>("ImageProducer"       ,"tpc");
    _pgraph_prod        = cfg.get<std::string>("PGraphProducer"      ,"test");
    _pcluster_ctor_prod = cfg.get<std::string>("Pixel2DCtorProducer" ,"test_ctor");
    _pcluster_img_prod  = cfg.get<std::string>("Pixel2DImageProducer","test_img");
    _reco_roi_prod      = cfg.get<std::string>("RecoROIProducer"     ,"croimerge");
    
    _trk_img_prod = cfg.get<std::string>("TrackImageProducer" ,"");
    _shr_img_prod = cfg.get<std::string>("ShowerImageProducer","");
    
  }

  void ParticleAna::initialize()
  {

    //
    // Event Tree (per vertex)
    //
    _signal_tree = new TTree("SignalTree","SignalTree");
    _signal_tree->Branch("entry",&_entry,"entry/I");
    _signal_tree->Branch("run",&_run,"run/I");
    _signal_tree->Branch("subrun",&_subrun,"subrun/I");
    _signal_tree->Branch("event",&_event,"event/I");
    _signal_tree->Branch("vtxid",&_vtx_id,"vtxid/I");
    _signal_tree->Branch("vtx_x",&_vtx_x,"vtx_x/F");
    _signal_tree->Branch("vtx_y",&_vtx_y,"vtx_y/F");
    _signal_tree->Branch("vtx_z",&_vtx_z,"vtx_z/F");

    _signal_tree->Branch("pathexists",&_pathexists,"pathexists/I");
    _signal_tree->Branch("pathexists2",&_pathexists2,"pathexists2/I");
    _signal_tree->Branch("pathexists_v",&_pathexists_v);
    _signal_tree->Branch("nparticles",&_nparticles,"nparticles/I");
    _signal_tree->Branch("track_frac_v",&_track_frac_v);
    _signal_tree->Branch("shower_frac_v",&_shower_frac_v);
    _signal_tree->Branch("infiducial",&_infiducial,"infiducial/I");
    _signal_tree->Branch("charge_neighbor2",&_charge_neighbor2,"charge_neighbor2/I");
    _signal_tree->Branch("charge_neighbor3",&_charge_neighbor3,"charge_neighbor3/I");
    _signal_tree->Branch("charge_neighbor_v",&_charge_neighbor_v);
    
    //
    // Particle Tree (per particle)
    //
    _particle_tree = new TTree("ParticleTree","ParticleTree");
    _particle_tree->Branch("entry"  , &_entry  ,"entry/I");
    _particle_tree->Branch("run"    , &_run    ,"run/I");
    _particle_tree->Branch("subrun" , &_subrun ,"subrun/I");
    _particle_tree->Branch("event"  , &_event  ,"event/I");
    _particle_tree->Branch("length"           , &_length          , "length/F");
    _particle_tree->Branch("width"            , &_width           , "width/F");
    _particle_tree->Branch("perimeter"        , &_perimeter       , "perimeter/F");
    _particle_tree->Branch("area"             , &_area            , "area/F");
    _particle_tree->Branch("npixel"           , &_npixel          , "npixel/i");
    _particle_tree->Branch("track_frac"       , &_track_frac      , "track_frac/F");
    _particle_tree->Branch("shower_frac"      , &_shower_frac     , "shower_frac/F");
    _particle_tree->Branch("mean_pixel_dist"  , &_mean_pixel_dist , "mean_pixel_dist/D");
    _particle_tree->Branch("sigma_pixel_dist" , &_sigma_pixel_dist, "sigma_pixel_dist/D");
    _particle_tree->Branch("angular_sum"      , &_angular_sum     , "angular_sum/D");
    _particle_tree->Branch("plane"            , &_plane           , "plane/I");    
 
    //
    // Angle Tree
    //
    _angle_tree = new TTree("AngleTree","AngleTree");
    _angle_tree->Branch("entry"  , &_entry  ,"entry/I");
    _angle_tree->Branch("run"    , &_run    ,"run/I");
    _angle_tree->Branch("subrun" , &_subrun ,"subrun/I");
    _angle_tree->Branch("event"  , &_event  ,"event/I");
    _angle_tree->Branch("vtxid"  , &_vtxid  ,"vtxid/I");
    _angle_tree->Branch("plane",&_plane,"plane/I");    
    _angle_tree->Branch("straight_lines",&_straight_lines,"straight_lines/I");
    _angle_tree->Branch("dir0_c",&_dir0_c);
    _angle_tree->Branch("dir1_c",&_dir1_c);
    _angle_tree->Branch("angle0_c",&_angle0_c);
    _angle_tree->Branch("angle1_c",&_angle1_c);
    _angle_tree->Branch("angle0_p",&_angle0_p);
    _angle_tree->Branch("angle1_p",&_angle1_p);
    _angle_tree->Branch("angle_diff_c",&_angle_diff_c,"angle_diff_c/D");
    _angle_tree->Branch("angle_diff_p",&_angle_diff_p,"angle_diff_p/D");
    _angle_tree->Branch("angle_diff_c_v",&_angle_diff_c_v);
    _angle_tree->Branch("angle_diff_p_v",&_angle_diff_p_v);
    _angle_tree->Branch("dir0_p",&_dir0_p);
    _angle_tree->Branch("dir1_p",&_dir1_p);
    _angle_tree->Branch("mean0c",&_mean0c);//position of cluste0 to 2d vtx
    _angle_tree->Branch("mean1c",&_mean1c);//position of cluste1 to 2d vtx
    _angle_tree->Branch("mean0p",&_mean0p);//position of cluste0 to 2d vtx
    _angle_tree->Branch("mean1p",&_mean1p);//position of cluste1 to 2d vtx
    _angle_tree->Branch("meanl",&_meanl,"meanl/F");//dqdx mean of particle left to the vtx
    _angle_tree->Branch("meanr",&_meanr,"meanr/F");//dqdx mean of particle right to the vtx
    _angle_tree->Branch("stdl",&_stdl,"stdl/F");
    _angle_tree->Branch("stdr",&_stdr,"stdr/F");

    _angle_tree->Branch("p0pxc",&_p0pxc);
    _angle_tree->Branch("p0pyc",&_p0pyc);
    _angle_tree->Branch("p1pxc",&_p1pxc);
    _angle_tree->Branch("p1pyc",&_p1pyc);
    _angle_tree->Branch("p0pxp",&_p0pxp);
    _angle_tree->Branch("p0pyp",&_p0pyp);
    _angle_tree->Branch("p1pxp",&_p1pxp);
    _angle_tree->Branch("p1pyp",&_p1pyp);
    _angle_tree->Branch("x2d",&_x2d_v);
    _angle_tree->Branch("y2d",&_y2d_v);
    _angle_tree->Branch("pct_0_c_v",&_pct_0_c_v);
    _angle_tree->Branch("pct_1_c_v",&_pct_1_c_v);
    _angle_tree->Branch("pct_0_p_v",&_pct_0_p_v);
    _angle_tree->Branch("pct_1_p_v",&_pct_1_p_v);
    //
    // dQdX Tree
    //
    _dqdx_tree = new TTree("dqdxTree","dqdxTree");
    _dqdx_tree->Branch("entry"  , &_entry  ,"entry/I");
    _dqdx_tree->Branch("run"    , &_run    ,"run/I");
    _dqdx_tree->Branch("subrun" , &_subrun ,"subrun/I");
    _dqdx_tree->Branch("event"  , &_event  ,"event/I");
    _dqdx_tree->Branch("dqdxdelta",&_dqdxdelta,"dqdxdelta/F");
    _dqdx_tree->Branch("dqdxratio",&_dqdxratio,"dqdxratio/F");
    

  }

  bool ParticleAna::process(IOManager& mgr)
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

    //std::cout<<"run "<<_run<<", subrun "<<_subrun<<", event"<<_event<<", entry"<<_entry<<std::endl;

    if(_analyze_signal)   AnalyzeSignal();
    if(_analyze_particle) AnalyzeParticle();
    if(_analyze_dqdx)     AnalyzedQdX();
    if(_analyze_angle)    AnalyzeAngle();

    return true;
  }

void ParticleAna::finalize()
  {
    if(_analyze_signal)   _signal_tree->Write();
    if(_analyze_particle) _particle_tree->Write();
    if(_analyze_dqdx)     _dqdx_tree->Write();
    if(_analyze_angle)    _angle_tree->Write();
  }  

  //
  // Signal Related Functionality (Vic reponsible)
  //
  void ParticleAna::AnalyzeSignal() {
    bool DEBUG = false;
    
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

      if (DEBUG) { if (pgraph_id!=1) { continue; } }
      
      auto const& pgraph = _ev_pgraph_v->PGraphArray().at(pgraph_id);

      _vtx_id = (int) pgraph_id;
      
      auto const& roi_v = pgraph.ParticleArray();

      // Get this 3D position
      _vtx_x = pgraph.ParticleArray().front().X();
      _vtx_y = pgraph.ParticleArray().front().Y();
      _vtx_z = pgraph.ParticleArray().front().Z();

      _infiducial=1;
      if( _vtx_x < 5.     || _vtx_x > 251.35 ||
	  _vtx_y < -111.5 || _vtx_y > 111.5  ||
	  _vtx_z < 5.     || _vtx_z > 1031.8 )
	_infiducial = 0;
      

      auto const& cluster_idx_v = pgraph.ClusterIndexArray();
      _nparticles = (int) cluster_idx_v.size();

      _track_frac_v.clear();
      _track_frac_v.resize(_nparticles,0.0);

      _shower_frac_v.clear();
      _shower_frac_v.resize(_nparticles,0.0);

      _pathexists = 0;
      _pathexists_v.clear();
      _pathexists_v.resize(_nparticles,1.0);

      _charge_neighbor_v.clear();
      _charge_neighbor_v.resize(3,0);
      
      // Particle wise check if path exists
      std::vector<std::vector<int> > particle_path_exists_vv;
      particle_path_exists_vv.resize(_nparticles);
      for(auto& v : particle_path_exists_vv) v.resize(3,kINVALID_INT);
      // Plane wise check if path exists
      std::vector<int> plane_path_exists_v(3,kINVALID_INT);
      
      // Loop per plane, get the particle contours and images for this plane
      for(size_t plane=0; plane<3; ++plane) {
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
	x_pixel=y_pixel=kINVALID_DOUBLE;

	// Check if gap exists
	Project3D(meta,_vtx_x,_vtx_y,_vtx_z,0,plane,x_pixel,y_pixel);
	y_pixel = meta.rows() - y_pixel;
	geo2d::Circle<float> circle;
	circle.center.x = x_pixel;
	circle.center.y = y_pixel;
	circle.radius = 6;
	
	auto thresh_img = larocv::Threshold(adc_cvimg,10,255);
	if(larocv::CountNonZero(thresh_img,geo2d::Circle<float>(circle.center,3)))
	  _charge_neighbor_v[plane] = 1;
	auto xs_v = larocv::QPointOnCircle(thresh_img,circle,10,0);
	if (xs_v.empty()) continue;

	auto mask_img = larocv::MaskImage(thresh_img,circle,0,false);
	auto mask_ctor_v = larocv::FindContours(mask_img);

	std::vector<size_t> parent_ctor_id_v(xs_v.size(),kINVALID_SIZE);

	for(size_t xs_id=0; xs_id < xs_v.size(); ++xs_id) {
	  auto parent_id = larocv::FindContainingContour(mask_ctor_v,xs_v[xs_id]);
	  parent_ctor_id_v[xs_id] = parent_id;
	  if(DEBUG) {
	    std::cout << "this id " << xs_id
		      << " @ (" << xs_v[xs_id].x << "," << xs_v[xs_id].y << ")"
		      << " parent is " << parent_id << std::endl;
	  }
	}


	auto this_ctor_id = parent_ctor_id_v.front();

	if(DEBUG) {
	  std::cout << "this ctor id " << this_ctor_id << std::endl;
	}
	
	bool different = false;
	for(auto parent_ctor_id : parent_ctor_id_v) {
	  if (parent_ctor_id == kINVALID_SIZE) continue;
	  if(DEBUG) std::cout << "compare " << this_ctor_id << " w " << parent_ctor_id << std::endl;
	  if (parent_ctor_id != this_ctor_id) {
	    if(DEBUG) std::cout << "...different" << std::endl;
	    different = true;
	    break;
	  }
	}

	if (xs_v.size()==1) {
	  different=true;
	}

	if (different) {
	  plane_path_exists_v[plane] = 0;
	}
	else {
	  plane_path_exists_v[plane] = 1;
	}
	   
	// For each particle, get the contour and image on this plane (from pcluster_v/ctor_v)
	for(size_t par_id=0; par_id < cluster_idx_v.size(); ++par_id) {
	  
	  auto cluster_idx = cluster_idx_v[par_id];
	  auto& trk_frac = _track_frac_v[par_id];
	  auto& shr_frac = _shower_frac_v[par_id];
	  auto& particle_path_ex  = particle_path_exists_vv[par_id][plane];
	  
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
	  
	  // Make a PixelChunk given this contour, and this adc image
	  larocv::PixelChunk pchunk(ctor,adc_cvimg,trk_cvimg,shr_cvimg);
	  trk_frac += pchunk.track_frac;
	  shr_frac += pchunk.shower_frac;
	  
	  double min_dist = kINVALID_DOUBLE;
	  geo2d::Vector<float> min_pt;
	  
	  for(const auto& xs : xs_v) {
	    double dist = kINVALID_DOUBLE;
	    auto inside = larocv::PointPolygonTest(ctor,xs,dist);
	    if (inside) {
	      min_dist = 0;
	      min_pt = xs;
	      break;
	    }
	    if (dist < min_dist) {
	      min_dist = dist;
	      min_pt = xs;
	    }
	  }

	  if (xs_v.size()==1) {
	    particle_path_ex = 0;
	  } else {
	    particle_path_ex = (int)larocv::PathExists(mask_img,circle.center,min_pt,5,10,1);
	  }	  
	  
	  if (DEBUG) {
	    static int iii=0;
	    std::stringstream ss;
	    ss << "adc_img_" << iii << ".png";
	    cv::imwrite(ss.str(),larocv::Threshold(adc_cvimg,10,255));
	    ss.str("");
	    ss << "trk_img_" << iii << ".png";
	    cv::imwrite(ss.str(),larocv::Threshold(trk_cvimg,10,255));
	    ss.str("");
	    ss << "shr_img_" << iii << ".png";
	    cv::imwrite(ss.str(),larocv::Threshold(shr_cvimg,10,255));
	    ss.str("");
	    ss << "mask_img_" << iii << ".png";
	    cv::imwrite(ss.str(),larocv::Threshold(mask_img,10,255));
	    std::cout << "number of xs: " << xs_v.size() << std::endl;
	    std::cout << iii << ") @ plane " << plane << " particle id " << par_id << " is path: " << particle_path_ex << std::endl;
	    std::cout << "2D vtx @ (" << x_pixel << "," << y_pixel << ")" << " & min pt (" << min_pt.x << "," << min_pt.y << ")" << std::endl;
	    std::cout << std::endl;
	    iii+=1;
	  }

	} // end this particle
      } // end this plane

      //
      // Per plane check of charge proximity
      //
      _charge_neighbor2 = 0;
      _charge_neighbor3 = 0;
      
      int cn_sum = 0;
      for(auto v : _charge_neighbor_v) cn_sum += v;
      if (cn_sum == 2) _charge_neighbor2 = 1;
      if (cn_sum == 3) _charge_neighbor3 = 1;
      
      //
      // Per plane check of gap
      //
      int plane_path_exists = 0;
      int valid_plane_path_exists = 0;
      for(size_t plane=0; plane<3; ++plane) {
	if (DEBUG) std::cout << plane << ") -- " << plane_path_exists_v[plane] << std::endl;
	if (plane_path_exists_v[plane] == kINVALID_INT) continue;
	plane_path_exists += plane_path_exists_v[plane];
	valid_plane_path_exists += 1;
      }

      if (DEBUG) std::cout << "valid plane path exists " << valid_plane_path_exists << " & plane path exists " << plane_path_exists << std::endl;
      _pathexists2 = 1;
      if ((valid_plane_path_exists == 2) and (plane_path_exists == 0)) {
	_pathexists2 = 0;
      }
      if ((valid_plane_path_exists == 3) and (plane_path_exists <= 1)) {
	_pathexists2 = 0;
      }
      if(DEBUG) std::cout << "pathexists2 " << _pathexists2 << std::endl;
      
      //
      // Per particle check of gap
      //
      for(size_t par_id = 0; par_id < particle_path_exists_vv.size(); ++par_id) {
      
	const auto path_exists_v = particle_path_exists_vv[par_id];
	int path_exists = 0;
	int valid_planes = 0;
	
	// per plane
	for(auto path_plane : path_exists_v) {
	  if (path_plane == kINVALID_INT) continue;
	  valid_planes += 1;
	  path_exists  += path_plane;
	}
	if(DEBUG)std::cout << "valid planes: " << valid_planes << " path exists " << path_exists << " on par " << par_id << std::endl;
	if (valid_planes == 2 && path_exists == 0 ) {
	  _pathexists_v[par_id] = 0.0;
	}

	if (valid_planes == 3 && path_exists <= 1 ) {
	  _pathexists_v[par_id] = 0.0;
	}
	
      }

      _pathexists = 1.0;
	
      for(auto pe : _pathexists_v) {
	if (pe == 0.0)  {
	  _pathexists = 0.0;
	  break;
	}
      }
      if(DEBUG)std::cout << "so _pathexists " << _pathexists << std::endl;
      
      _signal_tree->Fill();
    } // end this vertex

    return;
    
  }
  
  //
  // Particle Related Functionality (Vic reponsible)
  //
  void ParticleAna::AnalyzeParticle() {
    
    // Get particle contours and images
    auto const& ctor_m = _ev_ctor_v->Pixel2DClusterArray();
    auto const& pcluster_m = _ev_pcluster_v->Pixel2DClusterArray();
      
    // Iterate over reconstructed vertex (1 vertex == 1 ParticleGraph)
    for(auto const& pgraph : _ev_pgraph_v->PGraphArray()) {

      // Get a list of particles (each unique particle == 1 ROI)
      auto const& roi_v = pgraph.ParticleArray();

      // Peel off the first ROI to get the reconstructed vertex (the particle start point)
      // Each reconstructed PGraph is part of the same CROI, so vic expects
      // the meta to be the same for each ROI, lets get it out and store in vector
      std::vector<ImageMeta> meta_v;
      meta_v.resize(3);
      for(size_t plane=0; plane<3; ++plane) 
	meta_v[plane] = roi_v.front().BB(plane);
      
      // Now go retrieve the particle contours, and particle pixels
      // ...the indicies are stored in ClusterIndexArray
      auto const& cluster_idx_v = pgraph.ClusterIndexArray();
      
      // Loop per plane, get the particle contours and images for this plane
      for(size_t plane=0; plane<3; ++plane) {

	auto iter_pcluster = pcluster_m.find(plane);
	if(iter_pcluster == pcluster_m.end()) {
	  LARCV_DEBUG() << "No particle cluster found" << std::endl;
	  continue;
	}
	
	auto iter_ctor = ctor_m.find(plane);
	if(iter_ctor == ctor_m.end()) {
	  LARCV_DEBUG() << "No contour found" << std::endl;
	  continue;
	}

	// Retrieve the particle images and particle contours on this plane
	const auto& pcluster_v = (*iter_pcluster).second;
	const auto& ctor_v = (*iter_ctor).second;

	// Ges this planes meta
	const auto& meta = meta_v.at(plane);

	// Get this plane image2d, crop it to this ROI using the meta
	auto adc_img2d = _ev_img_v->Image2DArray().at(plane).crop(meta);

	// Get the cv::Mat for this image (in the same style as LArbysImage)
	// we have to transpose and flip it along the rows to match Image2D orientation
	auto adc_cvimg = _LArbysImageMaker.ExtractMat(adc_img2d);
	adc_cvimg = larocv::Transpose(adc_cvimg);
	adc_cvimg = larocv::Flip(adc_cvimg,0);

	// For each particle, get the contour and image on this plane (from pcluster_v/ctor_v)
	for(auto cluster_idx : cluster_idx_v) {
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
	  
	  // Make a PixelChunk given this contour, and this adc image
	  larocv::PixelChunk pchunk(ctor,adc_cvimg);

	  _length           = pchunk.length;
	  _width            = pchunk.width;
	  _perimeter        = pchunk.perimeter;
	  _area             = pchunk.area;
	  _npixel           = pchunk.npixel;
	  _mean_pixel_dist  = pchunk.mean_pixel_dist;
	  _sigma_pixel_dist = pchunk.sigma_pixel_dist;
	  _angular_sum      = pchunk.angular_sum;
	  _plane            = plane;

	  // To be filled if shower image is sent in
	  // _track_frac = n/a
	  // _shower_frac = n/a

	  // Write it out per particles
	  _particle_tree->Fill();
	} // end this particle
      } // end this plane    
    } // end this vertex

    return;
  }

  //
  // Angle Related Functionality (Rui reponsible)
  //
  void ParticleAna::AnalyzeAngle() {
    bool DEBUG = false;
    
    if(DEBUG) std::cout<<"==========================>>>>>>>AngleANA Start<<<<<<<<================================="<<std::endl;
        
    auto const& ctor_m     = _ev_ctor_v->Pixel2DClusterArray();
    auto const& pcluster_m = _ev_pcluster_v->Pixel2DClusterArray();
    
    // Iterate over vertex
    
    auto vtx_counts = _ev_pgraph_v->PGraphArray().size();
    
    if(vtx_counts!=0) {
      
      for (int vtx_idx = 0; vtx_idx < vtx_counts; ++ vtx_idx){
	_vtxid = vtx_idx;
	
	_dir0_c.clear();
	_dir0_c.resize(3,-99999);
	_dir1_c.clear();
	_dir1_c.resize(3,-99999);
	_dir0_p.clear();
	_dir0_p.resize(3,-99999);
	_dir1_p.clear();
	_dir1_p.resize(3,-99999);
	_angle0_c.clear();
	_angle0_c.resize(3,-11111);
	_angle1_c.clear();
	_angle1_c.resize(3,-99999);
	_angle0_p.clear();
	_angle0_p.resize(3,-11111);
	_angle1_p.clear();
	_angle1_p.resize(3,-99999);
	_mean0c.clear();
	_mean0c.resize(3);
	_mean1c.clear();
	_mean1c.resize(3);
	_mean0p.clear();
	_mean0p.resize(3);
	_mean1p.clear();
	_mean1p.resize(3);
	
	_x2d_v.clear();
	_x2d_v.resize(3);
	_y2d_v.clear();
	_y2d_v.resize(3);
    
	_straight_lines = 0;
	
	//For plot
	_p0pxc.clear();
	_p0pxc.resize(3);
	_p0pyc.clear();
	_p0pyc.resize(3);
	_p1pxc.clear();
	_p1pxc.resize(3);
	_p1pyc.clear();
	_p1pyc.resize(3);
	
	_p0pxp.clear();
	_p0pxp.resize(3);
	_p0pyp.clear();
	_p0pyp.resize(3);
	_p1pxp.clear();
	_p1pxp.resize(3);
	_p1pyp.clear();
	_p1pyp.resize(3);
	
	_angle_diff_c_v.clear();
	_angle_diff_c_v.resize(3,-99999);
	_angle_diff_p_v.clear();
	_angle_diff_p_v.resize(3,-99999);

	_pct_0_c_v.clear();
	_pct_0_c_v.resize(3,0);
	_pct_1_c_v.clear();
	_pct_1_c_v.resize(3,0);
	_pct_0_p_v.clear();
	_pct_0_p_v.resize(3,0);
	_pct_1_p_v.clear();
	_pct_1_p_v.resize(3,0);
	
	auto pgraph = _ev_pgraph_v->PGraphArray().at(vtx_idx);
	//for(auto const& pgraph : _ev_pgraph_v->PGraphArray()) {
	
	// Get a list of particles (each unique particle == 1 ROI)
	auto const& roi_v = pgraph.ParticleArray();
	if(roi_v.size() < 2) continue;
	
	// Peel off the first ROI to get the reconstructed vertex (the particle start point)
	auto const& roi0 = roi_v.front();
	
	auto _x = roi0.X();
	auto _y = roi0.Y();
	auto _z = roi0.Z();
	
	// Now go retrieve the particle contours, and particles pixels
	// the indicies are stored in ClusterIndexArray
	auto const& cluster_idx_v = pgraph.ClusterIndexArray();
	// Particle 0
	auto const& cluster_idx0 = cluster_idx_v.at(0);
	// Particle 1
	auto const& cluster_idx1 = cluster_idx_v.at(1);
	
	// Playing with straight lines
	for(size_t plane=0; plane<3; ++plane) {
	  
	  auto iter_pcluster = pcluster_m.find(plane);
	  if(iter_pcluster == pcluster_m.end()) continue;
	  auto iter_ctor = ctor_m.find(plane);
	  if(iter_ctor == ctor_m.end()) continue;
	  
	  // Retrieve the contour
	  auto const& ctor_v = (*iter_ctor).second;
	  
	  // Projected the 3D start point onto this plane
	  double x_vtx2d(0), y_vtx2d(0);
	  Project3D(pgraph.ParticleArray().back().BB(plane), _x, _y, _z, 0, plane, x_vtx2d, y_vtx2d);
	  // Some shit related to axis rotation, check this by looking @ image in notebook
	  auto tmp = pgraph.ParticleArray().back().BB(plane).rows()-y_vtx2d;
	  y_vtx2d = tmp;
	
	  _x2d_v[plane] = x_vtx2d;
	  _y2d_v[plane] = y_vtx2d;
  
	  if(DEBUG) std::cout<<"vtx id is "<<vtx_idx<<"plane,"<<plane<<"  x is "<<x_vtx2d<<"y is "<<y_vtx2d<<std::endl;
	  
	  ///CCC
	  _p0pxc[plane].clear();
	  _p0pyc[plane].clear();
	  _p1pxc[plane].clear();
	  _p1pyc[plane].clear();
	  // Get the first particle contour
	  auto const& ctor0 = ctor_v.at(cluster_idx0);
	  
	  if(ctor0.size()>2) {
	    // Converting Pixel2D to geo2d::VectorArray<int>
	    ::larocv::GEO2D_Contour_t ctor;
	    ctor.resize(ctor0.size());
	    
	    for(size_t i=0; i<ctor0.size(); ++i) {
	      ctor[i].x = ctor0[i].X();
	      ctor[i].y = ctor0[i].Y();
	      _p0pxc[plane].emplace_back(ctor0[i].X());
	      _p0pyc[plane].emplace_back(ctor0[i].Y());
	    }
	    
	    // Get the mean position of the contour
	    auto mean = Getx2vtxmean(ctor, x_vtx2d, y_vtx2d, _pct_0_c_v[plane] );
	    _mean0c[plane] = mean;
	    if(DEBUG) std::cout<<"mean c is "<<mean<<std::endl;
	    
	    // Calculate the direction of particle 0
	    auto dir0_c = larocv::CalcPCA(ctor).dir;
	    if (dir0_c.x == 0 && dir0_c.y >0) _angle0_c[plane] = 90;
	    if (dir0_c.x == 0 && dir0_c.y <0) _angle0_c[plane] = 270;
	    if (dir0_c.y == 0 && dir0_c.x >0) _angle0_c[plane] = 0;
	    if (dir0_c.y == 0 && dir0_c.x <0) _angle0_c[plane] = 180;
	    if(dir0_c.x!=0 ) {
	      _dir0_c[plane] =  (dir0_c.y/dir0_c.x);
	      _angle0_c[plane] = atan(_dir0_c[plane])*180/PI;
	      if (_mean0c[plane] < 0) _angle0_c[plane] = atan(_dir0_c[plane])*180/PI + 180; 
	      LARCV_DEBUG()<<"plane "<<plane<<"  angle0 "<<_angle0_c[plane]<<"  plane "<<plane<<std::endl;
	    }
	  }

	  // Get the contour of particle 1
	  auto const& ctor1 = ctor_v.at(cluster_idx1);
	  if(ctor1.size()>2) {
	  
	    ::larocv::GEO2D_Contour_t ctor;
	    ctor.resize(ctor1.size());
	    for(size_t i=0; i<ctor1.size(); ++i) {
	      ctor[i].x = ctor1[i].X();
	      ctor[i].y = ctor1[i].Y();
	      _p1pxc[plane].emplace_back(ctor1[i].X());
	      _p1pyc[plane].emplace_back(ctor1[i].Y());
	    }

	    // Get the mean position of the contour
	    auto mean = Getx2vtxmean(ctor, x_vtx2d, y_vtx2d, _pct_1_c_v[plane] );
	    _mean1c[plane] = mean;
	  
	    // Calculate the direction of particle 1
	    auto dir1_c = larocv::CalcPCA(ctor).dir;
	    if (dir1_c.x == 0 && dir1_c.y >0) _angle1_c[plane] = 90;
	    if (dir1_c.x == 0 && dir1_c.y <0) _angle1_c[plane] = 270;
	    if (dir1_c.y == 0 && dir1_c.x >0) _angle1_c[plane] = 0;
	    if (dir1_c.y == 0 && dir1_c.x <0) _angle1_c[plane] = 180;
	    if(dir1_c.x!=0 ) {
	      _dir1_c[plane] =  (dir1_c.y/dir1_c.x);
	      _angle1_c[plane] = atan(_dir1_c[plane])*180/PI;
	      if (_mean1c[plane] < 0) _angle1_c[plane] = atan(_dir1_c[plane])*180/PI + 180; 
	      LARCV_DEBUG()<<"plane "<<plane<<"  angle1 "<<_angle1_c[plane]<<"  plane "<<plane<<std::endl;
	    }
	  }
	} // end plane loop
		
	if(DEBUG) std::cout<<"Angle CCC=========>>>>>>>>>>>>>>>>>>>>>>"<<std::endl;
	int ctr_17x_c = 0;
	int ctr_0x_c = 0;

	for (int plane =0 ; plane <=2 ; plane++){

	  _angle_diff_c_v[plane] = std::abs(_angle0_c[plane]-_angle1_c[plane]);
	  
	  if (_angle_diff_c_v[plane]>180) _angle_diff_c_v[plane] = 360 - _angle_diff_c_v[plane];

	  //if (_pct_0_c_v[plane] <0.1 ||_pct_1_c_v[plane] < 0.1) _angle_diff_c_v[plane] = 180 - _angle_diff_c_v[plane];
	  
	  if (_angle_diff_c_v[plane] > 170) ctr_17x_c++;
	  if (_angle_diff_c_v[plane] < 10 ) ctr_0x_c++;
	  
	  if (_angle0_c[plane]==-11111 || _angle1_c[plane]==-99999) _angle_diff_c_v[plane] = -77777;	  
	  if (_angle_diff_c_v[plane]!=-77777)_angle_diff_c = _angle_diff_c_v[plane];
	}
	
	if (ctr_17x_c >=2 ) _angle_diff_c = *std::max_element(std::begin(_angle_diff_c_v), std::end(_angle_diff_c_v)); // c++11
	/*int ctr_17x_c = 0;
	//int ctr_0x_c = 0;
	
	for (int plane = 0 ; plane <= 2 ; plane++){
	  if (_angle0_c[plane]!=-99999 && _angle1_c[plane]!=-99999){
	    _angle_diff_c_v[plane] = std::abs(_angle0_c[plane]-_angle1_c[plane]);
	    if (_angle_diff_c_v[plane] > 180) _angle_diff_c_v[plane] = 360 - _angle_diff_c_v[plane];
	    if (_angle_diff_c_v[plane] > 170) ctr_17x_c++;
	    if (_angle_diff_c_v[plane] < 10 ) ctr_0x_c++;
	    if(DEBUG) std::cout<<">>>>>>Selected angle0 "<<_angle0_c[plane]<<"angle1 "
			       <<_angle1_c[plane]<<" on  plane "<<plane
			       <<" diff is "<<_angle_diff_c_v[plane]<<std::endl;
	  }
	}
	
	if (ctr_17x_c >=1 ){
	  _angle_diff_c = *std::max_element(std::begin(_angle_diff_c_v), std::end(_angle_diff_c_v)); // c++11
	  }*/
 
	
	///Pcluster(Angle calculated using part of cluster close to the vertex)
	// Loop over planes again to calculate the direction using points close to vertex
	for(size_t plane=0; plane<3; ++plane) {
	  auto iter_pcluster = pcluster_m.find(plane);
	  if(iter_pcluster == pcluster_m.end()) continue;
	  
	  auto iter_ctor = ctor_m.find(plane);
	  if(iter_ctor == ctor_m.end()) continue;
	  
	  auto const& pcluster_v = (*iter_pcluster).second;
	  auto const& ctor_v = (*iter_ctor).second;
	  
	  double x_vtx2d(0), y_vtx2d(0);
	  Project3D(pgraph.ParticleArray().back().BB(plane), _x, _y, _z, 0, plane, x_vtx2d, y_vtx2d);
	  auto tmp = pgraph.ParticleArray().back().BB(plane).rows()-y_vtx2d;
	  y_vtx2d = tmp;

	  if(DEBUG) std::cout<<"vtxpp id is "<<vtx_idx<<"plane,"<<plane<<"  x is "<<x_vtx2d<<"y is "<<y_vtx2d<<std::endl;
	  
	  //////////PPP
	  _p0pxp[plane].clear();
	  _p0pyp[plane].clear();
	  _p1pxp[plane].clear();
	  _p1pyp[plane].clear();
	  
	  auto const& pcluster0 = pcluster_v.at(cluster_idx0);
	  auto const& ctor0 = ctor_v.at(cluster_idx0);
	  	  
	  if(ctor0.size()>2) {
	    ::larocv::GEO2D_Contour_t pclus;
	    pclus.clear();
	    //Get the pixels inside a radius
	    for(auto const& pt : pcluster0) {
	      geo2d::Vector<double> ppt(pt.X(),pt.Y());
	      double y = pt.X();//Vic will fix this
	      double x = pt.Y();
	  
	      
	      auto d = pow((pow(y-x_vtx2d,2)+pow(x-y_vtx2d,2)),0.5);
	      if (d < _pradius){
		pclus.emplace_back(x,y);
		_p0pxp[plane].emplace_back(x);
		_p0pyp[plane].emplace_back(y);
	      }
	    }

	    if (pclus.size()>=2) {
	      auto mean = Getx2vtxmean(pclus, x_vtx2d, y_vtx2d, _pct_0_p_v[plane]);//Need to swap x and y
	      _mean0p[plane] = mean;
	      auto dir0_p = larocv::CalcPCA(pclus).dir;
	      if (dir0_p.x == 0 && dir0_p.y >0) _angle0_p[plane] = 90;
	      if (dir0_p.x == 0 && dir0_p.y <0) _angle0_p[plane] = 270;
	      if (dir0_p.y == 0 && dir0_p.x >0) _angle0_p[plane] = 0;
	      if (dir0_p.y == 0 && dir0_p.x <0) _angle0_p[plane] = 180;
	      if(dir0_p.x!=0 ) {
		_dir0_p[plane] =  (dir0_p.y/dir0_p.x);
		_angle0_p[plane] = atan(_dir0_p[plane])*180/PI;
		if (_mean0p[plane] < 0) _angle0_p[plane] = atan(_dir0_p[plane])*180/PI + 180; 
	      }
	    }
	  }
	  auto const& pcluster1 = pcluster_v.at(cluster_idx1);
	  auto const& ctor1 = ctor_v.at(cluster_idx1);
	  
	  if(ctor1.size()>2){
	    ::larocv::GEO2D_Contour_t pclus;
	    pclus.clear();
	    //Get the pixels inside a radius
	    for(auto const& pt : pcluster1) {
	      geo2d::Vector<double> ppt(pt.X(),pt.Y());
	      double y = pt.X();//Vic, fix this!!!
	      double x = pt.Y();//Vic, fix this!!!
	      
	      //_p1pxc[plane].emplace_back(x);
	      //_p1pyc[plane].emplace_back(y);
	      
	      auto d = pow((pow(y-x_vtx2d,2)+pow(x-y_vtx2d,2)),0.5);
	      if (d < _pradius){ 
		_p1pxp[plane].emplace_back(x);
		_p1pyp[plane].emplace_back(y);
		pclus.emplace_back(x,y);}
	    }
	    
	    if (pclus.size()>=2) {
	      auto mean = Getx2vtxmean(pclus, x_vtx2d, y_vtx2d, _pct_1_p_v[plane]);//Need to swap x and y
	      _mean1p[plane] = mean;
	      auto dir1_p = larocv::CalcPCA(pclus).dir;
	      if (dir1_p.x == 0 && dir1_p.y >0) _angle1_p[plane] = 90;
	      if (dir1_p.x == 0 && dir1_p.y <0) _angle1_p[plane] = 270;
	      if (dir1_p.y == 0 && dir1_p.x >0) _angle1_p[plane] = 0;
	      if (dir1_p.y == 0 && dir1_p.x <0) _angle1_p[plane] = 180;
	      if(dir1_p.x!=0 ) {
		_dir1_p[plane] =  (dir1_p.y/dir1_p.x);
		_angle1_p[plane] = atan(_dir1_p[plane])*180/PI;
		if (_mean1p[plane] < 0) _angle1_p[plane] = atan(_dir1_p[plane])*180/PI + 180; 
		LARCV_DEBUG()<<"plane "<<plane<<"  angle1 "<<_angle1_p[plane]<<"  plane "<<plane<<std::endl;
	      }
	    }
	  }
	}

	int ctr_v = 0;
	int ctr_17x_p = 0;
	int ctr_0x_p = 0;
	
	if(DEBUG) std::cout<<"Angle PPP=========>>>>>>>>>>>>>>>>>>>>>>"<<std::endl;
	for (int plane =0 ; plane <=2 ; plane++){
	  _angle_diff_p_v[plane] = std::abs(_angle0_p[plane]-_angle1_p[plane]);
	  
	  if (_angle_diff_p_v[plane]>180) _angle_diff_p_v[plane] = 360 - _angle_diff_p_v[plane];
	  
	  //if (_pct_0_p_v[plane] < 0.1 || _pct_1_p_v[plane] < 0.1) _angle_diff_p_v[plane] = 180 - _angle_diff_p_v[plane];
	  
	  //std::cout<<"plane"<<plane<<std::endl;
	  //std::cout<<_pct_0_p_v[plane]<<std::endl;
	  //std::cout<<_pct_1_p_v[plane]<<std::endl;
	  if (_angle_diff_p_v[plane] > 170) ctr_17x_p++;
	  if (_angle_diff_p_v[plane] < 10 ) ctr_0x_p++;
	  
	  if (_angle0_p[plane]==-11111 || _angle1_p[plane]==-99999) _angle_diff_p_v[plane] = -77777;	  
	  if (_angle_diff_p_v[plane]!=-77777)_angle_diff_p = _angle_diff_p_v[plane];
	}
	
	if (ctr_17x_p >=2 ) _angle_diff_p = *std::max_element(std::begin(_angle_diff_p_v), std::end(_angle_diff_p_v)); // c++11
	_angle_tree->Fill(); 
      }
    }
    return;
  }
  
  //
  // dQdX Related Functionality (Rui responsible)
  //
  
  void ParticleAna::AnalyzedQdX() {

    auto const& ctor_m = _ev_ctor_v->Pixel2DClusterArray();
    auto const& pcluster_m = _ev_pcluster_v->Pixel2DClusterArray();

    // Loop over vertices
    for(auto const& pgraph : _ev_pgraph_v->PGraphArray()) {

      // Get the list of particles
      auto const& roi_v = pgraph.ParticleArray();

      // Are there not two particles? If not skip...
      if(roi_v.size()!=2) continue;
      auto const& roi0 = roi_v.front();

      auto _x = roi0.X();
      auto _y = roi0.Y();
      auto _z = roi0.Z();

      auto const& cluster_idx_v = pgraph.ClusterIndexArray();
      //auto const& cluster_idx0 = cluster_idx_v.at(0);
      //auto const& cluster_idx1 = cluster_idx_v.at(1);
      
      //dqdx
      bool save = false;
      LARCV_DEBUG()<<"=========================dqdx starts here==========================="<<std::endl;
      for(size_t plane=0; plane < 3; ++plane) {
	if (plane !=2 ) continue;
	
	auto iter_pcluster = pcluster_m.find(plane);
	if(iter_pcluster == pcluster_m.end()) continue;

	auto iter_ctor = ctor_m.find(plane);
	if(iter_ctor == ctor_m.end()) continue;
	
	double x_vtx2d(0), y_vtx2d(0);

	// Mask Circle is centered at 2D vertex
	Project3D(pgraph.ParticleArray().back().BB(plane), _x, _y, _z, 0, plane, y_vtx2d, x_vtx2d);
	
	LARCV_DEBUG()<<"plane "<<plane<<std::endl;
	LARCV_DEBUG()<<_x<<" , "<<_y<<" , "<<_z<<std::endl;
	LARCV_DEBUG()<<"x_vtx2d, "<<x_vtx2d<<"y_vtx2d, "<<y_vtx2d<<std::endl;
	
	LARCV_DEBUG()<<"mask circle x<<  "<< x_vtx2d<<" circle y  "<<y_vtx2d<<std::endl; 
	geo2d::Circle<float> mask_circle(x_vtx2d, y_vtx2d, _maskradius);

	// Get the meta for this ROI
	auto crop_img = pgraph.ParticleArray().back().BB(plane);

	// Get the cv::Mat for this ROI
	auto cv_img = _LArbysImageMaker.ExtractMat(_ev_img_v->Image2DArray()[plane].crop(crop_img));

	// Mask away stuff outside this circle
	auto img = larocv::MaskImage(cv_img,mask_circle,0,false);
	
	LARCV_DEBUG()<<"rows  "<< img.rows<<"cols "<<img.cols<<std::endl;
	
	LARCV_DEBUG()<<"<<<<<<<<<<<<Found more than 2 pixel in Maskimage>>>>>>>>>>>>"<<std::endl;

	// Found the number of points inside the image
	if ( larocv::FindNonZero(img).size() < 2 ) continue ;

	// Get the PCA
	auto pca = larocv::CalcPCA(img);
	
	// The last point on the masked track pt00
	::cv::Point pt00;
	
	for (size_t row = 0; row < img.rows; row++){
	  for (size_t col = 0; col < img.cols; col++){
	    float q = (float)(img.at<uchar>(row,col));
	    if ( q > _adc_threshold ) {
	      ::cv::Point pt;
	      pt.y = row;
	      pt.x = col;
	      pt00 = PointShift(pt, pca);
	      LARCV_DEBUG()<<"pt00row "<<row<<"pt00col "<<col<<std::endl;
	      break;
	    }
	  }
	}
	
	LARCV_DEBUG()<<"lastptx "<<pt00.x<<" lastpt0y "<<pt00.y<<std::endl;
	
	std::vector<float> dist_v;
	dist_v.clear();
	std::vector<float> dq_v;
	dq_v.clear();
	
	for (size_t row = 0; row < img.rows; ++row)
	  {
	    for (size_t col = 0; col < img.cols; ++col)
	      {
		float q = (float)(img.at<uchar>(row,col));
		if ( q > _adc_threshold ) 
		  {
		    ::cv::Point pt;
		    ::cv::Point spt; //shifted point
		    pt.y = row;
		    pt.x = col;
		    spt = PointShift(pt,pca);
		    float dist = sqrt(pow(float(spt.x)-float(pt00.x), 2)+pow(float(spt.y)-float(pt00.y),2));
		    dist_v.emplace_back(dist);
		    dq_v.emplace_back(q);
		  }
	      }
	  }

	if (dist_v.size() != dq_v.size())
	  {
	    LARCV_CRITICAL()<<"Length of dist and dQ are different"<<std::endl;
	    larbys();
	  }
	
	LARCV_DEBUG()<<"<<<<<<<<Dist_v has >0 size>>>>>>>>"<<std::endl;
	if ( dist_v.size() < 2 ) continue;
	
	auto max  = *std::max_element(std::begin(dist_v), std::end(dist_v));
	auto min  = *std::min_element(std::begin(dist_v), std::end(dist_v)); 
	
	auto bin_width=  ( max - min ) / _bins;
	
	LARCV_DEBUG()<<"dist_v size  "<<dist_v.size()<<std::endl;
	LARCV_DEBUG()<<"max dist_v  "<<max<<std::endl;
	LARCV_DEBUG()<<"min dist_v  "<<min<<std::endl;
	LARCV_DEBUG()<<"bin length  "<<bin_width<<std::endl;
	
	std::vector<float> dqdx_v; //binned dqdx
	dqdx_v.resize(_bins+1, 0.0);
	std::vector<float> dqdx_l; //binned pixels on the LEFT side of vtx
	std::vector<float> dqdx_r; //binned dqdx on the RIGHT side of  vtx
	dqdx_l.clear();
	dqdx_r.clear();
	
	for (size_t i = 0; i< dist_v.size();++i)
	  {
	    int idx =  int(dist_v[i] / bin_width) ; 
	    dqdx_v[idx] += dq_v[i];
	  }
	  
	float vtx_dist;
	vtx_dist = sqrt(pow(x_vtx2d-pt00.x, 2)+pow(y_vtx2d-pt00.y,2));
	int vtx_id = int(vtx_dist / bin_width);
	
	for (size_t i = 0; i< _bins; ++i){
	  if (i <= vtx_id) dqdx_l.emplace_back(dqdx_v[i]);
	  if (i > vtx_id)  dqdx_r.emplace_back(dqdx_v[i]);
	}
	
	_meanl = Mean(dqdx_l);
	_meanr = Mean(dqdx_r);
	_stdl  = STD(dqdx_l);
	_stdr  = STD(dqdx_r);
	
	_dqdxdelta = fabs(_meanl - _meanr) ;
	
	if ( _meanl >= _meanr && _meanl !=0 ) _dqdxratio = _meanr/_meanl;
	if ( _meanl <  _meanr && _meanr !=0 ) _dqdxratio = _meanl/_meanr;

	LARCV_DEBUG()<<"meanl "<<_meanl<<std::endl;
	LARCV_DEBUG()<<"meanr "<<_meanr<<std::endl;
	LARCV_DEBUG()<<"diff  "<<_dqdxdelta<<std::endl;
	
	save = true;
	//if (save) break;
      }//dqdx_end
      _dqdx_tree->Fill();
    } // end vertex
  }
 
  double ParticleAna::Getx2vtxmean( ::larocv::GEO2D_Contour_t ctor, float x2d, float y2d, double& pct)
  {
    double ctr_pos = 0.0;
    double ctr_neg = 0.0;
    double sum = 0;
    double mean = -999;
    for(size_t idx= 0;idx < ctor.size(); ++idx){
      sum += ctor[idx].x - x2d;
      if (ctor[idx].x - x2d > 0) ctr_pos++;
      if (ctor[idx].x - x2d < 0) ctr_neg++;
    }
    pct = std::abs(ctr_pos - ctr_neg)/ctor.size() ;
    if (ctor.size()>0) mean = sum / ctor.size();
    return mean;
  }

  //Project pts to PCA by shifting.
  cv::Point ParticleAna::PointShift(::cv::Point pt, geo2d::Line<float> pca)
  {
    float slope ;
    slope = pca.dir.y / pca.dir.x;
    if (slope >-1 && slope <=1 ) pt.y = pca.y(pt.x);
    else pt.x = pca.x(pt.y);
    
    return pt;
  }

  double ParticleAna::Mean(const std::vector<float>& v)
  {
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    double mean = sum / (double) v.size();
    
    return mean;
  }

  double ParticleAna::STD(const std::vector<float>& v)
  {
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    double mean = sum / (double) v.size();
    double sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
    double stdev = std::sqrt(sq_sum / (double) v.size() - mean * mean);
    
    return stdev;
  }
  
 }
#endif
