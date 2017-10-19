#include "ANNAlgo.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace ann {

  ANNAlgo::ANNAlgo( int npoints, int ndims ) {
    fNpoints = npoints;
    fNdims   = ndims;
    datablock = nullptr;
    bdtree = nullptr;
    array = nullptr;
    alloc_data_block();
  }

  ANNAlgo::~ANNAlgo () {
    //std::cout << "dealloc ANN" << std::endl;
    if ( datablock!=nullptr ) dealloc_data_block();
    deinitialize();
  }

  void ANNAlgo::alloc_data_block() {
    // make coord block
    datablock = new ANNcoord[fNpoints*fNdims];
    memset( datablock, 0.0, sizeof(ANNcoord)*fNpoints*fNdims );
    // make point array
    points = new ANNpoint[fNpoints];
    for (int pt=0; pt<fNpoints; pt++)
      points[pt] = (datablock+pt*fNdims); // give it the pointer
    // set point array
    array = points;
  }

  void ANNAlgo::dealloc_data_block() {
    array = nullptr;
    delete [] points;
    points = nullptr;
    delete [] datablock;
    datablock = nullptr;
  }

  void ANNAlgo::initialize() {
    // ignore dials for now
    //std::cout << "initializing ANNbd_tree( array, " << fNpoints << "," <<  fNdims << ")" << std::endl;
    bdtree = new ANNbd_tree( array, fNpoints, fNdims );
    _init = true;
  }

  void ANNAlgo::deinitialize() {
    // ignore dials for now
    if ( bdtree!=nullptr)
      delete bdtree;
    bdtree = nullptr;
    _init = false;
  }

  void ANNAlgo::setPoint( int idx, const std::vector<double>& point ) {
    if ( (int)point.size()!=fNdims ) {
      std::cout << "ANNAlgo Error. Dimension of point " << point.size() << " is not right dimensions, " << fNdims << std::endl;
      return;
    }

    for ( int i=0; i<fNdims; i++ ) {
      *(datablock + idx*fNdims + i) = point.at(i);
    }
  }

  void ANNAlgo::setPoint2D( int idx, double x, double y ) {
    if ( fNdims!=2 ) {
      std::cout << "ANNAlgo Error. Dimension of point (" << 2 << ") is not right dimensions, " << fNdims << std::endl;
      return;
    }

    *(datablock + idx*fNdims + 0) = x;
    *(datablock + idx*fNdims + 1) = y;
  }

  void ANNAlgo::getPoint( int idx, std::vector<double>& point ) {
    point.resize(fNdims,0.0);
    for( int i=0; i<fNdims; i++ ) {
      point.at(i) = *(datablock+ idx*fNdims + i);
    }
  }

  ANNpoint ANNAlgo::getPoint( int idx ) {
    return (ANNpoint)(datablock + idx*fNdims);
  }

  std::vector<int> ANNAlgo::regionQuery( int idx, double eps2, double approx ) {
    ANNpoint queryPt = getPoint( idx );
    //std::pair< std::vector<int>, std::vector<double> > ret = bdtree->annkFRSearch2(queryPt, eps2, approx);
    //std::pair< std::vector<int>, std::vector<double> > ret = bdtree->annkFRSearch(queryPt, eps2);
    int k = 0;
    int ktotal = bdtree->annkFRSearch(queryPt, eps2, k);
    
    // get the list of points within radius
    int* nn_idx     = new int[ktotal];
    double* nn_dist = new double[ktotal];

    bdtree->annkFRSearch( queryPt, (ANNdist)eps2, ktotal, (ANNidxArray)nn_idx, (ANNdistArray)nn_dist, approx );
    std::vector<int> ret;
    for (int k=0; k<ktotal; k++) {
      ret.push_back( nn_idx[k] );
    }
    
    delete [] nn_idx;
    delete [] nn_dist;
    
    return ret;
  }

  std::vector<int> ANNAlgo::regionQuery( const std::vector<double>& queryPt_v, double eps2, double approx ) {
    if ( (int)queryPt_v.size()!=fNdims )
      throw std::runtime_error( "Query Point dim is not the same of the data points in the kD-tree" );
    ANNpoint queryPt = new ANNcoord[fNdims]; // typedef  to ANNcoord* or double*
    for (int i=0; i<(int)queryPt_v.size(); i++) {
      *(queryPt+i) = queryPt_v[i];
    }
    int k = 0;
    int ktotal = bdtree->annkFRSearch(queryPt, eps2, k);
    
    // get the list of points within radius
    int* nn_idx     = new int[ktotal];
    double* nn_dist = new double[ktotal];

    bdtree->annkFRSearch( queryPt, (ANNdist)eps2, ktotal, (ANNidxArray)nn_idx, (ANNdistArray)nn_dist, approx );
    std::vector<int> ret;
    for (int k=0; k<ktotal; k++) {
      ret.push_back( nn_idx[k] );
    }
    
    delete [] nn_idx;
    delete [] nn_dist;
    delete [] queryPt;
    
    return ret;
  }

  void ANNAlgo::dump( std::string outfile ) {
    std::ofstream out( outfile.c_str() );
    if ( _init ) {
      bdtree->Dump( ANNtrue, out );
    }
    out.close();
  }

  void ANNAlgo::printdata() {
    for (int ipt=0; ipt<fNpoints; ipt++) {
      std::cout << "[" << ipt << "] ";
      ANNpoint apoint = *(((ANNpoint*)array) + ipt);
      for (int ix=0; ix<fNdims; ix++) {
	ANNcoord x = *((ANNcoord*)apoint + ix);
	std::cout << " " << x;
      }
      std::cout << std::endl;
    }
  }

  void ANNAlgo::cleanup() {
    annClose();
  }
}
