//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// Classes
#pragma link C++ class larcv::CSVData+;
#pragma link C++ function larcv::read_csv(string,string)+;
//ADD_NEW_CLASS ... do not change this line

#endif







