#ifndef __SUPERAMCPCLUSTER_CXX__
#define __SUPERAMCPCLUSTER_CXX__

#include "SuperaMCPCluster.h"
#include "DataFormat/EventROI.h"
#include "DataFormat/EventPixel2D.h"
#include "LAr2Image.h"

namespace larcv {

  static SuperaMCPClusterProcessFactory __global_SuperaMCPClusterProcessFactory__;

  SuperaMCPCluster::SuperaMCPCluster(const std::string name)
    : SuperaMCROI(name)
  {}
    
  void SuperaMCPCluster::configure(const PSet& cfg)
  {
    SuperaMCROI::configure(cfg);
    supera::ParamsPixel2D::configure(cfg);
  }

  void SuperaMCPCluster::initialize()
  { SuperaMCROI::initialize(); }

  bool SuperaMCPCluster::process(IOManager& mgr)
  {
    SuperaMCROI::process(mgr);
    
    auto ev_pixel2d = (EventPixel2D*)(mgr.get_data(kProductPixel2D,OutPixel2DLabel()));
    if(!ev_pixel2d) {
      LARCV_CRITICAL() << "Failed to create an output pixel2d w/ label " << OutPixel2DLabel() << std::endl;
      throw larbys();
    }

    auto ev_roi = (EventROI*)(mgr.get_data(kProductROI,OutROILabel()));
    if(!ev_roi) {
      LARCV_CRITICAL() << "Failed to fetch ROI (needs to be created) by " << OutROILabel() << std::endl;
      throw larbys();
    }

    std::vector<size_t> trackid2cluster;
    auto const& roi2mcnode_vv = ROI2MCNode();
    auto const& primary_v = ParticleTree().PrimaryArray();

    // check ROI length
    if(ev_roi->ROIArray().size() != roi2mcnode_vv.size()) {
      LARCV_CRITICAL() << "ROI2MCNode() return length not same as EventROI!" << std::endl;
      throw larbys();
    }

    auto const& mcshower_v = LArData<supera::LArMCShower_t>();
    
    for(size_t roi_idx=0; roi_idx<roi2mcnode_vv.size(); ++roi_idx) {

      auto const& roi2mcnode_v = roi2mcnode_vv[roi_idx];

      for(auto const& mcnode_info : roi2mcnode_v) {

	auto const& primary_idx = mcnode_info.first;
	auto const& secondary_idx = mcnode_info.second;

	auto const& primary = primary_v[primary_idx];

	// if this is secondary
	if(secondary_idx != larcv::kINVALID_SIZE) {
	  auto const& node = primary.daughter_v[secondary_idx];
	  if(trackid2cluster.size() <= node.track_id)
	    trackid2cluster.resize(node.track_id+1,larcv::kINVALID_SIZE);
	  trackid2cluster[node.track_id] = roi_idx;
	  // if this is mcshower
	  if(node.source_type == supera::MCNode::SourceType_t::kMCShower) {
	    auto const& mcshower = mcshower_v.at(node.source_index);
	    for(auto const& daughter_track_id : mcshower.DaughterTrackID()) {
	      if(daughter_track_id == larcv::kINVALID_UINT) continue;
	      if(trackid2cluster.size() <= daughter_track_id)
		trackid2cluster.resize(daughter_track_id+1,larcv::kINVALID_SIZE);
	      trackid2cluster[daughter_track_id] = roi_idx;
	    }
	  }
	  continue;
	}
	// if primary but mctruth
	if(primary.source_type == supera::MCNode::SourceType_t::kMCTruth)
	  continue;
	// if primary and not mctruth
	if(trackid2cluster.size() <= primary.track_id)
	  trackid2cluster.resize(primary.track_id+1);
	trackid2cluster[primary.track_id] = roi_idx;

	if(primary.source_type == supera::MCNode::SourceType_t::kMCShower) {
	  auto const& mcshower = mcshower_v.at(primary.source_index);
	  for(auto const& daughter_track_id : mcshower.DaughterTrackID()) {
	    if(daughter_track_id == larcv::kINVALID_UINT) continue;
	    if(trackid2cluster.size() <= daughter_track_id)
	      trackid2cluster.resize(daughter_track_id+1,larcv::kINVALID_SIZE);
	    trackid2cluster[daughter_track_id] = roi_idx;
	  }
	}
      }
    }

    auto pxcluster_vv = supera::SimCh2Pixel2DCluster(Meta(),
						     RowCompressionFactor(),
						     ColCompressionFactor(),
						     LArData<supera::LArSimCh_t>(),
						     trackid2cluster,
						     TimeOffset());

    auto const& roi_v = ev_roi->ROIArray();
    if(roi_v.size() != pxcluster_vv.size()) {
      LARCV_CRITICAL() << "# PixelCluster does not match with # ROI!" << std::endl;
      throw larbys();
    }

    auto meta_v = Meta();
    for(auto& meta: meta_v) meta.update(meta.rows() / RowCompressionFactor().at(meta.plane()),
					meta.cols() / ColCompressionFactor().at(meta.plane()));
    
    for(size_t roi_idx=0; roi_idx<roi_v.size(); ++roi_idx) {

      auto const& pc_v = pxcluster_vv.at(roi_idx);

      if(pc_v.size() != supera::Nplanes()) {
	LARCV_CRITICAL() << "# pixel cluster not same as # planes!" << std::endl;
	throw larbys();
      }

      for(size_t plane=0; plane<supera::Nplanes(); ++plane)

	ev_pixel2d->Emplace(plane, std::move(pxcluster_vv[roi_idx][plane]), meta_v.at(plane));
      
    }
    
    return true;
  }

  void SuperaMCPCluster::finalize()
  { SuperaMCROI::finalize(); }

}
#endif
