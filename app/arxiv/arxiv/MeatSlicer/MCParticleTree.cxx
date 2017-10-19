#ifndef MCPARTICLETREE_CXX
#define MCPARTICLETREE_CXX

#include "MCParticleTree.h"

namespace supera {

  bool MCRoot::is_daughter(const size_t& parent_id) const
  {
    if(track_id == parent_id) return true;
    for(auto const& daughter : daughter_v)
      if(daughter.track_id == parent_id) return true;
    return false;
  }

  bool MCRoot::is_daughter(const larcv::Vertex& daughter_start) const
  {
    if(start == daughter_start || end == daughter_start) return true;
    for(auto const& daughter : daughter_v)
      if(daughter.start == daughter_start || daughter.end == daughter_start) return true;
    return false;
  }
  
  double MCRoot::dt(const MCNode& node) const
  {
    double min_dt = -1;
    double dt=0;

    dt = node.start.T() - start.T();
    if(dt>0 && (min_dt<0 || dt < min_dt)) min_dt = dt;
    dt = node.start.T() - end.T();
    if(dt>0 && (min_dt<0 || dt < min_dt)) min_dt = dt;

    for(auto const& daughter : daughter_v) {
      dt = node.start.T() - daughter.start.T();
      if(dt>0 && (min_dt<0 || dt < min_dt)) min_dt = dt;
      dt = node.start.T() - daughter.end.T();
      if(dt>0 && (min_dt<0 || dt < min_dt)) min_dt = dt;
    }
    return min_dt;
  }

  size_t MCParticleTree::FindPrimary(const larcv::Vertex& start,
				     const size_t parent_id,
				     const size_t ancestor_id) const
  {
    if(parent_id != larcv::kINVALID_SIZE) {
      for(size_t primary_idx=0; primary_idx<_primary_v.size(); ++primary_idx) {
	auto const& primary = _primary_v[primary_idx];
	if(primary.is_daughter(parent_id)) {
	  LARCV_DEBUG() << "Primary found by parent id " << parent_id << std::endl;
	  return primary_idx;
	}
      }
    }
    if(ancestor_id != larcv::kINVALID_SIZE) {
      for(size_t primary_idx=0; primary_idx<_primary_v.size(); ++primary_idx) {
	auto const& primary = _primary_v[primary_idx];
	if(primary.is_daughter(ancestor_id)) {
	  LARCV_DEBUG() << "Primary found by ancestor id " << ancestor_id << std::endl;
	  return primary_idx;
	}
      }
    }
    for(size_t primary_idx=0; primary_idx<_primary_v.size(); ++primary_idx) {
      auto const& primary = _primary_v[primary_idx];
      //if(node.source_type != primary.source_type) continue;
      if(primary.is_daughter(start)) {
	LARCV_DEBUG() << "Primary found by start vertex @ ("
		      << start.X() << "," << start.Y() << "," << start.Z() << ")"
		      << " ... primary bounds: ("
		      << primary.start.X() << "," << primary.start.Y() << "," << primary.start.Z() << ")"
		      << " => ("
		      << primary.end.X() << "," << primary.end.Y() << "," << primary.end.Z() << ")"
		      << std::endl;
	return primary_idx;
      }
    }
    return larcv::kINVALID_SIZE;
  }

  size_t MCParticleTree::FindPrimary(const size_t parent_id,
				     const size_t ancestor_id) const
  {
    if(parent_id != larcv::kINVALID_SIZE) {
      for(size_t primary_idx=0; primary_idx<_primary_v.size(); ++primary_idx) {
	auto const& primary = _primary_v[primary_idx];
	if(primary.is_daughter(parent_id)) {
	  LARCV_DEBUG() << "Primary found via parent id " << parent_id << std::endl;
	  return primary_idx;
	}
      }
    }
    if(ancestor_id != larcv::kINVALID_SIZE) {
      for(size_t primary_idx=0; primary_idx<_primary_v.size(); ++primary_idx) {
	auto const& primary = _primary_v[primary_idx];
	if(primary.is_daughter(ancestor_id)) {
	  LARCV_DEBUG() << "Primary found via ancestor id " << ancestor_id << std::endl;
	  return primary_idx;
	}
      }
    }
    return larcv::kINVALID_SIZE;
  }
  
