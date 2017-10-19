#ifndef __IMAGECHARGEANA_CXX__
#define __IMAGECHARGEANA_CXX__

#include "ImageChargeAna.h"
#include "DataFormat/EventImage2D.h"
namespace larcv {

  static ImageChargeAnaProcessFactory __global_ImageChargeAnaProcessFactory__;

  ImageChargeAna::ImageChargeAna(const std::string name)
    : ProcessBase(name)
  {}
    
  void ImageChargeAna::configure(const PSet& cfg)
  {}

  void ImageChargeAna::initialize()
  {
    _tree = new TTree("qtree","");
    _tree->Branch("q0",&_q0,"q0/D");
    _tree->Branch("q1",&_q1,"q1/D");
    _tree->Branch("q2",&_q2,"q2/D");
    _tree->Branch("qsum",&_qsum,"qsum/D");
  }

  bool ImageChargeAna::process(IOManager& mgr)
  {
    auto ev_image = (EventImage2D*)(mgr.get_data(kProductImage2D,"tpc_hires_crop"));
    auto const& images = ev_image->Image2DArray();
    auto const& image0 = images[0];
    auto const& image1 = images[1];
    auto const& image2 = images[2];
    _q0=_q1=_q2=_qsum=0.;
    for(auto const& v : image0.as_vector())

      if(v > 17.) _q0 += v;

    for(auto const& v : image1.as_vector())

      if(v > 13.) _q1 += v;

    for(auto const& v : image2.as_vector())

      if(v > 20.) _q2 += v;

    _qsum = _q0 + _q1 + _q2;

    _tree->Fill();
    return true;
  }

  void ImageChargeAna::finalize()
  {
    if(has_ana_file()) _tree->Write();
  }

}
#endif
