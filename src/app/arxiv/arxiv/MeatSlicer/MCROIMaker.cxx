#ifndef __SUPERA_MCROIMAKER_CXX__
#define __SUPERA_MCROIMAKER_CXX__

#include "MCROIMaker.h"
#include "Base/larbys.h"
#include "FMWKInterface.h"
#include <TLorentzVector.h> // ROOT
#include <set>
namespace supera {

  void MCROIMaker::configure(const supera::Config_t& cfg)
  {
    LARCV_DEBUG() << "start" << std::endl;
    set_verbosity((::larcv::msg::Level_t)(cfg.get<unsigned short>("Verbosity",logger().level())));
    _max_time_tick = cfg.get<unsigned int>("MaxTimeTick");
    _time_padding  = cfg.get<unsigned int>("TimePadding");
    _wire_padding  = cfg.get<unsigned int>("WirePadding");
    _min_width     = cfg.get<unsigned int>("MinWidth");
    _min_height    = cfg.get<unsigned int>("MinHeight");
    _apply_sce     = cfg.get<bool>("ApplySCE");
    
    LARCV_NORMAL() << "Configuration called..." << std::endl
		   << " Padding   (wire,time) = (" << _wire_padding << "," << _time_padding << ")" << std::endl
		   << " Min. Size (wire,time) = (" << _min_width << "," << _min_height << ")" << std::endl;
  }

  WTRangeArray_t MCROIMaker::WireTimeBoundary(const LArMCTrack_t& mct,
					      const std::vector<supera::LArSimCh_t>& sch_v) const
  {
    LARCV_DEBUG() << "start" << std::endl;
    // result is N planes' wire boundary + time boundary (N+1 elements)
    WTRangeArray_t result(supera::Nplanes() + 1);
    
    for (auto const& sch : sch_v) {
      
      auto const wire = ChannelToWireID(sch.Channel());
      auto& wrange = result[wire.Plane];
      auto& trange = result.back();
      
      for (auto const& tdc_ide_v : sch.TDCIDEMap()) {
	bool store = false;
	double tick = TPCTDC2Tick((double)(tdc_ide_v.first));
	//if(tick<0 || tick >= NumberTimeSamples()) continue;
	if (tick < 0 || tick >= _max_time_tick) continue;
	tick += 0.5;
	for (auto const& ide : tdc_ide_v.second) {
	  unsigned int trackid = (ide.trackID < 0 ? (-1 * ide.trackID) : ide.trackID);
	  if (trackid != mct.TrackID()) continue;
	  
	  store = true;
	  LARCV_DEBUG() << "IDE wire = " << (unsigned int)wire.Wire
		       << " ... tick = " << (unsigned int)tick << std::endl
		       << " (tdc=" << tdc_ide_v.first << ", x=" << ide.x << ")" << std::endl;
	  break;
	}
	if (!store) continue;
	if (!wrange.valid()) wrange.Set((unsigned int)wire.Wire, (unsigned int)(wire.Wire));
	else wrange += (unsigned int)(wire.Wire);
	if (!trange.valid()) trange.Set((unsigned int)(tick), (unsigned int)(tick));
	else trange += (unsigned int)(tick);
      }
    }
    
    for (auto& r : result) if (!r.valid()) r.Set(0, 0);
    
    for (size_t plane = 0; plane <= supera::Nplanes(); ++plane)
      
      LARCV_INFO() << "Single MCShower ... Plane " << plane
		   << " bound " << result[plane].Start() << " => " << result[plane].End() << std::endl;
    
    return result;
  }
  
