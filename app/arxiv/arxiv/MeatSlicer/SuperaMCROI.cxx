#ifndef __SUPERAMCBASE_CXX__
#define __SUPERAMCBASE_CXX__

#include "SuperaMCROI.h"
#include "ImageMetaMakerFactory.h"
#include "PulledPork3DSlicer.h"
#include "DataFormat/EventROI.h"
#include "LAr2Image.h"

namespace larcv {

  static SuperaMCROIProcessFactory __global_SuperaMCROIProcessFactory__;

  SuperaMCROI::SuperaMCROI(const std::string name)
    : SuperaBase(name)
  {}

  const std::vector<std::pair<size_t,size_t> >& SuperaMCROI::ROI2MCNode(int roi_index) const
  {
    if(roi_index >= (int)(_roi2mcnode_vv.size()))
      throw larbys("Invalid ROI index requested");

    return _roi2mcnode_vv[roi_index];
  }
    
  void SuperaMCROI::configure(const PSet& cfg)
  {
    SuperaBase::configure(cfg);
    supera::ParamsROI::configure(cfg);
    supera::ImageMetaMaker::configure(cfg);
    
    _store_roi = cfg.get<bool>("StoreROI",true);
    _store_g4_secondary_roi = cfg.get<bool>("StoreG4SecondaryROI",true);
    _store_g4_primary_roi = cfg.get<bool>("StoreG4PrimaryROI",true);
    _mcpt.configure(cfg.get<supera::Config_t>("MCParticleTree"));
    _mcroi_maker.configure(cfg.get<supera::Config_t>("MCROIMaker"));

    _pass_origin = cfg.get<unsigned short>("Origin");
    _mcpt.FilterOrigin(_pass_origin);
    
    _filter_pdg  = cfg.get<std::vector<int> >("FilterTargetPDG");
    _filter_min_einit = cfg.get<std::vector<double> >("FilterTargetInitEMin");
    _filter_min_edep  = cfg.get<std::vector<double> >("FilterTargetDepEMin");

    if(_filter_pdg.size() != _filter_min_einit.size()) {
      LARCV_CRITICAL() << "FilterTargetPDG and FilterTargetInitEMin not the same length!" << std::endl;
      throw larbys();
    }

    if(_filter_pdg.size() != _filter_min_edep.size()) {
      LARCV_CRITICAL() << "FilterTargetPDG and FilterTargetDepEMin not the same length!" << std::endl;
      throw larbys();
    }

    _shower_min_einit = cfg.get<double>("ShowerInitEMin");
    _shower_min_edep  = cfg.get<double>("ShowerDepEMin");

    _track_min_einit = cfg.get<double>("TrackInitEMin");
    _track_min_edep  = cfg.get<double>("TrackDepEMin");

    _filter_min_cols = cfg.get<size_t>("FilterROIMinCols");
    _filter_min_rows = cfg.get<size_t>("FilterROIMinRows");
    
  }

  void SuperaMCROI::initialize()
  {
    SuperaBase::initialize();
  }

