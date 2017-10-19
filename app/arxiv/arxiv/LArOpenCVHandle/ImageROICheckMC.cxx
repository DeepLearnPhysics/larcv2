#ifndef __IMAGEROICHECKMC_CXX__
#define __IMAGEROICHECKMC_CXX__

#include "ImageROICheckMC.h"
#include "DataFormat/EventROI.h"
#include "LArbysUtils.h"

namespace larcv {

  static ImageROICheckMCProcessFactory __global_ImageROICheckMCProcessFactory__;

  ImageROICheckMC::ImageROICheckMC(const std::string name)
    : ProcessBase(name)
  {}
    
  void ImageROICheckMC::configure(const PSet& cfg)
  {
    _producer_roi = cfg.get<std::string>("MCProducer");
    _check_num_bb = cfg.get<bool>("CheckNumNuROIs");
    _check_vtxfid = cfg.get<bool>("CheckVertexInFid");
  }

  void ImageROICheckMC::initialize(){}    
    
  bool ImageROICheckMC::CheckNumNuROIs(IOManager& mgr)
  {
    const auto event_roi = (larcv::EventROI*) mgr.get_data(kProductROI,_producer_roi);

    auto roi = (*event_roi).at(0);

    if ((roi.BB()).size() != 3) { return false; }

    return true;
  }

  bool ImageROICheckMC::VertexInFid(IOManager& mgr)
  {
    const auto event_roi = (larcv::EventROI*) mgr.get_data(kProductROI,_producer_roi);

    auto roi = (*event_roi).at(0);

    bool xInFid = (roi.X() < (xmax - edge_x) && (roi.X() > xmin + edge_x) );
    bool yInFid = (roi.Y() < (ymax - edge_y) && (roi.Y() > ymin + edge_y) );
    bool zInFid = (roi.Z() < (zmax - edge_z) && (roi.Z() > zmin + edge_z) );

    if (xInFid && yInFid && zInFid) {return true;}

    return false;
  }


  bool ImageROICheckMC::process(IOManager& mgr)
  {
    if (_check_num_bb)
      {
	if (CheckNumNuROIs(mgr) == false) {
	  LARCV_DEBUG() << "ROI filtered @ entry: " << mgr.current_entry() << std::endl;
	  return false;
	}

      }

    if (_check_vtxfid)
      {
	if (VertexInFid(mgr) == false) {
	  LARCV_DEBUG() << "Vertex filtered @ entry: " << mgr.current_entry() << std::endl;
	  return false;
	}
      }

    return true;
  }
  
  void ImageROICheckMC::finalize(){}

}
#endif

