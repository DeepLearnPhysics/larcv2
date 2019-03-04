//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ function larcv::BatchDataTypeName(BatchDataType_t);
#pragma link C++ class larcv::ThreadProcessor;
#pragma link C++ class larcv::BatchData<char>+;
#pragma link C++ class larcv::BatchData<short>+;
#pragma link C++ class larcv::BatchData<int>+;
#pragma link C++ class larcv::BatchData<float>+;
#pragma link C++ class larcv::BatchData<double>+;
#pragma link C++ class larcv::BatchData<string>+;
#pragma link C++ class larcv::BatchDataStorage<char>+;
#pragma link C++ class larcv::BatchDataStorage<short>+;
#pragma link C++ class larcv::BatchDataStorage<int>+;
#pragma link C++ class larcv::BatchDataStorage<float>+;
#pragma link C++ class larcv::BatchDataStorage<double>+;
#pragma link C++ class larcv::BatchDataStorage<string>+;
#pragma link C++ class larcv::BatchDataStorageFactory<char>+;
#pragma link C++ class larcv::BatchDataStorageFactory<short>+;
#pragma link C++ class larcv::BatchDataStorageFactory<int>+;
#pragma link C++ class larcv::BatchDataStorageFactory<float>+;
#pragma link C++ class larcv::BatchDataStorageFactory<double>+;
#pragma link C++ class larcv::BatchDataStorageFactory<string>+;
#pragma link C++ class larcv::BatchHolder;
#pragma link C++ class larcv::BatchFillerPIDLabel+;
#pragma link C++ class larcv::BatchFillerMultiLabel+;
#pragma link C++ class larcv::BatchFillerImage2D+;
#pragma link C++ class larcv::BatchFillerTensor3D+;
#pragma link C++ class larcv::BatchFillerBBox2D+;
//#pragma link C++ class larcv::ThreadFillerFactory+;
#pragma link C++ class larcv::RandomCropper+;
//#pragma link C++ class larcv::ExampleCircularBuffer+;
//#pragma link C++ class larcv::BatchFillerImage2D+;
//ADD_NEW_CLASS ... do not change this line
#endif
