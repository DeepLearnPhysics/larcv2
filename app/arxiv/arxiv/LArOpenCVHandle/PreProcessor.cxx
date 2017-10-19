#ifndef PREPROCESSOR_CXX
#define PREPROCESSOR_CXX

#include "PreProcessor.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "Geo2D/Core/Line.h"
#include <array>
#include <limits>

namespace larcv {

  PreProcessor::PreProcessor()
  {
    LARCV_DEBUG() << "start" << std::endl;
    /// General...
    _pi_threshold=1;
    _min_ctor_size=4;
    _blur = 4; 
    _pca_box_size=5;
    _min_overall_angle=10;
    _min_pca_angle=10;

    /// Straightness
    _mean_distance_pca = 3;

    /// MergeTracklets
    _merge_tracklets=true;
    _min_merge_track_size = 40;
    _min_merge_track_shower_dist = 2;

    /// ClaimShowers
    _claim_showers=true;
    _max_track_in_shower_frac = 0.1;
    
    /// MergePixelByFraction
    _merge_pixel_frac = true;
    _min_track_frac = 0.8;
    _min_shower_frac = 0.8;
    _allowed_neighbor_dist = 10;
    _allowed_merge_neighbor_dist=10;
    _allowed_edge_overlap=20;
      
    /// MergeStraightShowers
    _merge_straight_showers=true;
      
    /// MergeEndNubs                                                                                                                                  
    _merge_end_nubs     = false;
    _min_track_pixels   = 100;
    _end_nub_max_pixels = 30;
	    
    LARCV_DEBUG() << "end" << std::endl;
  }

  void
  PreProcessor::Configure(const PSet& pset) {
    LARCV_DEBUG() << "start" << std::endl;

    this->set_verbosity((msg::Level_t)pset.get<int>("Verbosity"));
    
    _pi_threshold = pset.get<uint>("PiThreshold",1);
    _min_ctor_size = pset.get<uint>("MinContourSize",4);
    _allowed_neighbor_dist = pset.get<float>("AllowedNeighborSeparation",10);
    _blur = pset.get<uint>("BlurSize",4);
    _pca_box_size = pset.get<uint>("EdgePCABoxSize",5);
    _min_overall_angle = pset.get<float>("MinPCAOverallAngle",10);

    _merge_tracklets = pset.get<bool>("MergeTracklets",false);
    if(_merge_tracklets) {
      _min_merge_track_size = pset.get<uint>("MergeTrackletsSize");
      _min_merge_track_shower_dist = pset.get<uint>("MergeTrackletsDist");
    }
    
    _claim_showers         = pset.get<bool>("ClaimShowers",false);
    _merge_pixel_frac      = pset.get<bool>("MergePixelFrac",false);
    _merge_straight_showers= pset.get<bool>("MergeStraightShowers",false);
    
    _merge_end_nubs         = pset.get<bool>("MergeEndNubs",false);
    _min_track_pixels       = pset.get<uint>("MinTrackPixels",100);
    _end_nub_max_pixels     = pset.get<uint>("MaxEndNubPixels",0);
    
    LARCV_DEBUG() << "end" << std::endl;
    return;
  }

  bool
  PreProcessor::IsStraight(const larocv::PixelChunk& track,const cv::Mat& img) {
    
    auto e1e2_a = std::abs(geo2d::angle(track.edge1PCA,track.edge2PCA));
    auto e1oA_a = std::abs(geo2d::angle(track.edge1PCA,track.overallPCA));
    auto e2oA_a = std::abs(geo2d::angle(track.edge2PCA,track.overallPCA));
    
    // auto oe1PCA_a  = std::abs(geo2d::angle(track.overallPCA,track.edge1PCA));
    bool e1e2_b = false;
    bool e1oA_b = false;
    bool e2oA_b = false;
    bool pca_b = false;
    
    if ((e1e2_a < _min_pca_angle) or (e1e2_a > 180-_min_pca_angle)) // probably straight
      e1e2_b=true;
    if ((e1oA_a < _min_pca_angle) or (e1oA_a > 180-_min_pca_angle)) // probably straight
      e1oA_b=true;
    if ((e2oA_a < _min_pca_angle) or (e2oA_a > 180-_min_pca_angle)) // probably straight
      e2oA_b=true;
    
    if ( track.mean_pixel_dist < _mean_distance_pca)
      pca_b = true;
    
    return (pca_b and (e1e2_b or e1oA_b or e2oA_b));
  }
  
