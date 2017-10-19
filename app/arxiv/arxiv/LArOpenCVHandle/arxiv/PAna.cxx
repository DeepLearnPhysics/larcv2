#ifndef TRACKSHOWERANA_CXX
#define TRACKSHOWERANA_CXX

#include "TrackShowerAna.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

namespace larcv {

  TrackShowerAna::TrackShowerAna() :
    _track_tree(nullptr),
    _shower_tree(nullptr)
  {
    LARCV_DEBUG() << "start" << std::endl;
    LARCV_DEBUG() << "end" << std::endl;
  }
  
  void
  TrackShowerAna::Configure(const PSet& pset) {
    LARCV_DEBUG() << "start" << std::endl;

    this->set_verbosity((msg::Level_t)pset.get<int>("Verbosity",2));

    _min_ctor_size = pset.get<uint>("MinContourSize",3);
    _min_points_inside = pset.get<uint>("MinPointsInside",10);
    
    std::string tree_prefix = pset.get<std::string>("TreePrefix");
    std::stringstream ss;
    
    ss.str("");
    ss << tree_prefix << "_track";
    _track_tree = new TTree(ss.str().c_str(),ss.str().c_str());    
    _track_tree->Branch("length"         , &_length         ,"length/F");
    _track_tree->Branch("width"          , &_width          ,"width/F");
    _track_tree->Branch("perimeter"      , &_perimeter      ,"perimeter/F");
    _track_tree->Branch("area"           , &_area           ,"area/F");
    _track_tree->Branch("mean_pix_dist"  , &_mean_pix_dist  ,"mean_pix_dist/F");
    _track_tree->Branch("sigma_pix_dist" , &_sigma_pix_dist ,"sigma_pix_dist/F");
    _track_tree->Branch("n_pixel"        , &_n_pixel        ,"n_pixel/F");
    _track_tree->Branch("weighted_angle" , &_weighted_angle ,"weighted_angle/F");

    ss.str("");
    ss << tree_prefix << "_shower";
    _shower_tree = new TTree(ss.str().c_str(),ss.str().c_str());
    _shower_tree->Branch("length"         , &_length         ,"length/F");
    _shower_tree->Branch("width"          , &_width          ,"width/F");
    _shower_tree->Branch("perimeter"      , &_perimeter      ,"perimeter/F");
    _shower_tree->Branch("area"           , &_area           ,"area/F");
    _shower_tree->Branch("mean_pix_dist"  , &_mean_pix_dist  ,"mean_pix_dist/F");
    _shower_tree->Branch("sigma_pix_dist" , &_sigma_pix_dist ,"sigma_pix_dist/F");
    _shower_tree->Branch("n_pixel"        , &_n_pixel        ,"n_pixel/F");
    _shower_tree->Branch("weighted_angle" , &_weighted_angle ,"weighted_angle/F");
    
    LARCV_DEBUG() << "end" << std::endl;
    return;
  }

  void TrackShowerAna::Clear() {
    _length = 0.0;
    _width = 0.0;
    _perimeter = 0.0;
    _area = 0.0;
    _mean_pix_dist = 0.0;
    _sigma_pix_dist = 0.0;
    _n_pixel = 0.0;
    _weighted_angle = 0.0;
  }


  //duplication from PreProcessor
  void
  TrackShowerAna::FilterContours(larocv::GEO2D_ContourArray_t& ctor_v) {
    larocv::GEO2D_ContourArray_t ctor_tmp_v;
    ctor_tmp_v.reserve(ctor_v.size());
    
    for(auto& ctor : ctor_v)
      if(ctor.size()>_min_ctor_size)
	ctor_tmp_v.emplace_back(std::move(ctor));
    
    std::swap(ctor_tmp_v,ctor_v);
    return;
  }
  
  void
  TrackShowerAna::CalcAndFill(const larocv::GEO2D_ContourArray_t& ctor_v,
			      const cv::Mat& img,
			      TTree* tree) {
    LARCV_DEBUG() << "calculating for " << ctor_v.size() << " contours" << std::endl;
    
    for(const auto& ctor : ctor_v) {

      auto npixel = cv::countNonZero(larocv::MaskImage(img,ctor,0,false));
      
      if (npixel < _min_points_inside) continue;

      _n_pixel = npixel;

      //rotated rect coordinates      
      auto min_rect  = cv::minAreaRect(ctor);
      cv::Point2f vertices[4];
      min_rect.points(vertices);

      auto rect = min_rect.size;
      _length = rect.height > rect.width ? rect.height : rect.width;
      _width = rect.height > rect.width ? rect.width  : rect.height;
      _perimeter = cv::arcLength(ctor,1);
      _area = cv::contourArea(ctor);

      auto overallPCA = larocv::CalcPCA(ctor);
      auto masked_pts=larocv::MaskImage(img,ctor,0,false);
	
      _mean_pix_dist = larocv::MeanDistanceToLine(masked_pts,overallPCA);
      _sigma_pix_dist = larocv::SigmaDistanceToLine(masked_pts,overallPCA);
      _weighted_angle = larocv::CircumferenceAngularSum(ctor);
	
      tree->Fill();
      Clear();
    } // end this contour
    
  }
  
  bool
  TrackShowerAna::Analyze(const cv::Mat& adc_img,
			  const cv::Mat& track_img,
			  const cv::Mat& shower_img)
  {
    LARCV_DEBUG() << "start" << std::endl;
    auto track_img_t = larocv::Threshold(track_img,10,1);
    auto shower_img_t = larocv::Threshold(shower_img,10,1);
    
    // FindContours
    larocv::GEO2D_ContourArray_t track_ctor_v,shower_ctor_v;
    track_ctor_v = larocv::FindContours(track_img_t);
    shower_ctor_v = larocv::FindContours(shower_img_t);

    // Filter them by size (number of contour points)
    FilterContours(track_ctor_v);
    FilterContours(shower_ctor_v);

    // Calculate metrics
    CalcAndFill(track_ctor_v,track_img,_track_tree);
    CalcAndFill(shower_ctor_v,shower_img,_shower_tree);
    
    return true;
    LARCV_DEBUG() << "end" << std::endl;
  }
  
  void TrackShowerAna::Finalize(TFile* file) {
    LARCV_DEBUG() << "TS Finalize" << std::endl;
    if(file && _track_tree && _shower_tree) {
      file->cd();
      _track_tree->Write();
      _shower_tree->Write();
      LARCV_DEBUG() << "wrote" << std::endl;
    }
    LARCV_DEBUG() << "done" << std::endl;
  }
  
}

#endif