  WTRangeArray_t MCROIMaker::WireTimeBoundary(const LArMCTrack_t& mct) const
  {
    LARCV_DEBUG() << "start" << std::endl;
    const double drift_velocity = ::supera::DriftVelocity() * 1.0e-3; // make it cm/ns
    //const int tick_max = ::supera::NumberTimeSamples();
    const double plane_tick_offset = PlaneTickOffset(0,1); // plane 1 as a reference
    const int tick_max = _max_time_tick;
    TVector3 xyz; xyz[0] = xyz[1] = xyz[2] = 0.;
    
    // result is N planes' wire boundary + time boundary (N+1 elements)
    WTRangeArray_t result(::supera::Nplanes() + 1);
    
    for (auto& step : mct) {

      xyz[0] = step.X();
      xyz[1] = step.Y();
      xyz[2] = step.Z();
      if(_apply_sce) ApplySCE(xyz);
      
      // Figure out time
      int tick = (int)(::supera::TPCG4Time2Tick(step.T() + (xyz[0]  / drift_velocity)) + plane_tick_offset + 0.5);
      
      if (tick < 0 || tick >= tick_max) {
	LARCV_DEBUG() << "tick out of time: " << tick << std::endl;
	continue;
      }

      auto& trange = result.back();
      if (!trange.valid()) trange.Set((unsigned int)tick, (unsigned int)tick); // 1st time: "set" it
      else trange += (unsigned int)tick; // >1st time: "add (include)" it
      
      // Figure out wire per plane
      LARCV_DEBUG() << "(x,t,v) = (" << xyz[0] << "," << step.T() << "," << drift_velocity << ") ... tick = " << tick << std::endl;
      for (size_t plane = 0; plane < supera::Nplanes(); ++plane) {
	
	auto wire_id = ::supera::NearestWire(xyz, plane);
	auto& wrange = result[plane];
	LARCV_DEBUG() << "(y,z) = (" << xyz[1] << "," << xyz[2] << ") ... @ plane " << plane << " wire = " << wire_id
		     << (wrange.valid() ? " updating wire-range" : " setting wire-range") << std::endl;
	if (!wrange.valid()) wrange.Set((unsigned int)wire_id, (unsigned int)wire_id);
	else wrange += (unsigned int)wire_id;
	
      }
    }
    
    for (auto& r : result) if (!r.valid()) r.Set(0, 0);
    //if(!r.valid() || (r.End() - r.Start()) < 2) r.Set(0,0);
    
    for (size_t plane = 0; plane <= supera::Nplanes(); ++plane)
      
      LARCV_INFO() << "Single MCTrack ... Plane " << plane
		   << " bound " << result[plane].Start() << " => " << result[plane].End() << std::endl;
    
    return result;
  }

  WTRangeArray_t MCROIMaker::WireTimeBoundary(const LArMCShower_t& mcs) const
  {
    LARCV_DEBUG() << "start" << std::endl;
    const double drift_velocity = ::supera::DriftVelocity() * 1.0e-3; // make it cm/ns
    //const int tick_max = ::supera::NumberTimeSamples();
    const int tick_max = _max_time_tick;
    double xyz[3] = {0.};
    
    // result is N planes' wire boundary + time boundary (N+1 elements)
    WTRangeArray_t result(::supera::Nplanes() + 1);
    
    auto const& detprofile = mcs.DetProfile();
    double energy = detprofile.E();
    
    double showerlength = 13.8874 + 0.121734 * energy - (3.75571e-05) * energy * energy;
    showerlength *= 2.5;
    //double showerlength = 100.0;
    double detprofnorm = sqrt( detprofile.Px() * detprofile.Px() + detprofile.Py() * detprofile.Py() + detprofile.Pz() * detprofile.Pz() );
    TLorentzVector showerend;
    const double plane_tick_offset = PlaneTickOffset(0,1); // plane 1 as a reference
    showerend[0] = detprofile.X() + showerlength * (detprofile.Px() / detprofnorm);
    showerend[1] = detprofile.Y() + showerlength * (detprofile.Py() / detprofnorm);
    showerend[2] = detprofile.Z() + showerlength * (detprofile.Pz() / detprofnorm);
    showerend[3] = detprofile.T();
    //std::cout << "showerlength: " << showerlength << " norm=" << detprofnorm << std::endl;
    
    std::vector< TLorentzVector > step_v;
    step_v.push_back( detprofile.Position() );
    step_v.push_back( showerend );
    
    for (auto& step : step_v) {
      xyz[0] = step.X();
      xyz[1] = step.Y();
      xyz[2] = step.Z();
      if(_apply_sce) ApplySCE(xyz);
      
      // Figure out time
      int tick = (int)(::supera::TPCG4Time2Tick(step.T() + (xyz[0]  / drift_velocity)) + plane_tick_offset + 0.5);
      
      if (tick < 0 || tick >= tick_max) continue;
      
      auto& trange = result.back();
      if (!trange.valid()) trange.Set((unsigned int)tick, (unsigned int)tick);
      else trange += (unsigned int)tick;
      
      // Figure out wire per plane
      for (size_t plane = 0; plane < supera::Nplanes(); ++plane) {
	
	auto wire_id = ::supera::NearestWire(xyz, plane);
	
	auto& wrange = result[plane];
	if (!wrange.valid()) wrange.Set((unsigned int)wire_id, (unsigned int)wire_id);
	else wrange += (unsigned int)wire_id;
      }
      
      LARCV_DEBUG() << "(x,t,v) = (" << xyz[0] << "," << step.T() << "," << drift_velocity << ") ... tick = " << tick << std::endl;
    }
    
    for (auto& r : result) if (!r.valid()) r.Set(0, 0);
    
    for (size_t plane = 0; plane <= supera::Nplanes(); ++plane)
      
      LARCV_INFO() << "Single MCShower ... Plane " << plane
		   << " bound " << result[plane].Start() << " => " << result[plane].End() << std::endl;
    
    return result;
  }
  