  bool SuperaMCROI::process(IOManager& mgr)
  {
    SuperaBase::process(mgr);

    if(supera::PulledPork3DSlicer::Is(supera::ImageMetaMaker::MetaMakerPtr())) {
      auto ptr = (supera::PulledPork3DSlicer*)(supera::ImageMetaMaker::MetaMakerPtr());
      ptr->ClearEventData();
      ptr->AddConstraint(LArData<supera::LArMCTruth_t>());
      ptr->GenerateMeta(LArData<supera::LArSimCh_t>(),TimeOffset());
    }
    
    auto const& meta_v = Meta();

    if(meta_v.empty()) {
      LARCV_CRITICAL() << "Meta not created!" << std::endl;
      throw larbys();
    }
    auto ev_roi = (EventROI*)(mgr.get_data(kProductROI,OutROILabel()));
    if(!ev_roi) {
      LARCV_CRITICAL() << "Output roi could not be created!" << std::endl;
      throw larbys();
    }
    if(!(ev_roi->ROIArray().empty())) {
      LARCV_CRITICAL() << "Output roi array not empty!" << std::endl;
      throw larbys();
    }

    LARCV_INFO() << "Running MCParticleTree::Register" << std::endl;
    _mcpt.Register(LArData<supera::LArMCTruth_t>(),
		   LArData<supera::LArMCTrack_t>(),
		   LArData<supera::LArMCShower_t>());

    auto primary_v = _mcpt.PrimaryArray();
    LARCV_INFO() << "Found " << primary_v.size() << " primary particles" << std::endl;

    std::vector<supera::LArSimCh_t> empty_sch_v;

    _roi_v.clear();
    _roi2mcnode_vv.clear();

    for(size_t primary_idx = 0; primary_idx < primary_v.size(); ++primary_idx) {
      
      auto& primary = primary_v[primary_idx];

      // filter out primary of certain origin, if specified
      if(_pass_origin && primary.origin != _pass_origin) {
	LARCV_INFO() << "Skipping MCTruth primary " << primary_idx
		     << " (origin type " << primary.origin
		     << " PDG " << primary.pdg
		     << ") with " << primary.daughter_v.size() << " children"
		     << std::endl;
	continue;
      }

      if(primary.source_type != supera::MCNode::SourceType_t::kMCTruth) {
	if(!FilterNode(primary)) {
	  LARCV_INFO() << "Skipping a non-MCTruth primary (TrackID " << primary.track_id
		       << " Origin " << primary.origin
		       << " PDG " << primary.pdg
		       << ") due to FilterNode()" << std::endl;
	  continue;
	}

	if(LArDataLabel(supera::LArDataType_t::kLArSimCh_t).empty())
	  primary.roi = MakeROI(primary,empty_sch_v);	  
	else
	  primary.roi = MakeROI(primary,LArData<supera::LArSimCh_t>());
      }

      LARCV_INFO() << "Analyzing primary " << primary_idx << " PDG " << primary.roi.PdgCode()
		   << " Origin " << primary.origin
		   << " PDG << " << primary.pdg
		   << " with " << primary.daughter_v.size() << " children" << std::endl;

      std::vector<larcv::ROI> sec_roi_v;
      std::vector<size_t> sec_idx_v;
      for(size_t daughter_idx=0; daughter_idx<primary.daughter_v.size(); ++daughter_idx) {

	auto const& daughter = primary.daughter_v[daughter_idx];

	if(!FilterNode(daughter)) {
	  LARCV_INFO() << "Skipping a daughter " << daughter_idx
		       << " (TrackID " << daughter.track_id
		       << " Origin " << daughter.origin
		       << " PDG " << daughter.pdg
		       << ") due to FilterNode()" << std::endl;
	  continue;
	}
	larcv::ROI roi;
	try {
	  if(LArDataLabel(supera::LArDataType_t::kLArSimCh_t).empty())
	    roi = MakeROI(daughter,empty_sch_v);	  
	  else
	    roi = MakeROI(daughter,LArData<supera::LArSimCh_t>());
	}catch(const larcv::larbys& err) {
	  LARCV_NORMAL() << "Skipping a secondary (PDG,TrackID) = (" 
			 << daughter.pdg << "," << daughter.track_id << ") as it could not be turned into ROI" << std::endl;
	  continue;
	}
	if( (_filter_min_rows>0 || _filter_min_cols>0) && roi.BB().empty() ) {
	  LARCV_INFO() << "Skipping a daughter " << daughter_idx
		       << " (TrackID " << daughter.track_id
		       << " Origin " << daughter.origin
		       << " PDG " << roi.PdgCode() << ") due to the size of ROI" << std::endl;
	  continue;
	}
	bool skip=false;
	for(auto const& bb : roi.BB()) {
	  if(bb.rows() <= _filter_min_rows && bb.cols() <= _filter_min_cols) {
	    LARCV_INFO() << "Skipping a daughter " << daughter_idx
			 << " (TrackID " << daughter.track_id
			 << " Origin " << daughter.origin
			 << " PDG " << roi.PdgCode() << ") due to the size of ROI "
			 << "(row,col) = (" << bb.rows() << "," << bb.cols() << ")"
			 << std::endl;
	    skip=true;
	    break;
	  }
	}
	if(skip) continue;
	LARCV_INFO() << "Registering a daughter " << daughter_idx
		     << " (TrackID " << daughter.track_id
		     << " Origin " << daughter.origin
		     << " PDG " << roi.PdgCode() << ")" << std::endl;
	sec_roi_v.push_back(roi);
	sec_idx_v.push_back(daughter_idx);
      }

      LARCV_INFO() << "Updating primary ROI with " << sec_roi_v.size() << " children" << std::endl;
      UpdatePrimaryROI(primary.roi, sec_roi_v);

      if( (_filter_min_rows>0 || _filter_min_cols>0) && primary.roi.BB().empty() )
	continue;
      bool skip=false;
      for(auto const& bb : primary.roi.BB()) {
	if(bb.rows() >= _filter_min_rows && bb.cols() >= _filter_min_cols)
	  continue;
	skip=true;
	break;
      }
      if(skip) continue;

      std::vector<std::pair<size_t,size_t> > roi2mcnode_v;
      roi2mcnode_v.clear();

      //
      // Register primary ROI
      //
      LARCV_INFO() << "Storing primary ROI (PDG " << primary.roi.PdgCode()
		   << " Shape " << primary.roi.Shape()
		   << " MCTIndex " << primary.roi.MCTIndex()
		   << " MCSTIndex) " << primary.roi.MCSTIndex()
		   << std::endl;
	
      _roi_v.push_back(primary.roi);

      // Record incorporated mcnode
      roi2mcnode_v.emplace_back(primary_idx, larcv::kINVALID_SIZE);
      for(size_t daughter_idx=0; daughter_idx<sec_roi_v.size(); ++daughter_idx) {
	auto const& secondary_idx = sec_idx_v[daughter_idx];
	auto const& secondary_roi = sec_roi_v[daughter_idx];
	if( (_store_g4_primary_roi && secondary_roi.TrackID() == secondary_roi.ParentTrackID())
	    ||
	    (_store_g4_secondary_roi && secondary_roi.TrackID() != secondary_roi.ParentTrackID())
	    )
	  roi2mcnode_v.emplace_back(primary_idx,secondary_idx);
      }
      for(size_t daughter_idx=0; daughter_idx<sec_roi_v.size(); ++daughter_idx) {
	auto const& daughter_roi = sec_roi_v[daughter_idx];
	LARCV_INFO() << "    Associated secondary (PDG " << daughter_roi.PdgCode()
		     << " Shape " << daughter_roi.Shape()
		     << " MCTIndex " << daughter_roi.MCTIndex()
		     << " MCSTIndex) " << daughter_roi.MCSTIndex()
		     << std::endl;	
      }
      _roi2mcnode_vv.emplace_back(std::move(roi2mcnode_v));
      
      //
      // Register secondary ROI
      //
      roi2mcnode_v.clear();
      for(size_t daughter_idx=0; daughter_idx<sec_roi_v.size(); ++daughter_idx) {
	auto const& secondary_idx = sec_idx_v[daughter_idx];
	auto& secondary_roi = sec_roi_v[daughter_idx];
	if( (_store_g4_primary_roi && secondary_roi.TrackID() == secondary_roi.ParentTrackID())
	    ||
	    (_store_g4_secondary_roi && secondary_roi.TrackID() != secondary_roi.ParentTrackID())
	    )
	  {
	    LARCV_INFO() << "Storing secondary ROI (PDG " << secondary_roi.PdgCode()
			 << " Shape " << secondary_roi.Shape()
			 << " MCTIndex " << secondary_roi.MCTIndex()
			 << " MCSTIndex) " << secondary_roi.MCSTIndex()
			 << std::endl;	
	    roi2mcnode_v.clear();
	    roi2mcnode_v.emplace_back(primary_idx,secondary_idx);
	    _roi_v.emplace_back(std::move(secondary_roi));
	    _roi2mcnode_vv.emplace_back(std::move(roi2mcnode_v));
	  }
      }
    }

    if(_store_roi)
      ev_roi->Emplace(std::move(_roi_v));
    
    return true;
  }

