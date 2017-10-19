#ifndef __SUPERAKEYPOINTCLUSTER_CXX__
#define __SUPERAKEYPOINTCLUSTER_CXX__

#include "SuperaKeyPointCluster.h"
#include "ImageMetaMakerFactory.h"
#include "PulledPork3DSlicer.h"
#include "DataFormat/EventPixel2D.h"
#include "DataFormat/EventROI.h"

namespace larcv {

  static SuperaKeyPointClusterProcessFactory __global_SuperaKeyPointClusterProcessFactory__;

  SuperaKeyPointCluster::SuperaKeyPointCluster(const std::string name)
    : SuperaBase(name)
  {}
    
  void SuperaKeyPointCluster::configure(const PSet& cfg)
  {
    SuperaBase::configure(cfg);
    supera::ParamsPixel2D::configure(cfg);
    supera::ImageMetaMaker::configure(cfg);

    _in_roi_label = cfg.get<std::string>("InputROILabel");
    _apply_sce    = cfg.get<bool>("ApplySCE");
    _row_pad      = cfg.get<size_t>("RowPad");
    _col_pad      = cfg.get<size_t>("ColPad");

    _cluster_primary_start   = cfg.get<bool>("UsePrimaryStart",true);
    _cluster_secondary_start = cfg.get<bool>("UseSecondaryStart",true);
    _cluster_scattering      = cfg.get<bool>("UseScattering",false);
  }

  void SuperaKeyPointCluster::initialize()
  { SuperaBase::initialize(); }

  larcv::Vertex SuperaKeyPointCluster::GetPoint(const supera::LArMCStep_t& step)
  {
    static double xyz[3];
    static const double drift_velocity = ::supera::DriftVelocity() * 1.0e-3; // make it cm/ns
    xyz[0] = step.X();
    xyz[1] = step.Y();
    xyz[2] = step.Z();
    if(_apply_sce) supera::ApplySCE(xyz);
      
    larcv::Vertex pt(xyz[0],xyz[1],xyz[2],
		     (double)(step.T() + (xyz[0] / drift_velocity)));
    
    return pt;
  }

  bool SuperaKeyPointCluster::process(IOManager& mgr)
  {
    SuperaBase::process(mgr);

    if(supera::PulledPork3DSlicer::Is(supera::ImageMetaMaker::MetaMakerPtr())) {
      auto ptr = (supera::PulledPork3DSlicer*)(supera::ImageMetaMaker::MetaMakerPtr());
      ptr->ClearEventData();
      ptr->AddConstraint(LArData<supera::LArMCTruth_t>());
      ptr->GenerateMeta(LArData<supera::LArSimCh_t>(),TimeOffset());
    }

    auto ev_pixel2d = (EventPixel2D*)(mgr.get_data(kProductPixel2D,OutPixel2DLabel()));
    if(!ev_pixel2d) {
      LARCV_CRITICAL() << "Failed to create Pixel2D output data container w/ label " << OutPixel2DLabel() << std::endl;
      throw larbys();
    }

    auto ev_roi = (EventROI*)(mgr.get_data(kProductROI,_in_roi_label));
    if(!ev_roi) {
      LARCV_CRITICAL() << "Failed to fetch an input ROI by " << _in_roi_label << std::endl;
      throw larbys();
    }

    std::set<larcv::Vertex> primary_start_s;
    std::set<larcv::Vertex> secondary_start_s;
    std::set<larcv::Vertex> scattering_s;

    for(auto const& mctrack : LArData<supera::LArMCTrack_t>()) {

      if(mctrack.size()<2) continue;

      auto start = GetPoint(mctrack.Start());
      if(mctrack.TrackID() == mctrack.MotherTrackID()) {
	if(_cluster_primary_start) {
	  LARCV_INFO() << "Primary MCTrack (PDG " << mctrack.PdgCode() << ") start @ ("
		       << start.X() << "," << start.Y() << "," << start.Z() << ")"
		       << " @ G4 Time = " << start.T() << " [ns]" << std::endl;
	  primary_start_s.emplace(std::move(start));
	}
      }
      else {
	if(_cluster_secondary_start) {
	  LARCV_INFO() << "Secondary MCTrack (PDG " << mctrack.PdgCode() << ") start @ ("
		       << start.X() << "," << start.Y() << "," << start.Z() << ")"
		       << " @ G4 Time = " << start.T() << " [ns]" << std::endl;
	  secondary_start_s.emplace(std::move(start));
	}
      }
      if(_cluster_scattering) {
	for(auto const& step : mctrack) {
	  if(step.T() == mctrack.Start().T()) continue;
	  auto pt = GetPoint(step);
	  LARCV_INFO() << "Scattering @ ("
		       << pt.X() << "," << pt.Y() << "," << pt.Z() << ")"
		       << " @ G4 Time = " << pt.T() << " [ns]" << std::endl;
	  scattering_s.emplace(std::move(pt));
	}
      }
    }

    for(auto const& mcshower : LArData<supera::LArMCShower_t>()) {

      if(mcshower.DetProfile().E()<=0) continue;

      auto start = GetPoint(mcshower.Start());
      
      if(mcshower.TrackID() == mcshower.MotherTrackID()) {
	if(_cluster_primary_start) {
	  LARCV_INFO() << "Primary MCShower (PDG " << mcshower.PdgCode() << ") start @ ("
		       << start.X() << "," << start.Y() << "," << start.Z() << ")"
		       << " @ G4 Time = " << start.T() << " [ns]" << std::endl;
	  primary_start_s.emplace(std::move(start));
	}
      }
      else {
	if(_cluster_secondary_start) {
	  LARCV_INFO() << "Primary MCShower (PDG " << mcshower.PdgCode() << ") start @ ("
		       << start.X() << "," << start.Y() << "," << start.Z() << ")"
		       << " @ G4 Time = " << start.T() << " [ns]" << std::endl;
	  secondary_start_s.emplace(std::move(start));
	}
      }
      if(_cluster_secondary_start) {
	if(mcshower.Start().T() != mcshower.DetProfile().T()) {
	  auto pt = GetPoint(mcshower.DetProfile());
	  LARCV_INFO() << "DetProfile @ ("
		       << pt.X() << "," << pt.Y() << "," << pt.Z() << ")"
		       << " @ G4 Time = " << pt.T() << " [ns]" << std::endl;
	  secondary_start_s.emplace(std::move(pt));
	}
      }
    }

    auto meta_v = Meta();
    for(size_t plane=0; plane<meta_v.size(); ++plane) {
      
      auto& meta = meta_v[plane];
      meta.update(meta.rows() / RowCompressionFactor().at(plane),
		  meta.cols() / ColCompressionFactor().at(plane));

    }
    
    const unsigned short primary_start_val   = 3;
    const unsigned short secondary_start_val = 2;
    const unsigned short scattering_val      = 1;
    LARCV_INFO() << "Creating cluster for primary start from " << primary_start_s.size() << " 3D points." << std::endl;
    auto primary_start_pcluster_v   = CreateCluster( meta_v, primary_start_s,   primary_start_val,   TimeOffset() );
    LARCV_INFO() << "Creating cluster for secondary start from " << secondary_start_s.size() << " 3D points." << std::endl;
    auto secondary_start_pcluster_v = CreateCluster( meta_v, secondary_start_s, secondary_start_val, TimeOffset() );
    LARCV_INFO() << "Creating cluster for scattering points from " << scattering_s.size() << " 3D points." << std::endl;
    auto scattering_pcluster_v      = CreateCluster( meta_v, scattering_s,      scattering_val,      TimeOffset() );

    for(size_t plane=0; plane<meta_v.size(); ++plane) {

      auto const& meta = meta_v[plane];
      ev_pixel2d->Emplace( plane, std::move( primary_start_pcluster_v.at(plane)   ), meta );
      ev_pixel2d->Emplace( plane, std::move( secondary_start_pcluster_v.at(plane) ), meta );
      ev_pixel2d->Emplace( plane, std::move( scattering_pcluster_v.at(plane)      ), meta );

    }
    return true;
  }