  WTRangeArray_t MCROIMaker::WireTimeBoundary(const LArMCShower_t& mcs, const std::vector<supera::LArSimCh_t>& sch_v) const
  {
    LARCV_DEBUG() << "start" << std::endl;
    // result is N planes' wire boundary + time boundary (N+1 elements)
    WTRangeArray_t result(::supera::Nplanes() + 1);
    
    std::set<unsigned int> daughters;
    for (auto const& trackid : mcs.DaughterTrackID()) daughters.insert(trackid);
    daughters.insert(mcs.TrackID());
    
    for (auto const& sch : sch_v) {
      
      auto const wire = ChannelToWireID(sch.Channel());
      auto& wrange = result[wire.Plane];
      auto& trange = result.back();
      
      for (auto const& tdc_ide_v : sch.TDCIDEMap()) {
	bool store = false;
	double tick = TPCTDC2Tick((double)(tdc_ide_v.first));
	//if(tick<0 || tick >= NumberTimeSamples()) continue;
	if (tick < 0 || tick >= _max_time_tick) continue;
	tick += 0.5;
	for (auto const& ide : tdc_ide_v.second) {
	  unsigned int trackid = (ide.trackID < 0 ? (-1 * ide.trackID) : ide.trackID);
	  if (daughters.find(trackid) != daughters.end()) {
	    store = true;
	    LARCV_DEBUG() << "IDE wire = " << (unsigned int)wire.Wire
			 << " ... tick = " << (unsigned int)tick << std::endl
			 << " (tdc=" << tdc_ide_v.first << ", x=" << ide.x << ")" << std::endl;
	    break;
	  }
	}
	if (!store) continue;
	if (!wrange.valid()) wrange.Set((unsigned int)wire.Wire, (unsigned int)(wire.Wire));
	else wrange += (unsigned int)(wire.Wire);
	if (!trange.valid()) trange.Set((unsigned int)(tick), (unsigned int)(tick));
	else trange += (unsigned int)(tick);
      }
    }
    
    for (auto& r : result) if (!r.valid()) r.Set(0, 0);
    
    for (size_t plane = 0; plane <= supera::Nplanes(); ++plane)
      
      LARCV_INFO() << "Single MCShower ... Plane " << plane
		   << " bound " << result[plane].Start() << " => " << result[plane].End() << std::endl;
    
    return result;
  }
  
