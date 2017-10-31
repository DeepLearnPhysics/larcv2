#ifndef __HISTADC_CXX__
#define __HISTADC_CXX__

#include "HistADC.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventParticle.h"

namespace larcv {

  static HistADCProcessFactory __global_HistADCProcessFactory__;

  HistADC::HistADC(const std::string name)
    : ProcessBase(name)
  {}

  void HistADC::configure(const PSet& cfg)
  {
    fImageProducer     = cfg.get<std::string>("ImageProducer");
    fParticleProducer  = cfg.get<std::string>("ParticleProducer");
    fPlane0Thresh      = cfg.get<int>("Plane0Thresh");
    fPlane1Thresh      = cfg.get<int>("Plane1Thresh");
    fPlane2Thresh      = cfg.get<int>("Plane2Thresh");
  }

  void HistADC::initialize()
  {
    // clear the vector
    for (auto &p : m_hADC_v ) {
      delete p;
      p = NULL;
    }
    m_hADC_v.clear();

    m_tree = new TTree("adctree", "ADC/pixel summary of cropped regions");
    m_tree->Branch( "plane", &m_plane, "plane/I");
    m_tree->Branch( "npixels", &m_npixels, "npixels/I");
    m_tree->Branch( "adcsum", &m_sum, "adcsum/F");
  }

  bool HistADC::process(IOManager& mgr)
  {

    auto const& event_image = mgr.get_data<larcv::EventImage2D>(fImageProducer);
    //auto const& event_part  = mgr.get_data<larcv::EventParticle>(fParticleProducer);
    //auto const& part = event_part.as_vector().front();

    // if ( fFillCosmic && roi.Type()!=kROICosmic)
    //   return true;
    // if ( !fFillCosmic && roi.Type()==kROICosmic )
    //   return true;

    for ( auto const& img : event_image.image2d_array()) {
      auto const plane = img.meta().id();
      if (m_hADC_v.size() <= plane) m_hADC_v.resize(plane + 1, nullptr);

      // if(!m_hADC_v[plane]) {
      //  if (fFillCosmic )
      //    m_hADC_v[plane] = new TH1D(Form("hADCsum_%s_Plane%02hu_cosmic",fHiResCropProducer.c_str(),plane),
      //             Form("ADC Values for Plane %hu",plane),
      //             500,0,10e3);
      //  else
      //    m_hADC_v[plane] = new TH1D(Form("hADCsum_%s_Plane%02hu_neutrino",fHiResCropProducer.c_str(),plane),
      //             Form("ADC Values for Plane %hu",plane),
      //             500,0,10e3);
      // }


      m_sum = 0.0;
      m_npixels = 0;
      auto const& adc_v = img.as_vector();

      float thresh = 0;
      switch ( plane ) {
      case 0:
        thresh = fPlane0Thresh;
        break;
      case 1:
        thresh = fPlane1Thresh;
        break;
      case 2:
        thresh = fPlane2Thresh;
        break;
      }

      for ( auto const& adc : adc_v ) {
        if ( adc > thresh ) {
          m_sum += adc;
          m_npixels++;
        }
      }

      //auto& p_hADC = m_hADC_v[plane];
      //std::cout << sum << std::endl;
      //p_hADC->Fill( m_sum );
      m_plane = plane;
      m_tree->Fill();
    }//end of loop over plane image
    return true;
  }

  void HistADC::finalize()
  {
    if ( has_ana_file() ) {
      //for(auto& p : m_hADC_v) if(p) p->Write();
      m_tree->Write();
    }
  }

}
#endif
