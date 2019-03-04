/**
 * \file ANNAlgo.h
 *
 * \ingroup ANN
 * 
 * \brief Interface to ANN tree search package
 *
 * @author twongjirad
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __ANN_ALGO__
#define __ANN_ALGO__
#ifndef __CINT__
#ifndef __CLING__
#include <ANN/ANN.h>
#endif
#endif
#include <vector>
#include <string>

namespace ann {

  class ANNAlgo {

  public:

    ANNAlgo( int npoints, int ndims );
    virtual ~ANNAlgo();

    void setPoint( int idx, const std::vector<double>& point ); ///< add point, which is a list of [ndims] doubles, returns point index
    void setPoint2D( int idx, double x, double y ); //sigh.
    void getPoint( int idx, std::vector<double>& point );       ///< get point, which is a list of [ndims] doubles, returns point index
    #ifndef __CINT__
    #ifndef __CLING__
    ANNpoint getPoint( int idx );
    #endif
    #endif
    std::vector<int> regionQuery( int idx, double eps2, double approx );
    std::vector<int> regionQuery( const std::vector<double>& queryPt_v, double eps2, double approx );
    static void cleanup();
    void dump( std::string outfile );
    void printdata();
    void initialize();
    void deinitialize();

  protected:

    int fNdims;    ///< number of dimensions
    int fNpoints;  ///< number of points
    bool _init;
    #ifndef __CINT__
    #ifndef __CLING__
    ANNcoord* datablock;
    ANNpoint* points;
    ANNpointArray array;

    ANNbd_tree* bdtree;
    #endif
    #endif
    void alloc_data_block();
    void dealloc_data_block();

  };


}

#endif

/** @} */ // end of doxygen group 
