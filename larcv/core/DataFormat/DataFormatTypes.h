#ifndef __DATAFORMATTYPES_H__
#define __DATAFORMATTYPES_H__

#include "larcv/core/Base/LArCVTypes.h"
#include <vector>
#include <set>
namespace larcv {

  /// Invalid rep for vector index
  static const unsigned short kINVALID_INDEX = kINVALID_USHORT;
  /// Image index type for Image2D within EventImage2D  
  typedef unsigned short ImageIndex_t;
  /// ROI index type for Particle within EventROI
  typedef unsigned short ParticleIndex_t;
  /// Projection ID to distinguish different 2D projections
  typedef unsigned short ProjectionID_t;
  /// "ID" of MCShower/MCTrack in terms of its index number in the collection std::vector
  typedef unsigned short MCSTIndex_t;
  /// "ID" of MCTruth in terms of its index number in the collection std::vector
  typedef unsigned short MCTIndex_t;
  /// "ID" for a set of elements
  typedef unsigned short InstanceID_t;
  /// Invalid rep for InstanceID_t
  static const unsigned short kINVALID_INSTANCEID = kINVALID_USHORT;
  /// Invalid projection id
  static const ProjectionID_t kINVALID_PROJECTIONID = kINVALID_USHORT;

  /// "ID" for Voxel3D
  typedef unsigned long long VoxelID_t;
  /// Invalid Voxel3DID_t definition
  static const VoxelID_t kINVALID_VOXELID = kINVALID_ULONGLONG;

  /// Channel status constants
  namespace chstatus {
    static const short kNOTPRESENT = -1;        ///< Channel does not exist
    static const short kNEGATIVEPEDESTAL = -2;  ///< Channel not reco-ed due to pedestal < 0
    /// Standard channel status enum stored in the database
    enum ChannelStatus_t { 
      kDISCONNECTED=0, ///< Channel is not connected
      kDEAD=1,         ///< Dead channel
      kLOWNOISE=2,     ///< Abnormally low noise channel
      kNOISY=3,        ///< Abnormally high noise channel
      kGOOD=4,         ///< Good channel
      kUNKNOWN=5       ///< Channel w/ unverified status
    };
  }

  /// Coordinate unit type
  enum DistanceUnit_t {
    kUnitUnknown, ///< undefined scale (bad!)
    kUnitCM,      ///< cm scale
    kUnitWireTime ///< wire/time in 2D image
  };

  /// Pooling type
  enum PoolType_t {
    kPoolSum,     ///< sum channels
    kPoolAverage, ///< average over channels
    kPoolMax      ///< max channel
  };

  /// Object appearance type in LArTPC  
  enum ShapeType_t {
    kShapeShower,  ///< Shower
    kShapeTrack,   ///< Track
    kShapeUnknown  ///< LArbys
  };

  /// "ID" of MCParticles in terms of its G4 track ID (unless mixing multiple MC samples)
  typedef size_t MCTrackID_t;
  /// A collection of MCTrackID_t for multiple MCParticles
  typedef std::set<larcv::MCTrackID_t> MCTrackIDSet_t;

  /// ProducerID_t to identify a unique data product within a process (for larcv::IOManager)
  typedef size_t ProducerID_t;
  /// ProductName_t to identify a unique data product within a process (for larcv::IOManager)
  typedef std::pair<std::string,std::string> ProducerName_t;
  /// Invalid ProducerID_t
  static const ProducerID_t kINVALID_PRODUCER=kINVALID_SIZE;

  template <class T>
  std::string product_unique_name();

  /// Type of geometrical point from projections
  enum PointType_t {
    kPoint3D,   ///< 3D
    kPointXY,   ///< XY projection (0)
    kPointYZ,   ///< YZ projection (1)
    kPointZX    ///< ZX projection (2)
  };
}
#endif
