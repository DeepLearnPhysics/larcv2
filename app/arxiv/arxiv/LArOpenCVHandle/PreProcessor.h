#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "Base/larcv_base.h"
#ifndef __CLING__
#ifndef __CINT__
#include <opencv2/opencv.hpp>
#endif
#endif
#include "Base/PSet.h"
#include "LArOpenCV/ImageCluster/AlgoClass/PixelChunk.h"


namespace larcv {
  
  class PreProcessor : public larcv_base {
    
  public:
    
    PreProcessor();
    ~PreProcessor(){}
    
    bool
    PreProcess(cv::Mat& adc_img,
	       cv::Mat& track_img,
	       cv::Mat& shower_img);
    
    void
    Configure(const PSet& pset);

    void
    MergeTracklets(cv::Mat& track_img, cv::Mat& shower_img);

    void
    ClaimShowers(cv::Mat& adc_img,
		 cv::Mat& track_img,
		 cv::Mat& shower_img);

    void
    MergePixelByFraction(cv::Mat& adc_img,
			 cv::Mat& track_img,
			 cv::Mat& shower_img);

    void
    MergeStraightShowers(cv::Mat& adc_img,
			 cv::Mat& track_img,
			 cv::Mat& shower_img);
   
    void
    MergeEndNubs(cv::Mat& adc_img,
                 cv::Mat& track_img,
                 cv::Mat& shower_img);
  
  private:
    bool
    OverallStraightCompatible(const larocv::PixelChunk& pchunk1,
			      const larocv::PixelChunk& pchunk2);
    
    bool
    IsStraight(const larocv::PixelChunk& track,
	       const cv::Mat& img);
    
    void
    FilterContours(larocv::GEO2D_ContourArray_t& ctor_v);
		   
    std::vector<larocv::PixelChunk>
    MakePixelChunks(const cv::Mat& img,
		    larocv::ChunkType_t type,
		    bool calc_params=true,
		    size_t min_ctor_size=0,
		    size_t min_track_size=0);
    bool
    EdgeConnected(const larocv::PixelChunk& track1,
		  const larocv::PixelChunk& track2);

    cv::Mat
    PrepareImage(const cv::Mat& img);

    float
    GetClosestEdge(const larocv::PixelChunk& track1, const larocv::PixelChunk& track2,
		   geo2d::Vector<float>& edge1, geo2d::Vector<float>& edge2);
    float
    GetClosestEdge(const larocv::PixelChunk& track1, const larocv::PixelChunk& track2);

    
  private:
    
    uint _pi_threshold;
    uint _min_ctor_size;
    uint _blur;
    float _allowed_neighbor_dist;
    uint _pca_box_size;
    float _min_overall_angle;
    float _min_pca_angle;
    float _min_track_size;
    bool _merge_tracklets;
    bool _merge_pixel_frac;
    bool _claim_showers;
    float _min_track_frac;
    float _min_shower_frac;
    float _max_track_in_shower_frac;
    double _mean_distance_pca;
    uint _min_merge_track_size;
    bool _merge_straight_showers;
    uint _min_merge_track_shower_dist;
    double _allowed_merge_neighbor_dist;
    double _allowed_edge_overlap;
    bool _merge_end_nubs;
    uint _end_nub_max_pixels;
    uint _min_track_pixels;
  
  };
}
#endif