  std::vector<larcv::ImageMeta> MCROIMaker::WTRange2BB(const WTRangeArray_t& wtrange_v) const
  {
    LARCV_DEBUG() << "start" << std::endl;
    std::vector<larcv::ImageMeta> bb_v;
    if (wtrange_v.empty()) return bb_v;
    
    bb_v.reserve(wtrange_v.size() - 1);
    bool invalid = false;
    for (size_t i = 0; i < wtrange_v.size() - 1; ++i) {
      auto const& wrange = wtrange_v[i];
      auto const& trange = wtrange_v.back();

      if(!wrange.valid() || !trange.valid()) {
	LARCV_INFO() << "Invalid WTRange_t for Plane " << i << std::endl
		     << "      W: " << wrange.Start() << " => " << wrange.End() << (wrange.valid() ? " good" : " bad")
		     << " ... T: " << trange.Start() << " => " << trange.End() << (trange.valid() ? " good" : " bad") << std::endl;
	invalid = true;
      }
      
      double width, height;
      double origin_x, origin_y;
      size_t rows, cols;
      width = (int)(wrange.End()) - (int)(wrange.Start());
      height = (int)(trange.End()) - (int)(trange.Start());
      origin_x = wrange.Start();

      origin_y = trange.End();
      rows = (int)(trange.End()) - (int)(trange.Start());
      cols = (int)(wrange.End()) - (int)(wrange.Start());
      //if(wrange.valid() && (wrange.End() - wrange.Start())) {
      if (wrange.valid() && wrange.Start()) {
	width = (int)(wrange.End()) - (int)(wrange.Start()) + 1 + 2 * ((int)_wire_padding);
	cols = (int)(wrange.End()) - (int)(wrange.Start()) + 1 + 2 * ((int)_wire_padding);
	origin_x = (double)(wrange.Start()) - (double)(_wire_padding);
      }
      //if(trange.valid() && (trange.End() - trange.Start())) {
      if (trange.valid() && trange.Start()) {
	height = (int)(trange.End()) - (int)(trange.Start()) + 1 + 2 * (int)(_time_padding);
	rows = (int)(trange.End()) - (int)(trange.Start()) + 1 + 2 * (int)(_time_padding);
	origin_y = (int)(trange.End()) + (int)(_time_padding);
      }
      
      if ( (width - 2 * _wire_padding) < _min_width || (height - 2 * _time_padding) < _min_height ) {
	LARCV_INFO() << "Too small WTRange_t for Plane " << i << std::endl
		     << "      W: " << wrange.Start() << " => " << wrange.End() << (wrange.valid() ? " good" : " bad")
		     << " ... T: " << trange.Start() << " => " << trange.End() << (trange.valid() ? " good" : " bad") << std::endl;
	invalid = true;
      } else {
	LARCV_INFO() << "Constructing ImageMeta from WTRange_t for Plane " << i
		     << " w/ padding (w,t) = (" << _wire_padding << "," << _time_padding << ")" << std::endl
		     << "      W: " << wrange.Start() << " => " << wrange.End() << (wrange.valid() ? " good" : " bad")
		     << " ... T: " << trange.Start() << " => " << trange.End() << (trange.valid() ? " good" : " bad") << std::endl
		     << "      Origin: (" << origin_x << "," << origin_y << ")" << std::endl
		     << "      Rows = " << rows << " Cols = " << cols << " ... Height = " << height << " Width = " << width << std::endl;
	bb_v.emplace_back(width, height, rows, cols, origin_x, origin_y, i);
      }
      
    }
    if(invalid) bb_v.clear();
    return bb_v;
  }
  
