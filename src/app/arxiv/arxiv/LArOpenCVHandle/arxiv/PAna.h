#ifndef TRACKSHOWERANA_H
#define TRACKSHOWERANA_H

#include "Base/larcv_base.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"
#include "Base/PSet.h"
#include <opencv2/opencv.hpp>
#include "TFile.h"
#include "TTree.h"

namespace larcv {

  class TrackShowerAna : public larcv_base {
    
  public:
    
    TrackShowerAna();
    ~TrackShowerAna(){}

    void
    Configure(const PSet& pset);
    
    bool
    Analyze(const cv::Mat& adc_img,
	    const cv::Mat& track_img,
	    const cv::Mat& shower_img);

    void
    Finalize(TFile* file);
    
  private:

    void Clear();
    
    void
    CalcAndFill(const larocv::GEO2D_ContourArray_t& ctor_v,
		const cv::Mat& img,
		TTree* tree);
    
    void
    FilterContours(larocv::GEO2D_ContourArray_t& ctor_v);

    uint _min_ctor_size;
    uint _min_points_inside;
    
    TTree* _track_tree;
    TTree* _shower_tree;

    float _length;
    float _width;
    float _perimeter;
    float _area;
    float _mean_pix_dist;
    float _sigma_pix_dist;
    float _n_pixel;
    float _weighted_angle;



    
  };
}
#endif


