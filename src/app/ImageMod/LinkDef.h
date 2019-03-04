//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

//#pragma link C++ class larcv::Binarize+;
//#pragma link C++ class larcv::ADCScaleAna+;
//#pragma link C++ class larcv::ADCScale+;
//#pragma link C++ class larcv::CombineImages+;
//#pragma link C++ class larcv::SliceImages+;
//#pragma link C++ class larcv::ImageCompressor+;
//#pragma link C++ class larcv::SegmentAna+;
//#pragma link C++ class larcv::StepDigitizer+;
//#pragma link C++ class larcv::ADCThreshold+;
//#pragma link C++ class larcv::SimpleDigitizer+;
//#pragma link C++ class larcv::ResizeImage+;
//#pragma link C++ class larcv::CosmicROIFiller+;
//#pragma link C++ class larcv::MeanSubtraction+;
//#pragma link C++ class larcv::SegmentRelabel+;
//#pragma link C++ class larcv::WireMask+;
//#pragma link C++ class larcv::EmbedImage+;
//#pragma link C++ class larcv::MaskImage+;
//#pragma link C++ class larcv::SegmentMask+;
//#pragma link C++ class larcv::SegmentMaker+;
//#pragma link C++ class larcv::CropBBox2D+;
//#pragma link C++ class larcv::ROIMask+;
//#pragma link C++ class larcv::ChannelMax+;
//#pragma link C++ class larcv::SegWeightTrackShower+;
//#pragma link C++ class larcv::ModularPadImage+;
//#pragma link C++ class larcv::MultiROICropper+;
#pragma link C++ function larcv::cluster_to_image2d(const larcv::Pixel2DCluster&, size_t, size_t)+;
//#pragma link C++ class larcv::LoadImageMod+;
//#pragma link C++ class larcv::ROIMerger+;
//#pragma link C++ class larcv::ImageFromCluster2D+;
#pragma link C++ class larcv::ImageFromTensor2D+;
//#pragma link C++ class larcv::ROIPad+;

//#pragma link C++ class larcv::UnionROI+;
#pragma link C++ class larcv::Tensor3DCompressor+;
#pragma link C++ class larcv::Cluster3DCompressor+;
#pragma link C++ class larcv::Tensor3DFromCluster3D+;
#pragma link C++ class larcv::SegWeightInstance3D+;
#pragma link C++ class larcv::SegLabelFromCluster3D+;
#pragma link C++ class larcv::MaskTensor3D+;
#pragma link C++ class larcv::ThresholdTensor3D+;
//#pragma link C++ class larcv::ParticleROIFromCluster+;
#pragma link C++ class larcv::Cluster2DFromCluster3D+;
#pragma link C++ class larcv::Tensor2DFromTensor3D+;
#pragma link C++ class larcv::BlurTensor3D+;
#pragma link C++ class larcv::NormalizeTensor3D+;
#pragma link C++ class larcv::CombineTensor3D+;
#pragma link C++ class larcv::VertexWeight3D+;
#pragma link C++ class larcv::Cluster2DThreshold+;
#pragma link C++ class larcv::OneParticleTensor+;
#pragma link C++ class larcv::MCTrackFix+;
#pragma link C++ class larcv::PPNParticleRemover+;
#pragma link C++ class larcv::SimCoefficient+;
#pragma link C++ class larcv::SparcifyImage2D+;
#pragma link C++ class larcv::SegLabelFiveTypes+;
#pragma link C++ class larcv::XYZ2PixelCoordinate+;
//ADD_NEW_CLASS ... do not change this line
#endif







