  bool SuperaMCROI::FilterNode(const supera::MCNode& node) const
  {
    if(node.source_type == supera::MCNode::SourceType_t::kMCTruth) {
      if(_pass_origin && node.origin != _pass_origin)
	return false;
    }else if(node.source_type == supera::MCNode::SourceType_t::kMCTrack) {
      auto const& mctrack = LArData<supera::LArMCTrack_t>()[node.source_index];
      LARCV_DEBUG() << "MCTrack InitE " << mctrack.Start().E() << " ... DepE "
		    << (mctrack.size() >1 ? mctrack.front().E() - mctrack.back().E() : 0) << std::endl;
      if(mctrack.Start().E() < _track_min_einit) return false;
      if(_track_min_edep > 0) {
	if(mctrack.size()<2) return false;
	if( (mctrack.front().E() - mctrack.back().E()) < _track_min_edep ) return false;
      }
      for(size_t filter_idx=0; filter_idx<_filter_pdg.size(); ++filter_idx) {
	auto const& pdg = _filter_pdg[filter_idx];
	if(pdg != mctrack.PdgCode()) continue;
	auto const& filter_min_einit = _filter_min_einit[filter_idx];
	if(mctrack.Start().E() < filter_min_einit) return false;
	auto const& filter_min_edep = _filter_min_edep[filter_idx];
	if(filter_min_edep > 0) {
	  if(mctrack.size()<2) return false;
	  if( (mctrack.front().E() - mctrack.back().E()) < filter_min_edep ) return false;
	}
      }
    }else if(node.source_type == supera::MCNode::SourceType_t::kMCShower) {
      auto const& mcshower = LArData<supera::LArMCShower_t>()[node.source_index];
      LARCV_DEBUG() << "MCShower InitE " << mcshower.Start().E() << " ... DepE "
		    << mcshower.DetProfile().E() << std::endl;
      if(mcshower.Start().E() < _shower_min_einit) return false;
      if(mcshower.DetProfile().E() < _shower_min_edep) return false;
      for(size_t filter_idx=0; filter_idx<_filter_pdg.size(); ++filter_idx) {
	auto const& pdg = _filter_pdg[filter_idx];
	if(pdg != mcshower.PdgCode()) continue;
	auto const& filter_min_einit = _filter_min_einit[filter_idx];
	if(mcshower.Start().E() < filter_min_einit) return false;
	auto const& filter_min_edep = _filter_min_edep[filter_idx];
	if( mcshower.DetProfile().E() < filter_min_edep) return false;
      }
    }
    return true;
  }

