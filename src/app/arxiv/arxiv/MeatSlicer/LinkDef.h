//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace supera;
//#pragma link C++ class larlite::Supera+;
//#pragma link C++ class supera::Cropper+;
#pragma link C++ class std::set<string>+;
//#pragma link C++ class larcv::SuperaWire+;
//#pragma link C++ class larcv::SuperaHit+;
//#pragma link C++ class larcv::SuperaSimCh+;
//#pragma link C++ class larcv::SuperaBase+;
//#pragma link C++ class larcv::SuperaOpDigit+;
//#pragma link C++ class larcv::SuperaChStatus+;
//#pragma link C++ class supera::MCParticleTree+;
//#pragma link C++ class supera::MCROIMaker+;
#pragma link C++ class larcv::LArCVSuperaDriver;
#pragma link C++ class larlite::LArLiteSuperaDriver+;
//#pragma link C++ class larcv::SuperaMCPCluster+;
//#pragma link C++ class larcv::SuperaKeyPointCluster+;
#pragma link C++ class supera::GridPoint3D+;
#pragma link C++ class supera::WireRange3D+;
#pragma link C++ class std::vector<supera::GridPoint3D>+;
//#pragma link C++ class supera::ImageMetaMakerBase+;
//#pragma link C++ class supera::PulledPork3DSlicer+;
//ADD_NEW_CLASS ... do not change this line
#endif



















