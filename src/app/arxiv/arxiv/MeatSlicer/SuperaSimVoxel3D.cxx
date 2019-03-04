#ifndef __SUPERASIMVOXEL3D_CXX__
#define __SUPERASIMVOXEL3D_CXX__

#include "SuperaSimVoxel3D.h"
#include "LAr2Image.h"
#include "ImageMetaMakerFactory.h"
#include "PulledPork3DSlicer.h"
#include "DataFormat/EventVoxel3D.h"
#include "DataFormat/DataFormatUtil.h"
namespace larcv {

  static SuperaSimVoxel3DProcessFactory __global_SuperaSimVoxel3DProcessFactory__;

  SuperaSimVoxel3D::SuperaSimVoxel3D(const std::string name)
    : SuperaBase(name)
  {}
    
  void SuperaSimVoxel3D::configure(const PSet& cfg)
  {
    SuperaBase::configure(cfg);
    supera::ParamsVoxel3D::configure(cfg);
    supera::ImageMetaMaker::configure(cfg);
    _origin = cfg.get<unsigned short>("Origin",0);
    _voxel_size = cfg.get<double>("VoxelSize",0.9);
    _target_plane = cfg.get<size_t>("TargetPlane",2);
    _t0_tick = cfg.get<size_t>("T0Tick",3200);
  }

  void SuperaSimVoxel3D::initialize()
  {}

  bool SuperaSimVoxel3D::process(IOManager& mgr)
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
    auto ev_voxel3d = (EventVoxel3D*)(mgr.get_data(kProductVoxel3D,OutVoxel3DLabel()));
    if(!ev_voxel3d) {
      LARCV_CRITICAL() << "Output voxel3d could not be created!" << std::endl;
      throw larbys();
    }

    std::vector<int> track_v;
    for(auto const& mctrack : LArData<supera::LArMCTrack_t>()) {

      if(_origin && ((unsigned short)(mctrack.Origin())) != _origin) continue;
      
      if(mctrack.TrackID() >= track_v.size())
	track_v.resize(mctrack.TrackID()+1,0);
      track_v[mctrack.TrackID()] = 1;

      if(mctrack.MotherTrackID() >= track_v.size())
	track_v.resize(mctrack.MotherTrackID()+1,0);
      track_v[mctrack.MotherTrackID()] = 1;

      if(mctrack.AncestorTrackID() >= track_v.size())
	track_v.resize(mctrack.AncestorTrackID()+1,0);
      track_v[mctrack.AncestorTrackID()] = 1;
    }
    for(auto const& mcshower : LArData<supera::LArMCShower_t>()) {

      if(_origin && ((unsigned short)(mcshower.Origin())) != _origin) continue;
      
      if(mcshower.TrackID() >= track_v.size())
	track_v.resize(mcshower.TrackID()+1,0);
      track_v[mcshower.TrackID()] = 1;

      if(mcshower.MotherTrackID() >= track_v.size())
	track_v.resize(mcshower.MotherTrackID()+1,0);
      track_v[mcshower.MotherTrackID()] = 1;
      
      if(mcshower.AncestorTrackID() >= track_v.size())
	track_v.resize(mcshower.AncestorTrackID()+1,0);
      track_v[mcshower.AncestorTrackID()] = 1;

      for(auto const& daughter_track_id : mcshower.DaughterTrackID()) {
	if(daughter_track_id == larcv::kINVALID_UINT)
	  continue;
	if(daughter_track_id >= track_v.size())
	  track_v.resize(daughter_track_id+1,0);
	track_v[daughter_track_id] = 1;
      }
    }

    //
    // Create Meta
    //
    // 0) get z range from plane2's wire range
    double zmin = meta_v.at(2).min_x() * supera::WirePitch(_target_plane);
    double zmax = meta_v.at(2).max_x() * supera::WirePitch(_target_plane);
    double ymin = -1 * supera::DetHalfHeight();
    double ymax = supera::DetHalfHeight();
    /*
    std::cout<<meta_v.at(2).min_y() << " : " << supera::TPCTickPeriod() << " : " << supera::TriggerOffsetTPC() << " : " << supera::DriftVelocity() << std::endl;
    std::cout<<meta_v.at(2).max_y() << " : " << supera::TPCTickPeriod() << " : " << supera::TriggerOffsetTPC() << " : " << supera::DriftVelocity() << std::endl;
    */
    double xmin = (meta_v.at(2).min_y() - _t0_tick) * supera::TPCTickPeriod() * supera::DriftVelocity();
    double xmax = (meta_v.at(2).max_y() - _t0_tick) * supera::TPCTickPeriod() * supera::DriftVelocity();
    Voxel3DMeta meta;
    meta.Set(xmin, xmax, ymin, ymax, zmin, zmax, (xmax-xmin)/_voxel_size, (ymax-ymin)/_voxel_size, (zmax-zmin)/_voxel_size);

    LARCV_INFO() << "Target Voxel3DMeta" << std::endl
		 << meta.Dump()
		 << std::endl;
    
    auto voxel3dset = supera::SimCh2Voxel3D(meta, track_v,
					    LArData<supera::LArSimCh_t>(),
					    TimeOffset() + supera::TriggerOffsetTPC() / supera::TPCTickPeriod(),
					    _target_plane);

    ev_voxel3d->Move(std::move(voxel3dset));
    
    return true;
  }

  void SuperaSimVoxel3D::finalize()
  {}

}
#endif