  larcv::ROI SuperaMCROI::MakeROI(const supera::MCNode& node,
				   const std::vector<supera::LArSimCh_t>& sch_v) const
  {
    larcv::ROI res;
    if(node.source_type == supera::MCNode::SourceType_t::kMCTruth)
      throw larbys("MCTruth type cannot make ROI using MCROIMaker!");
    else if(node.source_type == supera::MCNode::SourceType_t::kMCTrack) {
      auto const& mctrack = LArData<supera::LArMCTrack_t>().at(node.source_index);
      if(sch_v.empty()) res = _mcroi_maker.ParticleROI(mctrack,TimeOffset());
      else res = _mcroi_maker.ParticleROI(mctrack,sch_v,TimeOffset());
      res.MCSTIndex(node.source_index);
    }
    else if(node.source_type == supera::MCNode::SourceType_t::kMCShower) {
      auto const& mcshower = LArData<supera::LArMCShower_t>().at(node.source_index);
      if(sch_v.empty()) res = _mcroi_maker.ParticleROI(mcshower,TimeOffset());
      else res = _mcroi_maker.ParticleROI(mcshower,sch_v,TimeOffset());
      res.MCSTIndex(node.source_index);
    }else
      throw larbys("Unexpected SourceType_t!");

    // format ROI
    std::vector<larcv::ImageMeta> bb_v;
    for(size_t plane=0; plane<res.BB().size(); ++plane) {
      auto const& part_meta  = res.BB().at(plane);
      auto const& event_meta = Meta().at(plane);
      bb_v.push_back(FormatMeta(part_meta,event_meta));
    }
    res.SetBB(bb_v);
    return res;
  }
  
  void SuperaMCROI::UpdatePrimaryROI(larcv::ROI& pri_roi,
				      std::vector<larcv::ROI>& sec_roi_v) const
  {
    LARCV_DEBUG() << "start" << std::endl;

    std::map<larcv::PlaneID_t, larcv::ImageMeta> sum_roi_m;
    double energy_deposit = 0;
    // register primary BB
    for (auto const& bb : pri_roi.BB()) {
      if(pri_roi.EnergyDeposit()>0) energy_deposit += pri_roi.EnergyDeposit();
      if (!(bb.height() > 1 && bb.width() > 1)) continue;
      auto iter = sum_roi_m.find(bb.plane());
      if (iter == sum_roi_m.end())
	sum_roi_m[bb.plane()] = bb;
      else
	(*iter).second = (*iter).second.inclusive(bb);
    }

    // Next, secondary BB
    for (auto& sec_roi : sec_roi_v) {
      energy_deposit += sec_roi.EnergyDeposit();
      // loop over plane-by-plane ImageMeta
      for (auto const& bb : sec_roi.BB()) {
	if (!(bb.height() > 1 && bb.width() > 1)) continue;
	auto iter = sum_roi_m.find(bb.plane());
	if (iter == sum_roi_m.end())
	  sum_roi_m[bb.plane()] = bb;
	else
	  (*iter).second = (*iter).second.inclusive(bb);
      }
      sec_roi.MCTIndex(pri_roi.MCTIndex());
    }    
    
    std::vector<larcv::ImageMeta> bb_v;
    bb_v.reserve(sum_roi_m.size());

    for (auto const& plane_roi : sum_roi_m) {
      if(bb_v.size() <= plane_roi.first)
	bb_v.resize(plane_roi.first+1);
      bb_v[plane_roi.first] = plane_roi.second;
      LARCV_INFO() << "Updated primary ROI plane " << plane_roi.first << " ... " << plane_roi.second.dump();
    }
    
    pri_roi.EnergyDeposit(energy_deposit);
    
    pri_roi.SetBB(bb_v);
  }

