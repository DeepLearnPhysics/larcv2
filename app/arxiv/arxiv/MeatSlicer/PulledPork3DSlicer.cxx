#ifndef __PULLEDPORK3DSLICER_CXX__
#define __PULLEDPORK3DSLICER_CXX__

#include "PulledPork3DSlicer.h"
#include "LAr2Image.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/DataFormatUtil.h"
#include <cmath>

namespace supera {

  void PulledPork3DSlicer::configure(const supera::Config_t& cfg)
  {
    set_verbosity((larcv::msg::Level_t)(cfg.get<unsigned short>("Verbosity",logger().level())));
    _slicer.Verbosity((unsigned int)(logger().level()));

    LARCV_INFO() << std::endl;
    ImageMetaMakerBase::configure(cfg);
    
    _origin = cfg.get<unsigned short>("Origin",0);
    _slicer.Clear();
    LARCV_INFO() << std::endl;
    // Set the grid size by minimum wire spacing
    double grid_size_ymin = std::numeric_limits<double>::max();
    double grid_size_zmin = std::numeric_limits<double>::max();
    for(size_t plane=0; plane<supera::Nplanes(); ++plane) {
      int angle = std::abs((int)(supera::WireAngleToVertical(plane)/M_PI*180));
      angle = std::min(angle, 180 - angle);
      double grid_size_y = std::numeric_limits<double>::max();
      double grid_size_z = std::numeric_limits<double>::max();
      if(angle == 0) {
	grid_size_y = supera::WirePitch(plane);
      }else if(angle == 90){
	grid_size_z = supera::WirePitch(plane);
      }else {
	grid_size_y = std::fabs(supera::WirePitch(plane) / cos((double)(angle) / 180. * M_PI));
	grid_size_z = std::fabs(grid_size_y / tan((double)(angle) / 180. * M_PI));
      }
      if(grid_size_y < grid_size_ymin) grid_size_ymin = grid_size_y;
      if(grid_size_z < grid_size_zmin) grid_size_zmin = grid_size_z;
    }
    _slicer.SetGridSize(supera::DriftVelocity()*supera::TPCTickPeriod(),grid_size_ymin,grid_size_zmin);
    // Target volume size
    auto width_v = cfg.get<std::vector<double> >("WidthArray");
    if(width_v.size()!=3) {
      LARCV_CRITICAL() << "Must provide WidthArray of size 3 for xyz width" << std::endl;
      throw std::exception();
    }
    if(width_v[0] < _slicer.GridSizeX()) {
      LARCV_CRITICAL() << "X width is smaller than grid size!" << std::endl;
      throw std::exception();
    }
    if(width_v[1] < _slicer.GridSizeY()) {
      LARCV_CRITICAL() << "Y width is smaller than grid size!" << std::endl;
      throw std::exception();
    }
    if(width_v[2] < _slicer.GridSizeZ()) {
      LARCV_CRITICAL() << "Z width is smaller than grid size!" << std::endl;
      throw std::exception();
    }
    _slicer.SetWidth(width_v[0],width_v[1],width_v[2]);
    // Fiducial volume min
    auto min_pt = cfg.get<std::vector<double> >("MinCoordinate");
    if(min_pt.size() != 3) {
      LARCV_CRITICAL() << "Must provide MinCoordinate of size 3 for xyz min. point" << std::endl;
      throw std::exception();
    }
    auto max_pt = cfg.get<std::vector<double> >("MaxCoordinate");
    if(max_pt.size() != 3) {
      LARCV_CRITICAL() << "Must provide MaxCoordinate of size 3 for xyz max. point" << std::endl;
      throw std::exception();
    }
    for(size_t i=0; i<3; ++i) {
      if(min_pt[i] < max_pt[i]) continue;
      LARCV_CRITICAL() << "MinCoordinate exceeds MaxCoordinate for coordinate index " << i << std::endl;
      throw std::exception();
    }
    _slicer.SetMin(min_pt[0],min_pt[1],min_pt[2]);
    _slicer.SetMax(max_pt[0],max_pt[1],max_pt[2]);

    // Padding
    auto padding_v = cfg.get<std::vector<size_t> >("Padding");
    if(padding_v.size() != 3) {
      LARCV_CRITICAL() << "Must provide Padding of size 3 for xyz max. point" << std::endl;
      throw std::exception();
    }
    _slicer.SetPadding(padding_v[0] * _slicer.GridSizeX(),
		       padding_v[1] * _slicer.GridSizeY(),
		       padding_v[2] * _slicer.GridSizeZ());

    // Artificial constraint
    std::vector<double> constraint_xv, constraint_yv, constraint_zv;
    constraint_xv = cfg.get<std::vector<double> > ("ConstraintX",constraint_xv);
    constraint_yv = cfg.get<std::vector<double> > ("ConstraintY",constraint_yv);
    constraint_zv = cfg.get<std::vector<double> > ("ConstraintZ",constraint_zv);
    if(constraint_xv.size() != constraint_yv.size() ||
       constraint_xv.size() != constraint_zv.size()) {
      LARCV_CRITICAL() << "Constraint XYZ does not match in size..." << std::endl;
      throw std::exception();
    }
    for(size_t i=0; i<constraint_xv.size(); ++i)
      _slicer.AddConstraint(constraint_xv[i],
			    constraint_yv[i],
			    constraint_zv[i]);

    // Target time pixels
    _time_pixel = cfg.get<size_t>("TimePixels");
    
    // Target wire pixels
    _wire_pixel_v = cfg.get<std::vector<size_t> >("WirePixels");
    if(_wire_pixel_v.size()!=supera::Nplanes()) {
      LARCV_CRITICAL() << "WirePixels parameter array length != # planes..." << std::endl;
      throw std::exception();
    }

    // Apply SCE
    _apply_sce = cfg.get<bool>("ApplySCE");

    // T0 in G4 time
    _t0_g4ns = cfg.get<double>("T0G4ns");

    LARCV_NORMAL() << _slicer.PrintConfig() << std::flush;

    supera::GridPoint3D tmp_min_pt;
    supera::GridPoint3D tmp_max_pt;
    std::vector<supera::GridPoint3D> tmp_point_v;
    _slicer.DeriveRange(tmp_point_v,tmp_min_pt,tmp_max_pt);
    
    if(!this->Test()) {
      LARCV_CRITICAL() << "Meta test generation failed @ configuration end..." << std::endl;
      throw std::exception();
    }

  }