  std::vector<larcv::Pixel2DCluster>
  SuperaKeyPointCluster::CreateCluster(const std::vector<larcv::ImageMeta>& meta_v,
				       const std::set<larcv::Vertex>& pt_s,
				       const unsigned short val,
				       const int time_offset)
  {

    double xyz[3];
    std::vector<larcv::Pixel2DCluster> res_v;
    res_v.resize(meta_v.size());

    static std::vector<std::vector<unsigned short> > data_v;
    data_v.resize(meta_v.size());

    for(size_t plane=0; plane<meta_v.size(); ++plane) {
      LARCV_DEBUG() << "Clustering plane " << plane << std::endl;
      auto const& meta = meta_v[plane];
      auto& data = data_v[plane];
      data.resize(meta.rows() * meta.cols());
      for(auto& v : data) v = 0.;

      for(auto const& pt : pt_s) {
	xyz[0] = pt.X();
	xyz[1] = pt.Y();
	xyz[2] = pt.Z();
	double tick_float = (double)(::supera::TPCG4Time2Tick(pt.T()) + time_offset) + supera::PlaneTickOffset(0,plane) + 0.5;
	double wire_float = (double)(::supera::NearestWire(xyz, plane)) + 0.5;
	LARCV_DEBUG() << wire_float << " ... " << meta.min_x() << " => " << meta.max_x() << std::endl;
	LARCV_DEBUG() << tick_float << " ... " << meta.min_y() << " => " << meta.max_y() << std::endl;
	if(wire_float < meta.min_x() || meta.max_x() < wire_float) continue;
	if(tick_float < meta.min_y() || meta.max_y() < tick_float) continue;
	
	auto tick = meta.row(tick_float);
	auto wire = meta.col(wire_float);

	LARCV_DEBUG() << "Masking around (tick,wire) = (" << tick << "," << wire << ")" << std::endl;
	
	for(int row=((int)tick - (int)_row_pad); row<=((int)tick + (int)(_row_pad)); ++row) {
	  for(int col=((int)wire - (int)_col_pad); col<=((int)wire + (int)_col_pad); ++col) {
	    if(row<0 || row >= (int)(meta.rows())) continue;
	    if(col<0 || col >= (int)(meta.cols())) continue;
	    LARCV_DEBUG() << "    Registering (row,col) = (" << row << "," << col << ") w/ value " << val << std::endl;
	    auto index = meta.index((size_t)row,(size_t)col);
	    if(data[index] < val) data[index] = val;
	  }
	}
      }

      auto& res = res_v[plane];
      for(size_t index=0; index<data.size(); ++index) {

	if(data[index]<1) continue;

	size_t col = index / meta.rows();
	size_t row = index % meta.rows();

	LARCV_DEBUG() << "Clustering pixel (row,col) = (" << row << "," << col << ")" << std::endl;

	larcv::Pixel2D px2d(col,row);
	px2d.Intensity(data[index]);
	res.emplace_back(std::move(px2d));
      }

      LARCV_INFO() << "Clustered " << res.size() << " points @ plane " << plane << std::endl;
    }
    return res_v;
  }
  
  void SuperaKeyPointCluster::finalize()
  { SuperaBase::finalize(); }

}
#endif
