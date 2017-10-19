#ifndef __COSMICROIFILLER_CXX__
#define __COSMICROIFILLER_CXX__

#include "CosmicROIFiller.h"
#include "DataFormat/EventROI.h"
namespace larcv {

static CosmicROIFillerProcessFactory __global_CosmicROIFillerProcessFactory__;

CosmicROIFiller::CosmicROIFiller(const std::string name)
  : ProcessBase(name)
{}

void CosmicROIFiller::configure(const PSet& cfg)
{
  _ref_type = (ProductType_t)(cfg.get<unsigned short>("RefType"));
  _ref_producer = cfg.get<std::string>("RefProducer");
  _roi_producer = cfg.get<std::string>("ROIProducer");
  _roi_type = cfg.get<int>("ROIType");
}

void CosmicROIFiller::initialize()
{}

bool CosmicROIFiller::process(IOManager& mgr)
{
  auto ref_data = mgr.get_data(_ref_type, _ref_producer);
  if (!ref_data) {
    LARCV_CRITICAL() << "Could not find reference product type " << _ref_type
                     << " made by " << _ref_producer << std::endl;
    throw larbys();
  }
  auto event_roi = (EventROI*)(mgr.get_data(kProductROI, _roi_producer));
  std::vector<larcv::ROI> roi_v;
  ROI cosmic_roi;
  cosmic_roi.Type((ROIType_t) _roi_type);
  roi_v.emplace_back(cosmic_roi);
  event_roi->Emplace(std::move(roi_v));
  return true;
}

void CosmicROIFiller::finalize()
{}

}
#endif