  ::larcv::ROI MCROIMaker::ParticleROI( const LArMCTrack_t& mct, const int time_offset) const
  {
    LARCV_DEBUG() << "start" << std::endl;
    LARCV_INFO() << "Assessing MCTrack G4Track ID = " << mct.TrackID() << " PdgCode " << mct.PdgCode() << std::endl;
    auto wtrange_v = WireTimeBoundary(mct);
    // Add time offset
    wtrange_v.back().Set(wtrange_v.back().Start()+time_offset, wtrange_v.back().End()+time_offset);
    auto bb_v = WTRange2BB(wtrange_v);

    double xyz[3] = {0.};
    
    ::larcv::ROI res;
    res.SetBB(bb_v);
    res.Shape(::larcv::kShapeTrack);
    res.Type(::larcv::PdgCode2ROIType(mct.PdgCode()));
    if (mct.size())
      res.EnergyDeposit(mct.front().E() - mct.back().E());
    else
      res.EnergyDeposit(0);
    res.EnergyInit(mct.Start().E());

    xyz[0] = mct.Start().X();
    xyz[1] = mct.Start().Y();
    xyz[2] = mct.Start().Z();
    if(_apply_sce) ApplySCE(xyz);
    res.Position(xyz[0], xyz[1], xyz[2], mct.Start().T());

    xyz[0] = mct.End().X();
    xyz[1] = mct.End().Y();
    xyz[2] = mct.End().Z();
    if(_apply_sce) ApplySCE(xyz);
    res.EndPosition(xyz[0], xyz[1], xyz[2], mct.End().T());
    
    res.CreationProcess(mct.Process());
    
    if(mct.size()>0) {
      auto const& first_step = mct.front();
      xyz[0] = first_step.X();
      xyz[1] = first_step.Y();
      xyz[2] = first_step.Z();
      if(_apply_sce) ApplySCE(xyz);
      res.FirstStep(xyz[0], xyz[1], xyz[2], first_step.T());
    }
    if(mct.size()>1) {
      auto const& last_step = mct.back();
      res.LastStep(last_step.X(), last_step.Y(), last_step.Z(), last_step.T());
      double length=0;
      for(size_t step_idx=1; step_idx<mct.size(); ++step_idx) {
	auto const& step1 = mct[step_idx-1];
	auto const& step2 = mct[step_idx];
	length += sqrt(pow(step1.X()-step2.X(),2) + pow(step1.Y()-step2.Y(),2) + pow(step1.Z()-step2.Z(),2));
      }
      res.DistanceTravel(length);
    }
    res.Momentum(mct.Start().Px(), mct.Start().Py(), mct.Start().Pz());
    res.PdgCode(mct.PdgCode());
    res.ParentPdgCode(mct.MotherPdgCode());
    res.TrackID(mct.TrackID());
    res.ParentTrackID(mct.MotherTrackID());

    xyz[0] = mct.MotherStart().X();
    xyz[1] = mct.MotherStart().Y();
    xyz[2] = mct.MotherStart().Z();
    if(_apply_sce) ApplySCE(xyz);
    
    res.ParentPosition(xyz[0], xyz[1], xyz[2], mct.MotherStart().T());
    
    res.ParentMomentum(mct.MotherStart().Px(),
		       mct.MotherStart().Py(),
		       mct.MotherStart().Pz());
    LARCV_INFO() << res.dump();
    return res;
  }