  float
  PreProcessor::GetClosestEdge(const larocv::PixelChunk& track1, const larocv::PixelChunk& track2,
			       geo2d::Vector<float>& edge1, geo2d::Vector<float>& edge2) {
    
    
    geo2d::Vector<float> e1,e2;
    float dist=std::numeric_limits<float>::max();
    
    auto dt1e1t2e1 = geo2d::dist(track1.edge1,track2.edge1);
    auto dt1e1t2e2 = geo2d::dist(track1.edge1,track2.edge2);
    auto dt1e2t2e1 = geo2d::dist(track1.edge2,track2.edge1);
    auto dt1e2t2e2 = geo2d::dist(track1.edge2,track2.edge2);

    if (dt1e1t2e1 < dist)
      { dist = dt1e1t2e1; edge1=track1.edge1; edge2=track2.edge1; }
    if (dt1e1t2e2 < dist)
      { dist = dt1e1t2e2; edge1=track1.edge1; edge2=track2.edge2; }
    if (dt1e2t2e1 < dist)
      { dist = dt1e2t2e1; edge1=track1.edge2; edge2=track2.edge1; }
    if (dt1e2t2e2 < dist)
      { dist = dt1e2t2e2; edge1=track1.edge2; edge2=track2.edge2; }
    
    return dist;
  }

  float
  PreProcessor::GetClosestEdge(const larocv::PixelChunk& track1, const larocv::PixelChunk& track2) {
    geo2d::Vector<float> temp1,temp2;
    return GetClosestEdge(track1,track2,temp1,temp2);
  }

  
  void
  PreProcessor::FilterContours(larocv::GEO2D_ContourArray_t& ctor_v) {
    larocv::GEO2D_ContourArray_t ctor_tmp_v;
    ctor_tmp_v.reserve(ctor_v.size());

    for(auto& ctor : ctor_v)
      if(ctor.size()>_min_ctor_size)
	ctor_tmp_v.emplace_back(std::move(ctor));
    
    std::swap(ctor_tmp_v,ctor_v);
    return;
  }

  std::vector<larocv::PixelChunk>
  PreProcessor::MakePixelChunks(const cv::Mat& img,
				larocv::ChunkType_t type,
				bool calc_params,
				size_t min_ctor_size,
				size_t min_track_size) {

    
    auto ctor_v = larocv::FindContours(img);

    std::vector<larocv::PixelChunk> track_v;
    track_v.reserve(ctor_v.size());

    LARCV_DEBUG() << "Found " << ctor_v.size() << " contours of type " << (uint) type  << std::endl;
      
    for(auto& ctor_ : ctor_v) {
      LARCV_DEBUG() << "... size " << ctor_.size() << std::endl;
      if (ctor_.size() < min_ctor_size) continue;

      larocv::PixelChunk pchunk;
      pchunk.npixel = cv::countNonZero(larocv::MaskImage(img,ctor_,0,false));
      pchunk.type = type;
      pchunk.ctor = std::move(ctor_);
      auto& ctor = pchunk.ctor;
      
      if (pchunk.npixel < min_track_size) continue;
      
      if (calc_params) {

	// make the edge
	geo2d::Vector<float> edge1,edge2;
	larocv::FindEdges(ctor,edge1,edge2);

	//rotated rect coordinates
	auto min_rect  = cv::minAreaRect(ctor);
	cv::Point2f vertices[4];
	min_rect.points(vertices);

	pchunk.edge1 = edge1;
	pchunk.edge2 = edge2;

	//set parameters from rotated rect
	auto rect = min_rect.size;
	pchunk.length = rect.height > rect.width ? rect.height : rect.width;
	pchunk.width = rect.height > rect.width ? rect.width  : rect.height;
	pchunk.perimeter = cv::arcLength(ctor,1);
	pchunk.area = cv::contourArea(ctor);
	pchunk.overallPCA = larocv::CalcPCA(ctor);
	pchunk.edge1PCA = larocv::SquarePCA(img,edge1,_pca_box_size,_pca_box_size);
	pchunk.edge2PCA = larocv::SquarePCA(img,edge2,_pca_box_size,_pca_box_size);
	pchunk.track_frac = type==larocv::ChunkType_t::kTrack  ? 1 : 0;
	pchunk.shower_frac = type==larocv::ChunkType_t::kShower ? 1 : 0;
	auto masked_pts=larocv::MaskImage(img,ctor,0,false);
	pchunk.mean_pixel_dist = larocv::MeanDistanceToLine(masked_pts,pchunk.overallPCA);
	pchunk.sigma_pixel_dist = larocv::SigmaDistanceToLine(masked_pts,pchunk.overallPCA);
	pchunk.straight = IsStraight(pchunk,img);
      }
      
      //get the length, area, npixels
      track_v.emplace_back(std::move(pchunk));
    }

    return track_v;
  }


