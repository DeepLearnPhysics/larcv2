//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class larcv::LArbysImageMaker+;
#pragma link C++ class larcv::PreProcessor+;
#pragma link C++ class larcv::LArbysImageExtract+;

#pragma link C++ class larcv::LArbysImageMC+;
#pragma link C++ class larcv::VertexInROI+;
#pragma link C++ class larcv::LEE1e1pAna+;
#pragma link C++ class larcv::VertexAna+;
#pragma link C++ class larcv::ExampleAna+;

#pragma link C++ namespace handshake+;
#pragma link C++ class handshake::HandShaker+;

#pragma link C++ class std::vector<std::vector<std::vector<std::pair<size_t,size_t> > > >+;
#pragma link C++ class std::vector<std::vector<larcv::ImageMeta> >+;

#pragma link C++ class larcv::ROIAna+;
#pragma link C++ class larcv::CosmicPixelAna+;
#pragma link C++ class larcv::SSNetPixelAna+;
#pragma link C++ class larcv::OperateImage+;
#pragma link C++ class larcv::PostTagger+;

#pragma link C++ class larcv::BlankImage+;

#pragma link C++ class larcv::LArbysImage+;
#pragma link C++ class larcv::LArbysImageCheater+;
#pragma link C++ class larcv::LArbysImageReAna+;

#pragma link C++ class larcv::BadRegionAna+;
#pragma link C++ class larcv::SegmentFilter+;
#pragma link C++ class larcv::GenDeadChannelMap+;
#pragma link C++ class larcv::DeadWireAna+;
#pragma link C++ class larcv::GoodcROIFilter+;
#pragma link C++ class larcv::CosmicTrackAna+;
//ADD_NEW_CLASS ... do not change this line
#endif