  bool PulledPork3DSlicer::Test() const
  {
    // Test by adding fake points @ middle
    std::vector<supera::GridPoint3D> points_v;
    auto meta_v = DeriveMeta(points_v,0);

    if(meta_v.size() != supera::Nplanes())
      LARCV_CRITICAL() << "Failed on test meta generation!" << std::endl;
    else
      LARCV_NORMAL() << "Generated test meta..." << std::endl;
    for(auto const& meta : meta_v)
      LARCV_NORMAL() << meta.dump() << std::flush;
    
    return (meta_v.size() == supera::Nplanes());
  }

  void PulledPork3DSlicer::AddConstraint(const std::vector<supera::LArMCTruth_t>& mctruth_v) {
    for(auto const& mct : mctruth_v)
      this->AddConstraint(mct);
  }

  void PulledPork3DSlicer::AddConstraint(const supera::LArMCTruth_t& mctruth) {

    if(_origin > 0 && mctruth.Origin() != _origin) {
      LARCV_INFO() << "Skipping to add a constraint for origin " << mctruth.Origin() 
		   << " (target " << _origin << ")" << std::endl;
      return;
    }

    LARCV_INFO() << "Searching for a constraint from " << mctruth.NParticles() 
		 << " particles in one MCTruth..." << std::endl;
    for(int i=0; i<mctruth.NParticles(); ++i) {

      auto const& mcp = mctruth.GetParticle(i);
      
      if(mcp.StatusCode() != 1) {
	LARCV_INFO() << "Skipping PDG " << mcp.PdgCode() 
		     << " @ index " << i 
		     << " as the status code is " << mcp.StatusCode() << std::endl;
	continue;
      }

      auto const& pos = mcp.Position(0);
      double x = pos.X();
      double y = pos.Y();
      double z = pos.Z();

      LARCV_INFO() << "Adding a constraint by PDG " << mcp.PdgCode() 
		   << " @ index " << i 
		   << " @ (x,y,z) = (" << x << "," << y << "," << z << ")" << std::endl;

      // apply SCE if configured so
      if(_apply_sce) supera::ApplySCE(x,y,z);

      // shift X coodrinate for T0 correction
      x += (pos.T() - _t0_g4ns) / 1.e3 * supera::DriftVelocity();
      
      _slicer.AddConstraint(x,y,z);
    }
  }

