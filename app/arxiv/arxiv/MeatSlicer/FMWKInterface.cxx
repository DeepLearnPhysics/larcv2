#ifndef __FMWKINTERFACE_CXX__
#define __FMWKINTERFACE_CXX__

#include "FMWKInterface.h"
#include "LArUtil/InvalidWireError.h"
#include "LArUtil/DetectorProperties.h"
#include "LArUtil/LArProperties.h"
#include "LArUtil/TimeService.h"
#include "LArUtil/SpaceChargeMicroBooNE.h"

namespace supera {
  
  ::larlite::geo::WireID ChannelToWireID(unsigned int ch)
  { return ::larutil::Geometry::GetME()->ChannelToWireID(ch); }
  
  double DriftVelocity()
  { return ::larutil::LArProperties::GetME()->DriftVelocity(); }
  
  unsigned int NumberTimeSamples()
  {
    // Till resolved, do not use this function
    throw ::larcv::larbys("NumberTimeSamples function not available!");
    //return ::larutil::DetectorProperties::GetME()->NumberTimeSamples();
    //return 9600;
  }

  double DetHalfWidth()
  { return ::larutil::Geometry::GetME()->DetHalfWidth(); }

  double DetHalfHeight()
  { return ::larutil::Geometry::GetME()->DetHalfHeight(); }
  
  double DetLength()
  { return ::larutil::Geometry::GetME()->DetLength(); }
  
  unsigned int Nchannels()
  { return ::larutil::Geometry::GetME()->Nchannels(); }
  
  unsigned int Nplanes()
  { return ::larutil::Geometry::GetME()->Nplanes(); }
  
  unsigned int Nwires(unsigned int plane)
  { return ::larutil::Geometry::GetME()->Nwires(plane); }
  
  unsigned int NearestWire(const TVector3& xyz, unsigned int plane)
  {
    /*
      unsigned int res;
      try{
      res = ::larutil::Geometry::GetME()->NearestWire(xyz,plane);
      }catch( ::larutil::InvalidWireError& err){
      res = err.better_wire_number;
      }
    */
    double min_wire=0;
    double max_wire=Nwires(plane)-1;
    double wire = ::larutil::Geometry::GetME()->WireCoordinate(xyz,plane) + 0.5;
    if(wire<min_wire) wire = min_wire;
    if(wire>max_wire) wire = max_wire;
    
    return (unsigned int)wire;
  }

  unsigned int NearestWire(const double* xyz, unsigned int plane)
  {
    /*
      unsigned int res;
      try{
      res = ::larutil::Geometry::GetME()->NearestWire(xyz,plane);
      }catch( ::larutil::InvalidWireError& err){
      res = err.better_wire_number;
      }
    */
    double min_wire=0;
    double max_wire=Nwires(plane)-1;
    double wire = ::larutil::Geometry::GetME()->WireCoordinate(xyz,plane) + 0.5;
    if(wire<min_wire) wire = min_wire;
    if(wire>max_wire) wire = max_wire;
    
    return (unsigned int)wire;
  }

  double PlaneTickOffset(size_t plane0, size_t plane1)
  {
    static double pitch = ::larutil::Geometry::GetME()->PlanePitch();
    static double tick_period = ::larutil::TimeService::GetME()->TPCClock().TickPeriod();
    return (plane1 - plane0) * pitch / DriftVelocity() / tick_period;
  }

  double WirePitch(size_t plane)
  { return ::larutil::Geometry::GetME()->WirePitch(::larlite::geo::View_t(plane)); }

  double WireAngleToVertical(unsigned int plane)
  { return ::larutil::Geometry::GetME()->WireAngleToVertical(larlite::geo::View_t(plane)); }

  int TPCG4Time2Tick(double ns)
  { return ::larutil::TimeService::GetME()->TPCG4Time2Tick(ns); }

  int TPCG4Time2TDC(double ns)
  { return ::larutil::TimeService::GetME()->TPCG4Time2TDC(ns); }
    
  double TPCTDC2Tick(double tdc)
  { return ::larutil::TimeService::GetME()->TPCTDC2Tick(tdc); }

  double TriggerOffsetTPC()
  { return ::larutil::TimeService::GetME()->TriggerOffsetTPC(); }

  double TPCTickPeriod()
  { return ::larutil::TimeService::GetME()->TPCClock().TickPeriod(); }
  
  void ApplySCE(double& x, double& y, double& z)
  {
    static larutil::SpaceChargeMicroBooNE sce;
    auto pos = sce.GetPosOffsets(x,y,z);
    x = x - pos[0] + 0.7;
    y = y + pos[1];
    z = z + pos[2];
  }

  void ApplySCE(double* xyz)
  {
    static larutil::SpaceChargeMicroBooNE sce;
    auto pos = sce.GetPosOffsets(xyz[0],xyz[1],xyz[2]);
    xyz[0] = xyz[0] - pos[0] + 0.7;
    xyz[1] = xyz[1] + pos[1];
    xyz[2] = xyz[2] + pos[2];
  }

  void ApplySCE(TVector3& xyz)
  {
    static larutil::SpaceChargeMicroBooNE sce;
    auto pos = sce.GetPosOffsets(xyz[0],xyz[1],xyz[2]);
    xyz[0] = xyz[0] - pos[0] + 0.7;
    xyz[1] = xyz[1] + pos[1];
    xyz[2] = xyz[2] + pos[2];
  }
  
}

#endif