  larcv::ImageMeta SuperaMCROI::FormatMeta(const larcv::ImageMeta& part_image,
					   const larcv::ImageMeta& event_image) const
  {

    LARCV_DEBUG() << "Before format  " << part_image.dump();

    const size_t modular_row = event_image.height() / event_image.rows();
    const size_t modular_col = event_image.width()  / event_image.cols();
    
    if (event_image.rows() < modular_row || event_image.cols() < modular_col) {
      LARCV_ERROR() << "Event image too small to format ROI!" << std::endl;
      throw larbys();
    }
    double min_x  = (part_image.min_x() < event_image.min_x() ? event_image.min_x() : part_image.min_x() );
    double max_y  = (part_image.max_y() > event_image.max_y() ? event_image.max_y() : part_image.max_y() );
    double width  = (part_image.width() + min_x > event_image.max_x() ? event_image.max_x() - min_x : part_image.width());
    double height = (max_y - part_image.height() < event_image.min_y() ? max_y - event_image.min_y() : part_image.height());
    size_t rows   = height / part_image.pixel_height();
    size_t cols   = width  / part_image.pixel_width();

    if (modular_col > 1 && cols % modular_col) {
      int npixels = (modular_col - (cols % modular_col));
      if (event_image.width() < (width + npixels * part_image.pixel_width()))  npixels -= modular_col;
      cols += npixels;
      width += part_image.pixel_width() * npixels;
      if (npixels > 0) {
	// If expanded, make sure it won't go across event_image boundary
	if ( (min_x + width) > event_image.max_x() ) {
	  LARCV_DEBUG() << "X: " << min_x << " => " << min_x + width
		       << " exceeds event boundary " << event_image.max_x() << std::endl;
	  min_x = event_image.max_x() - width;
	} else if (min_x < event_image.min_x()) {
	  LARCV_DEBUG() << "X: " << min_x << " => " << min_x + width
		       << " exceeds event boundary " << event_image.min_x() << std::endl;
	  min_x = event_image.min_x();
	}
      }
    }

    if (modular_row > 1 && rows % modular_row) {
      int npixels = (modular_row - (rows % modular_row));
      if (event_image.height() < (height + npixels * part_image.pixel_height()))  npixels -= modular_row;
      rows += npixels;
      height += part_image.pixel_height() * npixels;
      if (npixels > 0) {
	// If expanded, make sure it won't go across event_image boundary
	if ( (max_y - height) < event_image.min_y() ) {
	  LARCV_DEBUG() << "Y: " << max_y - height << " => " << max_y
		       << " exceeds event boundary " << event_image.min_y() << std::endl;
	  max_y = event_image.min_y() + height;
	} else if (max_y > event_image.max_y()) {
	  LARCV_DEBUG() << "Y: " << max_y - height << " => " << max_y
		       << " exceeds event boundary " << event_image.max_y() << std::endl;
	  max_y = event_image.max_y();
	}
      }
    }
    LARCV_INFO() << "Creating ImageMeta Width=" << width
		 << " Height=" << height
		 << " NRows=" << rows / modular_row
		 << " NCols=" << cols / modular_col
		 << " Origin @ (" << min_x << "," << max_y << ")" << std::endl;
    larcv::ImageMeta res(width, height,
			 rows / modular_row, cols / modular_col,
			 min_x, max_y,
			 part_image.plane());

    LARCV_DEBUG() << "Event image   " << event_image.dump();

    LARCV_DEBUG() << "After format  " << res.dump();
    /*
    res = event_image.overlap(res);

    LARCV_DEBUG() << "After overlap " << res.dump();
    */
    return res;
  }
    
  void SuperaMCROI::finalize()
  {}

}
#endif
