#ifndef __PREPROCESSORANA_CXX__
#define __PREPROCESSORANA_CXX__

#include "PreProcessorAna.h"
#include "CVUtil/CVUtil.h"
#include "DataFormat/EventImage2D.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

namespace larcv {

  static PreProcessorAnaProcessFactory __global_PreProcessorAnaProcessFactory__;

  PreProcessorAna::PreProcessorAna(const std::string name)
    : ProcessBase(name),
      _out_tree(nullptr)
  {}
    
  void PreProcessorAna::configure(const PSet& cfg)
  {
    _PreProcessor.Configure(cfg.get<larcv::PSet>("PreProcessor"));
  }

  void PreProcessorAna::initialize()
  {

    _out_tree = new TTree("ssana","");

    _out_tree->Branch("true_track_ctr_v",&_true_track_ctr_v);
    _out_tree->Branch("true_shower_ctr_v",&_true_shower_ctr_v);

    _out_tree->Branch("ss_track_ctr_v",&_ss_track_ctr_v);
    _out_tree->Branch("ss_shower_ctr_v",&_ss_shower_ctr_v);

    _out_tree->Branch("pss_track_ctr_v",&_pss_track_ctr_v);
    _out_tree->Branch("pss_shower_ctr_v",&_pss_shower_ctr_v);

    _out_tree->Branch("ss_ratio_v",&_ss_ratio_v);
    _out_tree->Branch("pss_ratio_v",&_pss_ratio_v);
    
  }

  void PreProcessorAna::Clear() {
    _true_track_ctr_v.clear();
    _true_shower_ctr_v.clear();
    _ss_track_ctr_v.clear();
    _ss_shower_ctr_v.clear();
    _pss_track_ctr_v.clear();
    _pss_shower_ctr_v.clear();
    _ss_ratio_v.clear();
    _pss_ratio_v.clear();
    
    _true_track_ctr_v.resize(3);
    _true_shower_ctr_v.resize(3);
    _ss_track_ctr_v.resize(3);
    _ss_shower_ctr_v.resize(3);
    _pss_track_ctr_v.resize(3);
    _pss_shower_ctr_v.resize(3);
    _ss_ratio_v.resize(3);
    _pss_ratio_v.resize(3);
  }
  
  bool PreProcessorAna::process(IOManager& mgr)
  {

    // adc image
    const auto adc_ev_img = (EventImage2D*)mgr.get_data(kProductImage2D,"tpc_hires_crop");
    
    std::vector<cv::Mat> adc_v;
    adc_v.reserve(3);
    for(const auto& adc_img2d : adc_ev_img->Image2DArray())
      adc_v.emplace_back(as_gray_mat(adc_img2d,0.1,500,2));
    
    // true track and ss shower
    const auto true_track_ev_img = (EventImage2D*)mgr.get_data(kProductImage2D,"true_track_hires_crop");
    const auto true_shower_ev_img = (EventImage2D*)mgr.get_data(kProductImage2D,"true_shower_hires_crop");

    std::vector<cv::Mat> true_track_v;
    std::vector<cv::Mat> true_shower_v;
    true_track_v.reserve(3);
    true_shower_v.reserve(3);
    
    for(const auto& true_track_img2d : true_track_ev_img->Image2DArray())
      true_track_v.emplace_back(as_gray_mat(true_track_img2d,0.1,500,2));

    for(const auto& true_shower_img2d : true_shower_ev_img->Image2DArray())
      true_shower_v.emplace_back(as_gray_mat(true_shower_img2d,0.1,500,2));
    
    // ss track and ss shower
    const auto ss_track_ev_img = (EventImage2D*)mgr.get_data(kProductImage2D,"ss_track_hires_crop");
    const auto ss_shower_ev_img = (EventImage2D*)mgr.get_data(kProductImage2D,"ss_shower_hires_crop");
    
    std::vector<cv::Mat> ss_track_v,ss_shower_v;
    ss_track_v.reserve(3);
    ss_shower_v.reserve(3)
      ;
    for(const auto& ss_track_img2d : ss_track_ev_img->Image2DArray())
      ss_track_v.emplace_back(as_gray_mat(ss_track_img2d,0.1,500,2));

    for(const auto& ss_shower_img2d : ss_shower_ev_img->Image2DArray())
      ss_shower_v.emplace_back(as_gray_mat(ss_shower_img2d,0.1,500,2));

    Clear();
    
    for(size_t plane=0;plane<adc_v.size();++plane) {
      auto& adc_img = adc_v[plane];
      
      auto& ss_track_img = ss_track_v[plane];
      auto& ss_shower_img = ss_shower_v[plane];

      auto pss_track_img = ss_track_img.clone();
      auto pss_shower_img = ss_shower_img.clone();
      
      _PreProcessor.PreProcess(adc_img,pss_track_img,pss_shower_img);

      auto& true_track_img = true_track_v[plane];
      auto& true_shower_img = true_shower_v[plane];

      //binary images
      auto true_track_img_t = larocv::Threshold(true_track_img,10,1);
      auto true_shower_img_t = larocv::Threshold(true_shower_img,10,1);
      auto ss_track_img_t = larocv::Threshold(ss_track_img,10,1);
      auto ss_shower_img_t = larocv::Threshold(ss_shower_img,10,1);
      auto pss_track_img_t = larocv::Threshold(pss_track_img,10,1);
      auto pss_shower_img_t = larocv::Threshold(pss_shower_img,10,1);
      
      float true_track_ctr = cv::countNonZero(true_track_img_t);
      float true_shower_ctr = cv::countNonZero(true_shower_img_t);
      
      cv::Mat out;

      cv::bitwise_and(true_track_img_t,ss_track_img_t,out);
      float ss_track_ctr = cv::countNonZero(out);
      cv::bitwise_and(true_shower_img_t,ss_shower_img_t,out);
      float ss_shower_ctr = cv::countNonZero(out);
      cv::bitwise_and(true_track_img_t,pss_track_img_t,out);
      float pss_track_ctr = cv::countNonZero(out);
      cv::bitwise_and(true_shower_img_t,pss_shower_img_t,out);
      float pss_shower_ctr = cv::countNonZero(out);

      float ss_ratio = (ss_track_ctr+ss_shower_ctr)/(true_track_ctr+true_shower_ctr);
      float pss_ratio = (pss_track_ctr+pss_shower_ctr)/(true_track_ctr+true_shower_ctr);

      
      _true_track_ctr_v[plane]  = true_track_ctr;
      _true_shower_ctr_v[plane] = true_shower_ctr;

      _ss_track_ctr_v[plane] = ss_track_ctr;
      _ss_shower_ctr_v[plane] = ss_shower_ctr;
      
      _pss_track_ctr_v[plane] = pss_track_ctr;
      _pss_shower_ctr_v[plane] = pss_shower_ctr;

      _ss_ratio_v[plane] = ss_ratio;
      _pss_ratio_v[plane] = pss_ratio;
      
    }

    _out_tree->Fill();

    //Determine the correct fraction for this event
    
    
    return true;
  }

  void PreProcessorAna::finalize()
  {
    _out_tree->Write();
  }

}
#endif