  void PulledPork3DSlicer::ClearEventData()
  {
    _slicer.Clear();
    _meta_v.clear();
  }

  void PulledPork3DSlicer::GenerateMeta(const std::vector<supera::LArSimCh_t>& simch_v,
					const int time_offset)
  {
    std::vector<int> trackid_v;
    GenerateMeta(simch_v,time_offset,trackid_v,false);
  }

  void PulledPork3DSlicer::GenerateMeta(const std::vector<supera::LArSimCh_t>& simch_v,
					const int  time_offset,
					const std::vector<int>& trackid_v)
  { GenerateMeta(simch_v,time_offset,trackid_v,true); }
    



  void PulledPork3DSlicer::GenerateMeta(const std::vector<supera::LArSimCh_t>& simch_v,
					const int time_offset,
					const std::vector<int>& trackid_v,
					const bool use_track_id)
  {
    LARCV_INFO() << _slicer.PrintConfig() << std::flush;
	
    // Retrieve boundaries
    auto const& min_grid = _slicer.EffectiveMin();
    auto const& max_grid = _slicer.EffectiveMax();

    // Get xyz range: note, this is NOT A POINT, don't apply SCE
    // (if _apply_sce is true, this region already takes that into account)
    double xmax = max_grid.x * _slicer.GridSizeX();
    double ymax = max_grid.y * _slicer.GridSizeY();
    double zmax = max_grid.z * _slicer.GridSizeZ();
    double xmin = min_grid.x * _slicer.GridSizeX();
    double ymin = min_grid.y * _slicer.GridSizeY();
    double zmin = min_grid.z * _slicer.GridSizeZ();

    // Convenient conversion factor
    const double tdc2x = supera::TPCTickPeriod() * supera::DriftVelocity();
    // Being lazy, use std::set for a unique set of points
    std::set<supera::GridPoint3D> point_s;
    // Loop over sim channel and register relevant points
    for (auto const& sch : simch_v) {
      for (auto const tdc_ides : sch.TDCIDEMap()) {

	// Check tdc: this is effectively checking X in image coordinate
	double xpos = (tdc_ides.first - supera::TPCG4Time2TDC(_t0_g4ns)) * tdc2x;
	if(xpos < xmin || xpos > xmax) continue;

	for(auto const& edep : tdc_ides.second) {
	  // Check y/z
	  if(edep.y < ymin || edep.y > ymax) continue;
	  if(edep.z < zmin || edep.z > zmax) continue;
	  if(use_track_id &&
	     std::abs(edep.trackID) < trackid_v.size() &&
	     trackid_v[std::abs(edep.trackID)] <=0)
	    continue;
	  // Register
	  point_s.insert(_slicer.GridPoint3D(xpos,edep.y,edep.z));
	}
      }
    }

    // Now derive range
    std::vector<supera::GridPoint3D> point_v;
    point_v.reserve(point_s.size());
    for(auto const& pt : point_s) point_v.push_back(pt);
    
    _meta_v = DeriveMeta(point_v,time_offset);
  }