  void MCParticleTree::Register(const std::vector<supera::LArMCTruth_t>&  mctruth_v,
				const std::vector<supera::LArMCTrack_t>&  mctrack_v,
				const std::vector<supera::LArMCShower_t>& mcshower_v)
  {
    size_t old_size = _primary_v.size();
    _primary_v.clear();
    _primary_v.reserve(old_size);
    if(_used_mctrack_v.size() <= mctrack_v.size())
      _used_mctrack_v.resize(mctrack_v.size());
    if(_used_mcshower_v.size() <= mcshower_v.size())
      _used_mcshower_v.resize(mcshower_v.size());

    for(auto& used : _used_mctrack_v ) used = 0;
    for(auto& used : _used_mcshower_v) used = 0;

    //
    // Register primaries
    //
    if(!_use_g4_primary) {
      for (size_t mctruth_index = 0; mctruth_index < mctruth_v.size(); ++mctruth_index) {
	
	auto const& mctruth = mctruth_v[mctruth_index];

	if(_origin_filter && mctruth.Origin() != _origin_filter)
	  continue;
	
	if(mctruth.Origin() == 1) // neutrino
	  DefineNeutrinoPrimary(mctruth_v);
      }
      DefinePrimary(mctruth_v);
    }else
      DefinePrimary(mctrack_v,mcshower_v);

    //
    // Register secondaries
    //
    DefineSecondary(mctrack_v,mcshower_v);
    EstimateSecondary(mctrack_v,mcshower_v);

    //
    // Report
    //
    if(logger().level() <= larcv::msg::kINFO) {
      std::stringstream ss;
      for(size_t primary_idx=0; primary_idx<_primary_v.size(); ++primary_idx) {

	auto const& primary = _primary_v[primary_idx];
	
	ss << "      Primary " << primary_idx << " Source " << (int)(primary.source_type) << " @ " << primary.source_index
	   << " ... PDG " << primary.pdg << " TrackID " << primary.track_id
	   << " with " << primary.daughter_v.size() << " children"
	   << std::endl;

	for(size_t child_idx=0; child_idx<primary.daughter_v.size(); ++child_idx){

	  auto const& node = primary.daughter_v[child_idx];
	  
	  ss << "          Child " << child_idx << " Source " << (int)(node.source_type) << " @ " << node.source_index
	     << " ... PDG " << node.pdg << " TrackID " << node.track_id << std::endl;
	  
	}
	ss<<std::endl;
      }
      LARCV_INFO() << "Particle tree summary..." << std::endl << ss.str() << std::endl;
    }
    
  }
    
  void MCParticleTree::DefineNeutrinoPrimary(const std::vector<supera::LArMCTruth_t>& mctruth_v)
  {
    //
    // In case of neutrino generator, we set neutrino as primary
    //
    for (size_t mctruth_idx=0; mctruth_idx<mctruth_v.size(); ++mctruth_idx) {

      auto const& mctruth = mctruth_v[mctruth_idx];

      if(_origin_filter && mctruth.Origin() != _origin_filter) continue;
      
      for (size_t i = 0; i < (size_t)(mctruth.NParticles()); ++i) {
	
	auto const& mcp = mctruth.GetParticle(i);
	// ignore if not neutrino
	if ( mcp.PdgCode() != 12 && mcp.PdgCode() != -12 &&
	     mcp.PdgCode() != 14 && mcp.PdgCode() != -14 &&
	     mcp.PdgCode() != 16 && mcp.PdgCode() != -16 )
	  continue;
	if (mcp.Mother() != -1) continue; // non-primary neutrino (e.g. after scattering)
	
	auto const& pos = mcp.Position(0);
	auto const& mom = mcp.Momentum(0);
	::larcv::Vertex vtx(pos.X(), pos.Y(), pos.Z(), pos.T());
	
	MCRoot root_node;
	root_node.start = vtx;
	root_node.end   = vtx;
	root_node.pdg   = mcp.PdgCode();
	root_node.source_index = i;
	root_node.source_type = MCNode::SourceType_t::kMCTruth;
	root_node.origin = (unsigned short)(mctruth.Origin());
	
	root_node.roi.EnergyDeposit(0);
	root_node.roi.EnergyInit(0);
	root_node.roi.Position(vtx);
	root_node.roi.PdgCode(mcp.PdgCode());
	root_node.roi.Momentum(mom.X()*1.e3, mom.Y()*1.e3, mom.Z()*1.e3);
	root_node.roi.EnergyInit(mom.E() * 1.e3);
	root_node.roi.MCTIndex(mctruth_idx);
	if(mctruth.NeutrinoSet()) {
	  root_node.roi.NuCurrentType(mctruth.GetNeutrino().CCNC());
	  root_node.roi.NuInteractionType(mctruth.GetNeutrino().InteractionType());
	}
	
	_primary_v.emplace_back(std::move(root_node));
      }
    }
  }