  bool PreProcessor::EdgeConnected(const larocv::PixelChunk& track1,
				   const larocv::PixelChunk& track2) {
    const auto& t1edge1 = track1.edge1;
    const auto& t1edge2 = track1.edge2;

    const auto& t2edge1 = track2.edge1;
    const auto& t2edge2 = track2.edge2;

    bool bt11t21,bt11t22,bt12t21,bt12t22;
    bt11t21=bt11t22=bt12t21=bt12t22=false;

    auto ft11t21 = geo2d::dist(t1edge1,t2edge1);
    auto ft11t22 = geo2d::dist(t1edge1,t2edge2);
    auto ft12t21 = geo2d::dist(t1edge2,t2edge1);
    auto ft12t22 = geo2d::dist(t1edge2,t2edge2);
    
    if(ft11t21 < _allowed_neighbor_dist) bt11t21=true;
    if(ft11t22 < _allowed_neighbor_dist) bt11t22=true;
    if(ft12t21 < _allowed_neighbor_dist) bt12t21=true;
    if(ft12t22 < _allowed_neighbor_dist) bt12t22=true;

    if (bt11t21)
      { LARCV_DEBUG() << "Track 1 edge1 & Track 2 edge1 neaby" << std::endl; return true; }
    if (bt11t22)
      { LARCV_DEBUG() << "Track 1 edge1 & Track 2 edge2 nearby" << std::endl; return true; }
    if (bt12t21)
      { LARCV_DEBUG() << "Track 1 edge2 & Track 2 edge1 nearby" << std::endl; return true; }
    if (bt12t22)
      { LARCV_DEBUG() << "Track 1 edge2 & Track 2 edge2 nearby" << std::endl; return true; }
    
    return false;
  }


  cv::Mat PreProcessor::PrepareImage(const cv::Mat& img) {
    // Copy input
    cv::Mat img_t = img.clone();

    if(this->logger().level() == msg::kDEBUG) {
      static uint call0=0;
      std::stringstream ss;
      ss << "img_" << call0 << ".png";
      cv::imwrite(ss.str(),img_t);
      call0+=1;
    }
    
    // Blur
    if (_blur) {
      cv::blur(img_t,img_t,::cv::Size(_blur,_blur) );
      if(this->logger().level() == msg::kDEBUG) {
	static uint call1=0;
	std::stringstream ss;
	ss << "img_t0_" << call1 << ".png";
	cv::imwrite(ss.str(),img_t);
	call1+=1;
      }
    }
    
    // Threshold
    img_t = larocv::Threshold(img_t,_pi_threshold,255);
    
    if(this->logger().level() == msg::kDEBUG) {
      static uint call2=0;
      std::stringstream ss;
      ss << "img_t1_" << call2 << ".png";
      cv::imwrite(ss.str(),img_t);
      call2+=1;
    }

    return img_t;
  }


  void
  PreProcessor::MergeTracklets(cv::Mat& track_img, cv::Mat& shower_img) {
    
    auto track_img_t = PrepareImage(track_img);
    auto shower_img_t = PrepareImage(shower_img);

    auto track_pchunk_v = MakePixelChunks(track_img_t,larocv::ChunkType_t::kTrack,false);
    auto shower_pchunk_v = MakePixelChunks(shower_img_t,larocv::ChunkType_t::kShower,false);
    
    LARCV_DEBUG() << "Track chunks " << track_pchunk_v.size()
		  << " & Shower chunks " << shower_pchunk_v.size() << std::endl;
    for(auto& track : track_pchunk_v) {
      LARCV_DEBUG() << "npixel ... " << track.npixel << std::endl;
      if (track.npixel > _min_merge_track_size) continue;
      for(auto& shower : shower_pchunk_v) {
	auto dist=larocv::Pt2PtDistance(track.ctor,shower.ctor);
	LARCV_DEBUG() << "shower npix " << shower.npixel << " @ dist " << dist << std::endl;
	if ( dist > _min_merge_track_shower_dist) continue;
	LARCV_DEBUG() << "masking!" << std::endl;
	uint tol=0;
	if (track.npixel<4) tol=1;
	auto mask_shower = larocv::MaskImage(track_img,track.ctor,tol,false);
	track_img = larocv::MaskImage(track_img,track.ctor,tol,true);
	shower_img = shower_img + mask_shower;
      }
    }
    
  }


