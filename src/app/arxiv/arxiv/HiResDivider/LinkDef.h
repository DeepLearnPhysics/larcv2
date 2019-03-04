//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

//#pragma link C++ class larcv::hires::DivisionDef+;
//#pragma link C++ class std::vector<larcv::hires::DivisionDef>+;

//#pragma link C++ class larcv::hires::HiResImageDivider+;
#pragma link C++ class larcv::CosmicSegment+;
#pragma link C++ class larcv::DumpHiResCropImages+;
#pragma link C++ namespace divalgo;
#pragma link C++ class divalgo::DividerAlgo+;
#pragma link C++ function divalgo::getUstart;
#pragma link C++ function divalgo::getUend;
#pragma link C++ function divalgo::getVstart;
#pragma link C++ function divalgo::getVend;
#pragma link C++ function divalgo::getYstart;
#pragma link C++ function divalgo::getYend;
//ADD_NEW_CLASS ... do not change this line
#endif




