  void MCParticleTree::DefinePrimary(const std::vector<supera::LArMCTruth_t>& mctruth_v)
  {
    //
    // In case of neutrino generator, we set neutrino as primary
    //
    for (size_t mctruth_idx=0; mctruth_idx<mctruth_v.size(); ++mctruth_idx) {
      
      auto const& mctruth = mctruth_v[mctruth_idx];

      if(_origin_filter && mctruth.Origin() != _origin_filter) continue;
      
      for (size_t i = 0; i < (size_t)(mctruth.NParticles()); ++i) {
	
	auto const& mcp = mctruth.GetParticle(i);

	// only keep primary
	if(mcp.Mother() != mcp.TrackId() &&  mcp.Mother() != -1)
	  continue;
	  
	auto const& pos = mcp.Position(0);
	auto const& mom = mcp.Momentum(0);
	::larcv::Vertex vtx(pos.X(), pos.Y(), pos.Z(), pos.T());

	auto const primary_idx = FindPrimary(vtx, mcp.Mother(), mcp.Mother());
	if(primary_idx == larcv::kINVALID_SIZE) {
	  MCRoot root_node;
	  
	  root_node.start = vtx;
	  root_node.end   = vtx;
	  root_node.pdg   = mcp.PdgCode();
	  root_node.source_index = i;
	  root_node.source_type  = MCNode::SourceType_t::kMCTruth;
	  root_node.origin = (unsigned short)(mctruth.Origin());
	  
	  root_node.roi.EnergyDeposit(0);
	  root_node.roi.EnergyInit(0);
	  root_node.roi.Position(vtx);
	  root_node.roi.PdgCode(mcp.PdgCode());
	  root_node.roi.Momentum(mom.X()*1.e3, mom.Y()*1.e3, mom.Z()*1.e3);
	  root_node.roi.EnergyInit(mom.E() * 1.e3);
	  root_node.roi.MCTIndex(mctruth_idx);
	  _primary_v.emplace_back(std::move(root_node));
	}
      }
    }
  }

  void MCParticleTree::DefinePrimary(const std::vector<supera::LArMCTrack_t>& mctrack_v,
				     const std::vector<supera::LArMCShower_t>& mcshower_v)
  {
    for(size_t track_idx=0; track_idx<mctrack_v.size(); ++track_idx) {
      auto const& mctrack = mctrack_v[track_idx];
      if(_origin_filter && mctrack.Origin() != _origin_filter) continue;
      if(mctrack.TrackID() != mctrack.MotherTrackID()) continue;
      LARCV_INFO() << "Registering Primary MCTrack PDG " << mctrack.PdgCode()
		   << " G4 Track " << mctrack.TrackID() << " Mother Track " << mctrack.MotherTrackID()
		   << " Origin " << mctrack.Origin() << std::endl;
      ::larcv::Vertex vtx( mctrack.Start().X(), mctrack.Start().Y(), mctrack.Start().Z(), mctrack.Start().T() );
      auto node = FillNode(mctrack);
      node.source_index = track_idx;
      _primary_v.emplace_back(std::move(node));
      _used_mctrack_v[track_idx] = 1;
    }

    for(size_t shower_idx=0; shower_idx<mcshower_v.size(); ++shower_idx) {
      auto const& mcshower = mcshower_v[shower_idx];
      if(_origin_filter && mcshower.Origin() != _origin_filter) continue;
      if(mcshower.TrackID() != mcshower.MotherTrackID()) continue;
      LARCV_INFO() << "Registering Primary MCShower PDG " << mcshower.PdgCode()
		   << " G4 Track " << mcshower.TrackID() << " Mother Track " << mcshower.MotherTrackID()
		   << " Origin " << mcshower.Origin() << std::endl;
      ::larcv::Vertex vtx( mcshower.Start().X(), mcshower.Start().Y(), mcshower.Start().Z(), mcshower.Start().T() );
      auto node = FillNode(mcshower);
      node.source_index = shower_idx;
      _primary_v.emplace_back(std::move(node));
      _used_mcshower_v[shower_idx] = 1;
    }
  }