  void
  PreProcessor::ClaimShowers(cv::Mat& adc_img,
			     cv::Mat& track_img,
			     cv::Mat& shower_img) {

    auto adc_img_t = PrepareImage(adc_img);
    auto track_img_t = PrepareImage(track_img);
    auto shower_img_t = PrepareImage(shower_img);
    
    auto track_pchunk_v = MakePixelChunks(track_img_t,larocv::ChunkType_t::kTrack,true);
    auto shower_pchunk_v = MakePixelChunks(shower_img_t,larocv::ChunkType_t::kShower,true);

    for(auto& shower_pchunk : shower_pchunk_v) {
      auto& shower_ctor = shower_pchunk.ctor;
      auto& track_frac = shower_pchunk.track_frac;
      for(const auto& track_pchunk : track_pchunk_v) {
	//this track not near the edge point...
	auto& track_ctor = track_pchunk.ctor;
	track_frac += larocv::PixelFraction(adc_img_t,shower_ctor,track_ctor);
      }
      if (track_frac < _max_track_in_shower_frac) {
	auto mask_shower = larocv::MaskImage(track_img,shower_ctor,0,false);
	track_img = larocv::MaskImage(track_img,shower_ctor,0,true);
	shower_img = shower_img + mask_shower;
      }
    }
    
  }

  void
  PreProcessor::MergePixelByFraction(cv::Mat& adc_img,
				     cv::Mat& track_img,
				     cv::Mat& shower_img) {

    auto adc_img_t = PrepareImage(adc_img);
    auto track_img_t = PrepareImage(track_img);
    auto shower_img_t = PrepareImage(shower_img);
    
    auto adc_pchunk_v = MakePixelChunks(adc_img_t,larocv::ChunkType_t::kUnknown,true);
    auto track_pchunk_v = MakePixelChunks(track_img_t,larocv::ChunkType_t::kTrack,true);
    auto shower_pchunk_v = MakePixelChunks(shower_img_t,larocv::ChunkType_t::kShower,true);
     
    /// determine track/shower fraction of ADC contours
    for(auto& adc_pchunk : adc_pchunk_v) {
      
      auto& adc_ctor = adc_pchunk.ctor;
      auto& track_frac = adc_pchunk.track_frac;
      auto& shower_frac = adc_pchunk.shower_frac;
      
      LARCV_DEBUG() << "ADC track @ " << &adc_pchunk << " contour size " << adc_ctor.size() << " w/ " << adc_pchunk.npixel << " pixels" << std::endl;
      LARCV_DEBUG() << "Edge 1 " << adc_pchunk.edge1 << " & Edge 2 " << adc_pchunk.edge2 << std::endl;
      const larocv::PixelChunk* track = nullptr;
      const larocv::PixelChunk* shower = nullptr;
      float largest_track_frac = std::numeric_limits<float>::min();
      float largest_shower_frac = std::numeric_limits<float>::min();

      bool track_at_edge=false;
      for(const auto& track_pchunk : track_pchunk_v) {
	auto frac = larocv::PixelFraction(adc_img_t,adc_ctor,track_pchunk.ctor);	
	if (frac > largest_track_frac) {
	  largest_track_frac = frac;
	  track = &track_pchunk;
	}
	track_frac += frac;
	auto dist = GetClosestEdge(track_pchunk,adc_pchunk);
	if (dist<_allowed_edge_overlap) track_at_edge=true;
      }

      bool shower_at_edge=false;      
      for(const auto& shower_pchunk : shower_pchunk_v) { 
	auto frac = larocv::PixelFraction(adc_img_t,adc_ctor,shower_pchunk.ctor);
	if (frac > largest_shower_frac) {
	  largest_shower_frac = frac;
	  shower = &shower_pchunk;
	}
	shower_frac += frac;
	auto dist = GetClosestEdge(shower_pchunk,adc_pchunk);
	if (dist<_allowed_edge_overlap) shower_at_edge=true;
      }
	
      if (!track or !shower) continue;

      geo2d::Vector<float> ed1,ed2;
      auto dist = GetClosestEdge(*track,*shower,ed1,ed2);
      LARCV_DEBUG() << "Distance is " << dist << " and track @ edge " << track_at_edge << " & shower @ edge " << shower_at_edge << std::endl;
      if (dist < _allowed_merge_neighbor_dist or (track_at_edge and shower_at_edge)) 
	adc_pchunk.ignore=true;
      
    }
    
    // loop over the ADC contours, claim contours above threshold as track and shower
    for(auto& adc_pchunk : adc_pchunk_v) {

      if (adc_pchunk.ignore) continue;
      
      auto& adc_ctor = adc_pchunk.ctor;
      LARCV_DEBUG() << "Merging adc contour of size " << adc_pchunk.npixel << std::endl;
      auto& track_frac = adc_pchunk.track_frac;
      auto& shower_frac = adc_pchunk.shower_frac;    
      LARCV_DEBUG() << "...with relative fractions (trk,shr)=("<<track_frac<<","<<shower_frac<<")"<<std::endl;
      if (track_frac > _min_track_frac) {
	LARCV_INFO() << "Claiming a mostly track ADC contour as track" << std::endl;
	auto mask_adc = larocv::MaskImage(adc_img,adc_ctor,0,false);
	auto mask_track = larocv::MaskImage(track_img,adc_ctor,0,true);
	track_img = mask_adc + mask_track;
	shower_img = larocv::MaskImage(shower_img,adc_ctor,0,true);
      }

      if (shower_frac > _min_shower_frac) {
	LARCV_INFO() << "Claiming a mostly shower ADC contour as shower" << std::endl;
	auto mask_adc = larocv::MaskImage(adc_img,adc_ctor,0,false);
	auto mask_shower = larocv::MaskImage(shower_img,adc_ctor,0,true);
	shower_img = mask_adc + mask_shower;
	track_img = larocv::MaskImage(track_img,adc_ctor,0,true);
      }
    }
  }