  ::larcv::ROI MCROIMaker::ParticleROI( const LArMCTrack_t& mct,
					const std::vector<supera::LArSimCh_t>& sch_v,
					const int time_offset ) const
  {
    LARCV_DEBUG() << "start" << std::endl;
    LARCV_INFO() << "Assessing MCTrack G4Track ID = " << mct.TrackID() << " PdgCode " << mct.PdgCode() << std::endl;
    auto wtrange_v = WireTimeBoundary(mct, sch_v);
    wtrange_v.back().Set(wtrange_v.back().Start()+time_offset, wtrange_v.back().End()+time_offset);
    auto bb_v = WTRange2BB(wtrange_v);

    double xyz[3] = {0.};
    
    ::larcv::ROI res;
    res.SetBB(bb_v);
    res.Shape(::larcv::kShapeTrack);
    res.Type(::larcv::PdgCode2ROIType(mct.PdgCode()));
    if (mct.size())
      res.EnergyDeposit(mct.front().E() - mct.back().E());
    else
      res.EnergyDeposit(0);
    res.EnergyInit(mct.Start().E());

    xyz[0] = mct.Start().X();
    xyz[1] = mct.Start().Y();
    xyz[2] = mct.Start().Z();
    if(_apply_sce) ApplySCE(xyz);
    res.Position(xyz[0], xyz[1], xyz[2], mct.Start().T());

    xyz[0] = mct.End().X();
    xyz[1] = mct.End().Y();
    xyz[2] = mct.End().Z();
    if(_apply_sce) ApplySCE(xyz);
    res.EndPosition(xyz[0], xyz[1], xyz[2], mct.End().T());

    res.CreationProcess(mct.Process());
    if(mct.size()>0) {
      auto const& first_step = mct.front();
      xyz[0] = first_step.X();
      xyz[1] = first_step.Y();
      xyz[2] = first_step.Z();
      if(_apply_sce) ApplySCE(xyz);
      res.FirstStep(xyz[0], xyz[1], xyz[2], first_step.T());
    }
    if(mct.size()>1) {
      auto const& last_step = mct.back();
      res.LastStep(last_step.X(), last_step.Y(), last_step.Z(), last_step.T());
      double length=0;
      for(size_t step_idx=1; step_idx<mct.size(); ++step_idx) {
	auto const& step1 = mct[step_idx-1];
	auto const& step2 = mct[step_idx];
	length += sqrt(pow(step1.X()-step2.X(),2) + pow(step1.Y()-step2.Y(),2) + pow(step1.Z()-step2.Z(),2));
      }
      res.DistanceTravel(length);
    }
    res.Momentum(mct.Start().Px(), mct.Start().Py(), mct.Start().Pz());
    res.PdgCode(mct.PdgCode());
    res.ParentPdgCode(mct.MotherPdgCode());
    res.TrackID(mct.TrackID());
    res.ParentTrackID(mct.MotherTrackID());

    xyz[0] = mct.MotherStart().X();
    xyz[1] = mct.MotherStart().Y();
    xyz[2] = mct.MotherStart().Z();
    if(_apply_sce) ApplySCE(xyz);
    
    res.ParentPosition(xyz[0], xyz[1], xyz[2], mct.MotherStart().T());
    res.ParentMomentum(mct.MotherStart().Px(),
		       mct.MotherStart().Py(),
		       mct.MotherStart().Pz());
    LARCV_INFO() << res.dump();
    return res;
  }
  
  ::larcv::ROI MCROIMaker::ParticleROI( const LArMCShower_t& mcs, const int time_offset ) const
  {
    LARCV_DEBUG() << "start" << std::endl;
    LARCV_INFO() << "Assessing MCShower G4Track ID = " << mcs.TrackID() << " PdgCode " << mcs.PdgCode() << std::endl;
    auto wtrange_v = WireTimeBoundary(mcs);
    wtrange_v.back().Set(wtrange_v.back().Start()+time_offset, wtrange_v.back().End()+time_offset);
    auto bb_v = WTRange2BB(wtrange_v);

    double xyz[3] = {0.};
    
    ::larcv::ROI res;
    res.SetBB(bb_v);
    res.Shape(::larcv::kShapeShower);
    res.Type(::larcv::PdgCode2ROIType(mcs.PdgCode()));
    res.EnergyDeposit(mcs.DetProfile().E());
    //res.EnergyDeposit(0);
    res.EnergyInit(mcs.Start().E());

    xyz[0] = mcs.Start().X();
    xyz[1] = mcs.Start().Y();
    xyz[2] = mcs.Start().Z();
    if(_apply_sce) ApplySCE(xyz);
    res.Position(xyz[0], xyz[1], xyz[2], mcs.Start().T());

    xyz[0] = mcs.End().X();
    xyz[1] = mcs.End().Y();
    xyz[2] = mcs.End().Z();
    if(_apply_sce) ApplySCE(xyz);
    res.EndPosition(xyz[0], xyz[1], xyz[2], mcs.End().T());

    res.CreationProcess(mcs.Process());
    
    auto const& first_step = mcs.DetProfile();
    xyz[0] = first_step.X();
    xyz[1] = first_step.Y();
    xyz[2] = first_step.Z();
    if(_apply_sce) ApplySCE(xyz);
    res.FirstStep(xyz[0], xyz[1], xyz[2], first_step.T());
    
    res.Momentum(mcs.Start().Px(), mcs.Start().Py(), mcs.Start().Pz());
    res.PdgCode(mcs.PdgCode());
    res.ParentPdgCode(mcs.MotherPdgCode());
    res.TrackID(mcs.TrackID());
    res.ParentTrackID(mcs.MotherTrackID());

    xyz[0] = mcs.MotherStart().X();
    xyz[1] = mcs.MotherStart().Y();
    xyz[2] = mcs.MotherStart().Z();
    if(_apply_sce) ApplySCE(xyz);
    
    res.ParentPosition(xyz[0], xyz[1], xyz[2], mcs.MotherStart().T());
    res.ParentMomentum(mcs.MotherStart().Px(),
		       mcs.MotherStart().Py(),
		       mcs.MotherStart().Pz());
    LARCV_INFO() << res.dump();
    return res;
  }
  
