//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace dbscan;
#pragma link C++ class std::vector< std::vector<double> >;
#pragma link C++ class std::vector< std::vector<int> >;
#pragma link C++ typedef dbPoints;
#pragma link C++ typedef dbCluster;
#pragma link C++ typedef dbClusters;
#pragma link C++ class dbscan::DBSCANAlgo+;
#pragma link C++ class larcv::DBSCAN+;
#pragma link C++ class larcv::NNCosine+;
//ADD_NEW_CLASS ... do not change this line
#endif

