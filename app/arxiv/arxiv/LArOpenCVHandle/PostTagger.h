#ifndef __POSTTAGGER_H__
#define __POSTTAGGER_H__

#include "Base/larcv_base.h"
#include "Base/PSet.h"
#ifndef __CLING__
#ifndef __CINT__
#include <opencv2/opencv.hpp>
#endif
#endif

namespace larcv {
  class PostTagger : public larcv_base {

  public:
    PostTagger();
    ~PostTagger() {}
  
  private:
  
    void Configure(const PSet& pset);
    void MergeTaggedPixels(cv::Mat& adc_img,
			   cv::Mat& trk_img,
			   cv::Mat& shr_img,
			   cv::Mat& thrumu_img,
			   cv::Mat& stopmu_img);
    
    unsigned _ctor_sz_threshold;
    bool _check_thrumu;
    bool _check_stopmu;

  
  };
}

#endif