  MCNode MCParticleTree::FillNode(const supera::LArMCTrack_t& mct)
  {
    MCNode node;
    node.start.Reset(mct.Start().X(),mct.Start().Y(),mct.Start().Z(),mct.Start().T());
    node.end.Reset(mct.End().X(),mct.End().Y(),mct.End().Z(),mct.End().T());
    node.track_id = mct.TrackID();
    node.source_type  = MCNode::SourceType_t::kMCTrack;
    node.origin = (unsigned short)(mct.Origin());
    node.pdg = mct.PdgCode();
    return node;
  }

  MCNode MCParticleTree::FillNode(const supera::LArMCShower_t& mcs)
  {
    MCNode node;
    node.start.Reset(mcs.Start().X(),mcs.Start().Y(),mcs.Start().Z(),mcs.Start().T());
    node.end.Reset(mcs.End().X(),mcs.End().Y(),mcs.End().Z(),mcs.End().T());
    node.track_id = mcs.TrackID();
    node.source_type  = MCNode::SourceType_t::kMCShower;
    node.origin = (unsigned short)(mcs.Origin());
    node.pdg = mcs.PdgCode();
    return node;
  }

  void MCParticleTree::DefineSecondary(const std::vector<supera::LArMCTrack_t>&  mctrack_v,
				       const std::vector<supera::LArMCShower_t>& mcshower_v)
  {
    size_t used_count_mcshower = 0;
    size_t used_count_mctrack  = 0;
    for(auto const& v : _used_mctrack_v ) if(v) ++used_count_mctrack;
    for(auto const& v : _used_mcshower_v) if(v) ++used_count_mcshower;
    size_t last_used_count_mctrack  = larcv::kINVALID_SIZE;
    size_t last_used_count_mcshower = larcv::kINVALID_SIZE;

    while(used_count_mcshower != last_used_count_mcshower ||
	  used_count_mctrack  != last_used_count_mctrack) {

      // Update "last" counter
      last_used_count_mctrack  = used_count_mctrack;
      last_used_count_mcshower = used_count_mcshower;

      // Scan tracks for exact parentage connection
      for(size_t track_idx=0; track_idx<mctrack_v.size(); ++track_idx) {
	if(_used_mctrack_v[track_idx]) continue;
	auto const& mct = mctrack_v[track_idx];
	if(_origin_filter && mct.Origin() != _origin_filter) continue;
	auto node = FillNode(mct);
	node.source_index=track_idx;
	size_t primary_idx = larcv::kINVALID_SIZE;
	if(!_use_g4_primary) 
	  primary_idx = FindPrimary(node.start, mct.MotherTrackID(), mct.AncestorTrackID());
	else
	  primary_idx = FindPrimary(mct.MotherTrackID(), mct.AncestorTrackID());
	if(primary_idx == larcv::kINVALID_SIZE)
	  continue;
	LARCV_INFO() << "Associating MCTrack (index " << track_idx
		     << " PDG " << mct.PdgCode() << " Origin " << mct.Origin()
		     << ") with primary (index " << primary_idx
		     << " PDG " << _primary_v[primary_idx].roi.PdgCode()
		     << " Origin " << _primary_v[primary_idx].origin
		     << ")" << std::endl;
	_primary_v[primary_idx].daughter_v.emplace_back(std::move(node));
	_used_mctrack_v[track_idx] = 1;
      }

      // Scan showers for exact parentage connection
      for(size_t shower_idx=0; shower_idx<mcshower_v.size(); ++shower_idx) {
	if(_used_mcshower_v[shower_idx]) continue;
	auto const& mcs = mcshower_v[shower_idx];
	if(_origin_filter && mcs.Origin() != _origin_filter) continue;
	auto node = FillNode(mcs);
	node.source_index=shower_idx;
	size_t primary_idx = larcv::kINVALID_SIZE;
	if(!_use_g4_primary) 
	  primary_idx = FindPrimary(node.start, mcs.MotherTrackID(), mcs.AncestorTrackID());
	else
	  primary_idx = FindPrimary(mcs.MotherTrackID(), mcs.AncestorTrackID());
	if(primary_idx == larcv::kINVALID_SIZE)
	  continue;
	LARCV_INFO() << "Associating MCShower (index " << shower_idx
		     << " PDG " << mcs.PdgCode() << " Origin " << mcs.Origin()
		     << ") with primary (index " << primary_idx
		     << " PDG " << _primary_v[primary_idx].roi.PdgCode()
		     << " Origin " << _primary_v[primary_idx].origin
		     << ")" << std::endl;
	_primary_v[primary_idx].daughter_v.emplace_back(std::move(node));
	_used_mcshower_v[shower_idx] = 1;
      }

      // Update "current" counter
      used_count_mcshower = 0;
      used_count_mctrack  = 0;
      for(auto const& v : _used_mctrack_v ) if(v) ++used_count_mctrack;
      for(auto const& v : _used_mcshower_v) if(v) ++used_count_mcshower;

    }
  }
  