  ::larcv::ROI MCROIMaker::ParticleROI( const LArMCShower_t& mcs,
					const std::vector<LArSimCh_t>& sch_v,
					const int time_offset ) const
  {
    LARCV_DEBUG() << "start" << std::endl;
    LARCV_INFO() << "Assessing MCShower G4Track ID = " << mcs.TrackID() << " PdgCode " << mcs.PdgCode() << std::endl;
    auto wtrange_v = WireTimeBoundary(mcs, sch_v);
    wtrange_v.back().Set(wtrange_v.back().Start()+time_offset, wtrange_v.back().End()+time_offset);
    auto bb_v = WTRange2BB(wtrange_v);

    double xyz[3] = {0.};
    
    ::larcv::ROI res;
    res.SetBB(bb_v);
    res.Shape(::larcv::kShapeShower);
    res.Type(::larcv::PdgCode2ROIType(mcs.PdgCode()));
    res.EnergyDeposit(mcs.DetProfile().E());
    //res.EnergyDeposit(0);
    res.EnergyInit(mcs.Start().E());

    xyz[0] = mcs.Start().X();
    xyz[1] = mcs.Start().Y();
    xyz[2] = mcs.Start().Z();
    if(_apply_sce) ApplySCE(xyz);
    res.Position(xyz[0], xyz[1], xyz[2], mcs.Start().T());

    xyz[0] = mcs.End().X();
    xyz[1] = mcs.End().Y();
    xyz[2] = mcs.End().Z();
    if(_apply_sce) ApplySCE(xyz);
    res.EndPosition(xyz[0], xyz[1], xyz[2], mcs.End().T());
    
    res.CreationProcess(mcs.Process());

    auto const& first_step = mcs.DetProfile();
    xyz[0] = first_step.X();
    xyz[1] = first_step.Y();
    xyz[2] = first_step.Z();
    if(_apply_sce) ApplySCE(xyz);
    res.FirstStep(xyz[0], xyz[1], xyz[2], first_step.T());
    
    res.Momentum(mcs.Start().Px(), mcs.Start().Py(), mcs.Start().Pz());
    res.PdgCode(mcs.PdgCode());
    res.ParentPdgCode(mcs.MotherPdgCode());
    res.TrackID(mcs.TrackID());
    res.ParentTrackID(mcs.MotherTrackID());

    xyz[0] = mcs.MotherStart().X();
    xyz[1] = mcs.MotherStart().Y();
    xyz[2] = mcs.MotherStart().Z();
    if(_apply_sce) ApplySCE(xyz);
    res.ParentPosition(xyz[0], xyz[1], xyz[2], mcs.MotherStart().T());
		       
    res.ParentMomentum(mcs.MotherStart().Px(),
		       mcs.MotherStart().Py(),
		       mcs.MotherStart().Pz());
    LARCV_INFO() << res.dump();
    return res;
  }

}
#endif

// Local Variables:
// mode: c++
// End:
