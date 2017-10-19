#ifndef __LARBYSIMAGEEXTRACT_H__
#define __LARBYSIMAGEEXTRACT_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include "LArbysImageMaker.h"
#include "PreProcessor.h"
#include "DataFormat/EventROI.h"

namespace larcv {

  class LArbysImageExtract : public ProcessBase {

  public:
    
    LArbysImageExtract(const std::string name="LArbysImageExtract");
    ~LArbysImageExtract(){}

    void configure(const PSet&);
    void initialize();
    bool process(IOManager& mgr);
    void finalize();

    const std::vector<larocv::ImageMeta>& ADCMetas() const { return _adc_meta_v; }
    const std::vector<cv::Mat>& ADCImages() const { return _adc_mat_v; }
    const std::vector<cv::Mat>& TrackImages() const { return _track_mat_v; }
    const std::vector<cv::Mat>& ShowerImages() const { return _shower_mat_v; }    
    const std::vector<cv::Mat>& ThruMuImages() const { return _thrumu_mat_v; }
    const std::vector<cv::Mat>& StopMuImages() const { return _stopmu_mat_v; }
    
    const larocv::ImageMeta& ADCMeta(size_t planeid) const { return _adc_meta_v[planeid]; }
    const cv::Mat& ADCImage(size_t planeid) const { return _adc_mat_v[planeid]; }
    const cv::Mat& TrackImage(size_t planeid) const { return _track_mat_v[planeid]; }
    const cv::Mat& ShowerImage(size_t planeid) const { return _shower_mat_v[planeid]; }
    const cv::Mat& ThruMuImage(size_t planeid) const { return _thrumu_mat_v[planeid]; }
    const cv::Mat& StopMuImage(size_t planeid) const { return _stopmu_mat_v[planeid]; }


    const std::vector<ROI>& TrueROI() const { return _true_roi_v; }
    const std::vector<ROI>& RecoROI() const { return _reco_roi_v; }
    
    
    const LArbysImageMaker& maker() const { return _LArbysImageMaker; }
    const PreProcessor& pproc() const { return _PreProcessor; }

    void FillcvMat(larcv::ROI* roi=nullptr);
    
  private:
    std::string _adc_producer;
    std::string _track_producer;
    std::string _shower_producer;
    std::string _thrumu_producer;
    std::string _stopmu_producer;
    std::string _true_roi_producer;
    std::string _reco_roi_producer;
    
    LArbysImageMaker _LArbysImageMaker;
    PreProcessor _PreProcessor;

    // The ROIs
    std::vector<ROI> _true_roi_v;
    std::vector<ROI> _reco_roi_v;
    
    // The images
    std::vector<larocv::ImageMeta> _adc_meta_v;
    std::vector<cv::Mat> _adc_mat_v;
    std::vector<cv::Mat> _track_mat_v;
    std::vector<cv::Mat> _shower_mat_v;
    std::vector<cv::Mat> _thrumu_mat_v;
    std::vector<cv::Mat> _stopmu_mat_v;

    EventImage2D _ev_adc;
    EventImage2D _ev_trk;
    EventImage2D _ev_shr;    
    EventImage2D _ev_thrumu;
    EventImage2D _ev_stopmu;
    
    EventPixel2D _ev_thrumu_pix;
    EventPixel2D _ev_stopmu_pix;

  public:

    const EventImage2D& ev_adc() const { return _ev_adc; }
    const EventImage2D& ev_trk() const { return _ev_trk; }
    const EventImage2D& ev_shr() const { return _ev_shr; }
    const EventImage2D& ev_thrumu() const { return _ev_thrumu; }
    const EventImage2D& ev_stopmu() const { return _ev_stopmu; }

    const std::vector<Image2D>& adc_img_v() const { return _ev_adc.Image2DArray(); }
    const std::vector<Image2D>& trk_img_v() const { return _ev_trk.Image2DArray(); }
    const std::vector<Image2D>& shr_img_v() const { return _ev_shr.Image2DArray(); }
    const std::vector<Image2D>& thrumu_v() const { return _ev_thrumu.Image2DArray(); }
    const std::vector<Image2D>& stopmu_v() const { return _ev_stopmu.Image2DArray(); }
    
    const Image2D& adc_img(size_t plane) const { return _ev_adc.Image2DArray().at(plane); }
    const Image2D& trk_img(size_t plane) const { return _ev_trk.Image2DArray().at(plane); }
    const Image2D& shr_img(size_t plane) const { return _ev_shr.Image2DArray().at(plane); }
    const Image2D& thumu_img(size_t plane) const { return _ev_stopmu.Image2DArray().at(plane); }
    const Image2D& stopmu_img(size_t plane) const { return _ev_thrumu.Image2DArray().at(plane); }
    
    const EventPixel2D& ev_thrumu_pix() const { return _ev_thrumu_pix; }
    const EventPixel2D& ev_stopmu_pix() const { return _ev_stopmu_pix; }
    const std::map<PlaneID_t,std::vector<Pixel2DCluster> >& thrumu_pix_m() const { return _ev_thrumu_pix.Pixel2DClusterArray(); }
    const std::map<PlaneID_t,std::vector<Pixel2DCluster> >& stopmu_pix_m() const { return _ev_stopmu_pix.Pixel2DClusterArray(); }
    const std::vector<Pixel2DCluster>& thrumu_pix_v(size_t plane) const { return _ev_thrumu_pix.Pixel2DClusterArray().at(plane); }
    const std::vector<Pixel2DCluster>& stopmu_pix_v(size_t plane) const { return _ev_stopmu_pix.Pixel2DClusterArray().at(plane); }
    
  };

  class LArbysImageExtractProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    LArbysImageExtractProcessFactory() { ProcessFactory::get().add_factory("LArbysImageExtract",this); }
    /// dtor
    ~LArbysImageExtractProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new LArbysImageExtract(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

