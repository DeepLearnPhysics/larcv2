#ifndef __DBSCAN_ALGO__
#define __DBSCAN_ALGO__

#include "ANN/ANNAlgo.h"

#include <vector>
#include <utility> // for pair

#include "DataFormat/ImageMeta.h"

namespace dbscan {

  typedef std::vector< std::vector<double> > dbPoints; // list of (x,y,z,....) points
  typedef std::vector< int > dbCluster; // list of indices to dbPoints
  typedef std::vector< std::vector<int> >  dbClusters; // list of list of indices to provided dbPoints

  class dbscanOutput {
  public: 
    dbscanOutput() {
      clusters.clear();
      clusterid.clear();
    };
    virtual ~dbscanOutput() {};
    dbClusters clusters;
    std::vector<int> clusterid;
    std::vector<int> nneighbors;

    // utility functions. seems like useful, common tasks, but would make more sense to separate these from the data itself?
    int findMatchingCluster( const std::vector<double>& testpoint, const dbPoints& data, const double radius ) const;
    void closestHitsInCluster( const int clusterid, const std::vector<double>& test_pos, const dbPoints& src_data, 
                               const larcv::ImageMeta& meta, const float cm_per_tick, const float cm_per_wire,
                               std::vector< std::pair<int,double> >& hitlist, const int max_nhits=-1 ) const;

  };
  

  class DBSCANAlgo {

  public:
    DBSCANAlgo() {};
    virtual ~DBSCANAlgo() {};

    dbscanOutput scan( dbPoints input, int minPoints, double eps, bool borderPoints=false, double approx=0.0 );

  };

  // Utility functions

  // extract from an Image2D object, the pixels (in (col,row)=(wire,tick)) above a given threshold
  dbPoints extractPointsFromImage( const larcv::Image2D& img, const double threshold );
  dbPoints extractPointsFromImageBounds( const larcv::Image2D& img, const double threshold, int row_min, int row_max, int col_min, int col_max );  

  // Cluster Extrema. Often it is useful to know the extent of a cluster
  class ClusterExtrema {
    public:
      typedef enum { kleftmost=0, krightmost, ktopmost, kbottommost, kNumExtrema } Extrema_t;

      std::vector< std::vector<double> > m_extrema;
      bool empty;

    protected:
      // don't make your own object, use the factory method below
      ClusterExtrema() {
        empty = false;
        m_extrema.resize(4);
        for (size_t i=0; i<(size_t)kNumExtrema; i++ )
          m_extrema.at(i).resize(2,0.0);
      };

    public:
      virtual ~ClusterExtrema() {};

      bool isempty() const { return empty; }
      const std::vector<double>& leftmost()   const  { return m_extrema[kleftmost]; }
      const std::vector<double>& rightmost()  const  { return m_extrema[krightmost]; }
      const std::vector<double>& topmost()    const  { return m_extrema[ktopmost]; }
      const std::vector<double>& bottommost() const  { return m_extrema[kbottommost]; } 
      const std::vector<double>& extrema( const Extrema_t whichpoint ) const { return m_extrema[(size_t)whichpoint]; }
      std::vector<double>& leftmost()   { return m_extrema[kleftmost]; }
      std::vector<double>& rightmost()  { return m_extrema[krightmost]; }
      std::vector<double>& topmost()    { return m_extrema[ktopmost]; }
      std::vector<double>& bottommost() { return m_extrema[kbottommost]; }      
      std::vector<double>& extrema( const Extrema_t whichpoint ) { return m_extrema[(size_t)whichpoint]; }

      static ClusterExtrema FindClusterExtrema( const int cluster_id, const dbscanOutput& clustering_info, const dbPoints& hitlist );
      static ClusterExtrema FindClusterExtrema( const dbCluster& cluster, const dbPoints& hitlist );
      static ClusterExtrema MakeEmptyExtrema(); // for cases when you need to represent a null cluster
    };


}

#endif
