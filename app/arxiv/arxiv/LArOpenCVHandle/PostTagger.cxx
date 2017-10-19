#ifndef __POSTTAGGER_CXX__
#define __POSTTAGGER_CXX__

#include "PostTagger.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

namespace larcv {

  PostTagger::PostTagger() {
    _ctor_sz_threshold = 3;
    _check_thrumu = true;
    _check_stopmu = true;
  }

  void PostTagger::Configure(const PSet& pset) {
    return;
  }
  
  void PostTagger::MergeTaggedPixels(cv::Mat& adc_img,
				     cv::Mat& trk_img,
				     cv::Mat& shr_img,
				     cv::Mat& thrumu_img,
				     cv::Mat& stopmu_img) {
    
    auto ctor_v = larocv::FindContours(adc_img);

    for(const auto& ctor : ctor_v) {
      if (ctor.size() <= _ctor_sz_threshold) continue;

      if (_check_thrumu && larocv::CountNonZero(larocv::MaskImage(thrumu_img,ctor,0,false))) {
	adc_img = larocv::MaskImage(adc_img,ctor,0,true);
	shr_img = larocv::MaskImage(shr_img,ctor,0,true);
	trk_img = larocv::MaskImage(trk_img,ctor,0,true);
      }

      if (_check_stopmu && larocv::CountNonZero(larocv::MaskImage(stopmu_img,ctor,0,false))) {
	adc_img = larocv::MaskImage(adc_img,ctor,0,true);
	shr_img = larocv::MaskImage(shr_img,ctor,0,true);
	trk_img = larocv::MaskImage(trk_img,ctor,0,true);
      }
      
    } // end this contour
  }

}


#endif
