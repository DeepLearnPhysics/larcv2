//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace larcv::algo+;
#pragma link C++ namespace larcv::algo::crop+;
#pragma link C++ function larcv::algo::crop::EquipartitionCropper(const larcv::Image2D&, size_t, size_t, size_t a=0, size_t b=0);
#pragma link C++ class larcv::algo::crop::Algorithms+;
//ADD_NEW_CLASS ... do not change this line
#endif
