  std::vector<larcv::ImageMeta>
  PulledPork3DSlicer::DeriveMeta(const std::vector<supera::GridPoint3D>& point_v,
				 const int time_offset) const {

    supera::GridPoint3D min_pt;
    supera::GridPoint3D max_pt;
    _slicer.DeriveRange(point_v,min_pt,max_pt);

    std::vector<std::vector<double> > edge_v(4,std::vector<double>(3,0.));
    edge_v[0][1] = min_pt.y * _slicer.GridSizeY();
    edge_v[0][2] = min_pt.z * _slicer.GridSizeZ();
    edge_v[1][1] = min_pt.y * _slicer.GridSizeY();
    edge_v[1][2] = max_pt.z * _slicer.GridSizeZ();
    edge_v[2][1] = max_pt.y * _slicer.GridSizeY();
    edge_v[2][2] = max_pt.z * _slicer.GridSizeZ();
    edge_v[3][1] = max_pt.y * _slicer.GridSizeY();
    edge_v[3][2] = min_pt.z * _slicer.GridSizeZ();
    
    // Figure out wire range
    std::vector<std::pair<int,int> > wire_range_v(supera::Nplanes());
    for(auto& wire_range : wire_range_v) {
      wire_range.first  = std::numeric_limits<int>::max();
      wire_range.second = std::numeric_limits<int>::min();
    }
    for(auto const& edge_pt : edge_v) {
      for(size_t plane=0; plane<supera::Nplanes(); ++plane) {
	int wire = supera::NearestWire(&edge_pt[0],plane);
	auto& wire_range = wire_range_v[plane];
	if(wire < wire_range.first)  wire_range.first  = wire;
	if(wire > wire_range.second) wire_range.second = wire;
      }
    }
    // Clean up edge effects
    for(size_t plane=0; plane<supera::Nplanes(); ++plane) {
      auto& wire_range = wire_range_v[plane];
      auto const& pixel_count = _wire_pixel_v[plane];
      // Check if target pixel is within 1 tick
      int pixel_count_raw = wire_range.second - wire_range.first + 1;
      int pixel_count_offset = (int)(pixel_count) - pixel_count_raw;
      if(std::abs(pixel_count_offset) > 1) {
	LARCV_CRITICAL() << "Plane " << plane
		  << " wire range " << wire_range.first
		  << " => " << wire_range.second
		  << " does not match expected pixel count " << pixel_count
		  << std::endl;
	throw std::exception();
      }
      LARCV_INFO() << "Plane " << plane
		   << " wire range " << wire_range.first
		   << " => " << wire_range.second
		   << " good match for expected pixel count " << pixel_count
		   << std::endl;
      // If pixel count does not match exactly, extend
      if(pixel_count_offset > 0) {
	bool extend_to_lower = ( wire_range.first > ((int)(supera::Nwires(plane)) - wire_range.second) );
	if(extend_to_lower)
	  wire_range.first -= pixel_count_offset;
	else
	  wire_range.second += pixel_count_offset;
      }
      if(pixel_count_offset < 0) {
	bool shrink_from_lower = ( wire_range.first > ((int)(supera::Nwires(plane)) - wire_range.second) );
	if(shrink_from_lower)
	  wire_range.first -= pixel_count_offset;
	else
	  wire_range.second += pixel_count_offset;
      }
    }
    
    // Tick conversion from X
    //int tick_start = (int)((min_pt.x / supera::DriftVelocity() - supera::TriggerOffsetTPC()) / supera::TPCTickPeriod() + 0.5) + time_offset;
    //int tick_end   = (int)((max_pt.x / supera::DriftVelocity() - supera::TriggerOffsetTPC()) / supera::TPCTickPeriod() + 0.5) + time_offset;
    int tick_start = min_pt.x + time_offset - supera::TriggerOffsetTPC() / supera::TPCTickPeriod();
    int tick_end   = max_pt.x + time_offset - supera::TriggerOffsetTPC() / supera::TPCTickPeriod();

    LARCV_INFO() << "X range: " << min_pt.x << " => " << max_pt.x
		 << " converted to " << tick_start << " => " << tick_end << std::endl;

    // Clean up edge effect
    int tick_count_offset = _time_pixel - (tick_end - tick_start + 1);
    if( std::abs(tick_count_offset) > 1 ) {
      LARCV_CRITICAL() << "Time tick range " << tick_start
		       << " => " << tick_end 
		       << " does not match expected pixel count " << _time_pixel << std::endl;
      throw std::exception();
    }
    if( tick_count_offset>0 ) tick_end -= tick_count_offset;
    if( tick_count_offset<0 ) tick_end += tick_count_offset;

    std::vector<larcv::ImageMeta> meta_v;
    for(size_t plane=0; plane<supera::Nplanes(); ++plane) {
      auto const& wire_range = wire_range_v[plane];
      LARCV_INFO() << "Creating ImageMeta Width=" << (double)(wire_range.second - wire_range.first + 1)
		   << " Height=" << (double)(tick_end - tick_start + 1)
		   << " NRows=" << (size_t)(tick_end - tick_start + 1)
		   << " NCols=" << (size_t)(wire_range.second - wire_range.first + 1)
		   << " Origin @ (" << (double)(wire_range.first) << "," << (double)(tick_end) << ")" << std::endl;
      meta_v.emplace_back((double)(wire_range.second - wire_range.first + 1),
			  (double)(tick_end - tick_start + 1),
			  (size_t)(tick_end - tick_start + 1),
			  (size_t)(wire_range.second - wire_range.first + 1),
			  (double)(wire_range.first),
			  (double)(tick_end),
			  (larcv::PlaneID_t)(plane));
      LARCV_INFO() << "...done on plane " << plane << std::endl;
    }
    return meta_v;
  }

}
#endif