  bool
  PreProcessor::OverallStraightCompatible(const larocv::PixelChunk& pchunk1,
					  const larocv::PixelChunk& pchunk2) {
    auto angle = std::abs(geo2d::angle(pchunk1.overallPCA,pchunk2.overallPCA));
    if ((angle < _min_overall_angle) or (angle > 180 - _min_overall_angle))
      return true;
    return false;
  }
  
  void
  PreProcessor::MergeStraightShowers(cv::Mat& adc_img,
				     cv::Mat& track_img,
				     cv::Mat& shower_img) {

    auto adc_img_t = PrepareImage(adc_img);
    auto track_img_t = PrepareImage(track_img);
    auto shower_img_t = PrepareImage(shower_img);
        
    auto adc_pchunk_v = MakePixelChunks(adc_img_t,larocv::ChunkType_t::kUnknown);
    auto track_pchunk_v = MakePixelChunks(track_img_t,larocv::ChunkType_t::kTrack);
    auto shower_pchunk_v = MakePixelChunks(shower_img_t,larocv::ChunkType_t::kShower);

    std::vector<size_t> cidx_v;

    for(size_t shower_id=0;shower_id<shower_pchunk_v.size();++shower_id) {
      const auto& shower = shower_pchunk_v[shower_id];

      for(size_t track1_id=0;track1_id<track_pchunk_v.size();++track1_id) {
	const auto& track1 = track_pchunk_v[track1_id];

	//Determine the straightness of this track and this shower
	if (track1.straight and shower.straight) {
	  LARCV_DEBUG() << "Straight shower and straight track identified" << std::endl;
	  if (OverallStraightCompatible(shower,track1)) {
	    cidx_v.push_back(shower_id);
	    continue;
	  }
	}

	// are they connected by edge, if not move on
	if (!EdgeConnected(shower,track1))
	  continue;

	// Find "sandwich" showers -- showers between two tracks    
	for(size_t track2_id=track1_id+1;track2_id<track_pchunk_v.size();++track2_id) {
	  const auto& track2 = track_pchunk_v[track2_id];

	  // are they connected by edge, if not move on
	  if (!EdgeConnected(shower,track2))
	    continue;
	  
	  LARCV_DEBUG() << "Shower " << shower_id
			<< " sandwich btw"
			<< " track " << track1_id
			<< " & track " << track2_id
			<< std::endl;
	  
	  LARCV_DEBUG() << "This shower of size " << shower.npixel
			<< " e1 " << shower.edge1 << " e2 " << shower.edge2
			<< "mean dist " << shower.mean_pixel_dist << std::endl;
	  LARCV_DEBUG() << "Straight " << shower.straight << std::endl;

	  if (shower.straight)
	    cidx_v.push_back(shower_id);
	  
	} // end track2
      } // end track1
    } // end this shower
    
    LARCV_INFO() << "Found " << cidx_v.size() << " compatible tracks and showers" << std::endl;
    //lets use the shower to mask the ADC image, and append to the track image
    for(const auto& cidx : cidx_v) {
      const auto& shower_ctor = shower_pchunk_v[cidx].ctor;
      auto mask_adc = larocv::MaskImage(adc_img,shower_ctor,0,false);
      track_img += mask_adc;
      shower_img = larocv::MaskImage(shower_img,shower_ctor,0,true);
    }

  }
	
