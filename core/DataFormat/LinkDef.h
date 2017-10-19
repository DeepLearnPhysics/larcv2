//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// namespace
#pragma link C++ namespace larcv::chstatus+;

// enums
#pragma link C++ enum larcv::ProductType_t+;
#pragma link C++ enum larcv::ShapeType_t+;
#pragma link C++ enum larcv::ROIType_t+;
#pragma link C++ enum larcv::chstatus::ChannelStatus_t+;

// Classes
#pragma link C++ class larcv::DataProductFactory+;
#pragma link C++ class larcv::IOManager+;
#pragma link C++ class larcv::EventBase+;
#pragma link C++ class larcv::Vertex+;
#pragma link C++ class larcv::Point2D+;

#pragma link C++ class larcv::Image2D+;
#pragma link C++ class std::vector<larcv::Image2D>+;
#pragma link C++ class larcv::EventImage2D+;

#pragma link C++ class larcv::ImageMeta+;
#pragma link C++ class std::vector<larcv::ImageMeta>+;

#pragma link C++ class larcv::BBox2D+;
#pragma link C++ class std::vector<larcv::BBox2D>+;
#pragma link C++ class larcv::EventBBox2D+;

#pragma link C++ class larcv::BBox3D+;
#pragma link C++ class std::vector<larcv::BBox3D>+;
#pragma link C++ class larcv::EventBBox3D+;

#pragma link C++ class larcv::ChStatus+;
#pragma link C++ class std::vector<larcv::ChStatus>+;
#pragma link C++ class std::map<larcv::PlaneID_t,larcv::ChStatus>+;
#pragma link C++ class larcv::EventChStatus+;

#pragma link C++ class larcv::Voxel+;
#pragma link C++ class larcv::VoxelSet+;
#pragma link C++ class larcv::EventPixel2D+;
#pragma link C++ class larcv::Voxel3DMeta+;
#pragma link C++ class larcv::EventVoxel3D+;

//#pragma link C++ class std::array<unsigned short,6>+;
//#pragma link C++ class std::array<float,6>+;

#pragma link C++ class larcv::IPartition+;
#pragma link C++ class std::vector<larcv::IPartition>+;
#pragma link C++ class larcv::EventIPartition+;

#pragma link C++ class larcv::Particle+;
#pragma link C++ class std::vector<larcv::Particle>+;
#pragma link C++ class larcv::EventParticle+;
//ADD_NEW_CLASS ... do not change this line

#endif










