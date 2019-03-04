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
#pragma link C++ namespace larcv+;
//#pragma link C++ class larcv::Watch+;
//#pragma link C++ class larcv::Range<int>+;
//#pragma link C++ class larcv::Range<size_t>+;
//#pragma link C++ class larcv::Range<double>+;
//#pragma link C++ class larcv::UniqueRangeSet<int>+;
//#pragma link C++ class larcv::UniqueRangeSet<size_t>+;
//#pragma link C++ class larcv::UniqueRangeSet<double>+;

//#pragma link C++ class larcv::Point2D+;
#pragma link C++ namespace larcv::msg+;
#pragma link C++ enum  larcv::msg::Level_t+;
#pragma link C++ class larcv::logger+;
#pragma link C++ class larcv::larcv_base+;
#pragma link C++ class larcv::larbys+;

// Configuration handler
#pragma link C++ class larcv::PSet+;
#pragma link C++ function larcv::PSet::get< string > (const string&)+;
#pragma link C++ function larcv::ConfigFile2String(const string)+;
#pragma link C++ function larcv::CreatePSetFromFile(const string)+;
#pragma link C++ class larcv::ConfigManager+;
//ADD_NEW_CLASS ... do not change this line

#endif