  void
  PreProcessor::MergeEndNubs(cv::Mat& adc_img,
                             cv::Mat& track_img,
                             cv::Mat& shower_img) {

    auto adc_img_t = PrepareImage(adc_img);
    auto track_img_t = PrepareImage(track_img);
    auto shower_img_t = PrepareImage(shower_img);

    auto adc_pchunk_v = MakePixelChunks(adc_img_t,larocv::ChunkType_t::kUnknown,false);
    auto track_pchunk_v = MakePixelChunks(track_img_t,larocv::ChunkType_t::kTrack,false);
    auto shower_pchunk_v = MakePixelChunks(shower_img_t,larocv::ChunkType_t::kShower,false);

    std::vector<size_t> cidx_v;

    for(size_t shower1_id=0;shower1_id<shower_pchunk_v.size();++shower1_id) {
      const auto& shower1 = shower_pchunk_v[shower1_id];

      for(size_t track1_id=0;track1_id<track_pchunk_v.size();++track1_id) {
        const auto& track1 = track_pchunk_v[track1_id];

        // Check if we're looking at a potential muon track, is it straight?                                                                          
        if (!track1.straight)
          continue;

        // Typically mistag involves a good sized muon track with proton nub, ignore small tracks                                                     
        if (track1.npixel < _min_track_pixels)
          continue;

        // Are the shower and track connected? If not continue                                                                                        
        if (!EdgeConnected(shower1,track1))
          continue;

        // Check to be sure that the attached shower pixels is a small cluster                                                                        
        if (shower1.npixel > _end_nub_max_pixels)
          continue;

        cidx_v.push_back(shower1_id);

      } // End loop over track clusters                                                                                                               
    } // End loop over shower clusters  
	  
    LARCV_INFO() << "Found " << cidx_v.size() << " shower end nubs on track clusters" << std::endl;
    // Using these shower to mask the ADC image. Add them to the track image                                                                          
    for(const auto& cidx : cidx_v) {
      const auto& shower_ctor = shower_pchunk_v[cidx].ctor;
      auto mask_adc = larocv::MaskImage(adc_img,shower_ctor,0,false);
      track_img += mask_adc;
      shower_img = larocv::MaskImage(shower_img,shower_ctor,0,true);
    }
  }
		
  bool
  PreProcessor::PreProcess(cv::Mat& adc_img, cv::Mat& track_img, cv::Mat& shower_img)
  {
    LARCV_DEBUG() << "start" << std::endl;

    LARCV_DEBUG() << "Merging tracklets" << std::endl;
    if (_merge_tracklets) MergeTracklets(track_img,shower_img);
    else LARCV_DEBUG() << "... not" << std::endl;
    
    LARCV_DEBUG() << "Cleaning up showers first" << std::endl;
    if (_claim_showers) ClaimShowers(adc_img,track_img,shower_img);
    else LARCV_DEBUG() << "... not" << std::endl;

    LARCV_DEBUG() << "Merge via pixel frac" << std::endl;
    if (_merge_pixel_frac) MergePixelByFraction(adc_img,track_img,shower_img);
    else LARCV_DEBUG() << "... not" << std::endl;
    
    LARCV_DEBUG() << "Merging straight showers to track" << std::endl;
    if(_merge_straight_showers) MergeStraightShowers(adc_img,track_img,shower_img);
    else LARCV_DEBUG() << "... not" << std::endl;
    
    LARCV_DEBUG() << "Merging end nubs to track" << std::endl;
    if(_merge_end_nubs) MergeEndNubs(adc_img,track_img,shower_img);
    else LARCV_DEBUG() << "... not" << std::endl;
	 	  
    // Lets make sure the track and shower images do not gain
    // extra pixel value from masking
    adc_img.copyTo(track_img,larocv::Threshold(track_img,1,1));
    adc_img.copyTo(shower_img,larocv::Threshold(shower_img,1,1));

    
    LARCV_DEBUG() << "end" << std::endl;    
    return true;
  }
}

#endif
