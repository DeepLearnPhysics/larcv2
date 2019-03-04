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
#pragma link C++ class larcv::Point3D+;

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
#pragma link C++ class std::map<larcv::ProjectionID_t,larcv::ChStatus>+;
#pragma link C++ class larcv::EventChStatus+;

#pragma link C++ class larcv::Voxel+;
#pragma link C++ class std::vector<larcv::Voxel>+;
#pragma link C++ class larcv::VoxelSet+;
#pragma link C++ class larcv::VoxelSetArray+;


#pragma link C++ class larcv::ClusterPixel2D+;
#pragma link C++ class larcv::SparseTensor2D+;
#pragma link C++ class larcv::EventClusterPixel2D+;
#pragma link C++ class larcv::EventSparseTensor2D+;

#pragma link C++ class larcv::Voxel3DMeta+;
#pragma link C++ class larcv::ClusterVoxel3D+;
#pragma link C++ class larcv::SparseTensor3D+;
#pragma link C++ class larcv::EventClusterVoxel3D+;
#pragma link C++ class larcv::EventSparseTensor3D+;

//#pragma link C++ class std::array<unsigned short,6>+;
//#pragma link C++ class std::array<float,6>+;

//#pragma link C++ class larcv::VoxelPartition+;
//#pragma link C++ class std::vector<larcv::VoxelPartition>+;
//#pragma link C++ class larcv::VoxelPartitionSet+;
//#pragma link C++ class larcv::EventVoxelPartition+;

#pragma link C++ class larcv::Particle+;
#pragma link C++ class std::vector<larcv::Particle>+;
#pragma link C++ class larcv::ParticleSet+;
#pragma link C++ class larcv::EventParticle+;

#pragma link C++ class larcv::Meta+;
#pragma link C++ class larcv::EventMeta+;

#pragma link C++ class larcv::FlatTensorContents+;
#pragma link C++ function larcv::as_image2d(const SparseTensor2D&)+;
#pragma link C++ function larcv::as_image2d(const VoxelSet&, const ImageMeta&)+;
#pragma link C++ function larcv::as_sparse_tensor2d(const ClusterPixel2D&)+;
#pragma link C++ function larcv::as_image2d(const ClusterPixel2D&)+;
#pragma link C++ function larcv::as_sparse_tensor3d(const ClusterVoxel3D&)+;

//ADD_NEW_CLASS ... do not change this line

#endif