  void MCParticleTree::EstimateSecondary(const std::vector<supera::LArMCTrack_t>&  mctrack_v,
					 const std::vector<supera::LArMCShower_t>& mcshower_v)
  {
    if(_dt_max <=0) return;
    
    double primary_min_time = 1.e20;
    double primary_max_time = 0.;
    for(auto const& primary : _primary_v) {
      if(primary.start.T() < primary_min_time) primary_min_time = primary.start.T();
      if(primary.end.T() > primary_max_time) primary_max_time = primary.end.T();
    }

    for(size_t track_idx=0; track_idx<mctrack_v.size(); ++track_idx) {
      if(_used_mctrack_v[track_idx]) continue;
      auto const& track = mctrack_v[track_idx];
      if(_origin_filter && track.Origin() != _origin_filter) continue;
      auto node = FillNode(track);
      node.source_index = track_idx;
      if(node.start.T() < primary_min_time) {
	LARCV_INFO() << "Ignoring MCTrack (track id " << node.track_id << ", pdg " << track.PdgCode()
		     << ") as it comes before any primary in time "
		     << " (this time " << node.start.T() << " ... primary " << primary_min_time
		     << " => " << primary_max_time << ")"
		     << std::endl;
	continue;
      }
      size_t primary_idx = larcv::kINVALID_SIZE;
      double min_dt = 1e20;
      for(size_t idx=0; idx<_primary_v.size(); ++idx) {
	auto const& primary = _primary_v[idx];
	auto const dt = primary.dt(node);
	if(dt<0) continue;
	if(dt < min_dt) {
	  min_dt = dt;
	  primary_idx = idx;
	}
      }
      if(min_dt > _dt_max) continue;
      LARCV_INFO() << "Associating (time-approx) MCTrack (index " << track_idx
		   << " PDG " << track.PdgCode() << " Origin " << track.Origin()
		   << ") with primary (index " << primary_idx
		   << " PDG " << _primary_v[primary_idx].roi.PdgCode()
		   << " Origin " << _primary_v[primary_idx].origin
		   << ")" << std::endl;
      _primary_v[primary_idx].daughter_v.emplace_back(std::move(node));
      _used_mctrack_v[track_idx] = 1;
    }

    for(size_t shower_idx=0; shower_idx<mcshower_v.size(); ++shower_idx) {
      if(_used_mcshower_v[shower_idx]) continue;
      auto const& shower = mcshower_v[shower_idx];
      if(_origin_filter && shower.Origin() != _origin_filter) continue;
      auto node = FillNode(shower);
      node.source_index = shower_idx;
      if(node.start.T() < primary_min_time) {
	LARCV_INFO() << "Ignoring MCShower (track id " << node.track_id << ", pdg " << shower.PdgCode()
		     << ") as it comes before any primary in time "
		     << " (this time " << node.start.T() << " ... primary " << primary_min_time
		     << " => " << primary_max_time << ")"
		     << std::endl;
	continue;
      }
      size_t primary_idx = larcv::kINVALID_SIZE;
      double min_dt = 1e20;
      for(size_t idx=0; idx<_primary_v.size(); ++idx) {
	auto const& primary = _primary_v[idx];
	auto const dt = primary.dt(node);
	if(dt<0) continue;
	if(dt < min_dt) {
	  min_dt = dt;
	  primary_idx = idx;
	}
      }
      LARCV_INFO() << "Associating (time-approx) MCShower (index " << shower_idx
		   << " PDG " << shower.PdgCode() << " Origin " << shower.Origin()
		   << ") with primary (index " << primary_idx
		   << " PDG " << _primary_v[primary_idx].roi.PdgCode()
		   << " Origin " << _primary_v[primary_idx].origin
		   << ")" << std::endl;
      if(min_dt > _dt_max) continue;
      _primary_v[primary_idx].daughter_v.emplace_back(std::move(node));
      _used_mcshower_v[shower_idx] = 1;
    }
  }
  
}

#endif
