#include "DBSCANAlgo.h"

#include <algorithm> // for std::sort
#include <sstream>
#include <stdexcept>

#include "DataFormat/Image2D.h"
#include "DataFormat/ImageMeta.h"

namespace dbscan {

  dbscanOutput DBSCANAlgo::scan( dbPoints input, int minPts, double eps, bool borderPoints, double approx ) {

    // allocate output
    dbscanOutput output;

    size_t npts  = input.size();
    if ( npts==0 )
      return output;
    int ndims = input.at(0).size();
    std::vector<double> weights;

    // create bdtree structure object
    ann::ANNAlgo bdtree( npts, ndims );
    
    // now fill it
    for (size_t i=0; i<npts; i++) {
      bdtree.setPoint( i, input.at(i) );
    }

    // initialize structure
    bdtree.initialize();

    // kd-tree uses squared distances
    double eps2 = eps*eps;

    bool weighted = false;
    double Nweight = 0.0;
    if (weights.size() != 0) {
      if (weights.size() != npts) {
	std::cout << "length of weights vector is incompatible with data." << std::endl;
	return output;
      }
      weighted = true;
    }

    // DBSCAN
    std::vector<bool> visited(npts, false);
    std::vector<int>  N, N2;
    output.clusterid.resize(npts,-1);
    output.nneighbors.resize(npts,0);
    
    for (size_t i=0; i<npts; i++) {
      // check for interrupt
      //if (!(i % 100)) Rcpp::checkUserInterrupt(); //convert this to python
      
      if (visited[i]) continue;

      // ---------------------------------------------------------
      // Region Query
      // ---------------------------------------------------------
      // replacing this
      //N = regionQuery(i, dataPts, kdTree, eps2, approx);
      //if(frNN.size())   N = Rcpp::as< std::vector<int> >(frNN[i]);
      //else              N = regionQuery(i, dataPts, kdTree, eps2, approx);
      // ---------------------------------------------------------
      N = bdtree.regionQuery( i, eps2, approx );
      output.nneighbors.at( i ) = N.size();
      // Note: the points are not sorted by distance!
      // ---------------------------------------------------------

      // noise points stay unassigned for now
      //if (weighted) Nweight = sum(weights[IntegerVector(N.begin(), N.end())]) +
      //       if (weighted) {
      // 	// This should work, but Rcpp has a problem with the sugar expression!
      // 	// Assigning the subselection forces it to be materialized.
      // 	// Nweight = sum(weights[IntegerVector(N.begin(), N.end())]) +
      // 	// weights[i];
      // 	NumericVector w = weights[IntegerVector(N.begin(), N.end())];
      // 	Nweight = sum(w);
      //       } else Nweight = N.size();
      
      //       if (Nweight < minPts) continue;
      
      // start new cluster and expand
      std::vector<int> cluster;
      cluster.push_back(i);
      visited[i] = true;

      while (!N.empty()) {
	int j = N.back();
	N.pop_back();

	if (visited[j]) continue; // point already processed
	visited[j] = true;

// 	if(frNN.size())   N2 = Rcpp::as< std::vector<int> >(frNN[j]);
// 	else              N2 = regionQuery(j, dataPts, kdTree, eps2, approx);
	N2 = bdtree.regionQuery(j,eps2,approx);
		
	if (weighted) {
	  //NumericVector w = weights[IntegerVector(N2.begin(), N2.end())];
	  //Nweight = sum(w);
	  Nweight = 0.0;
	  for (int in2=0; in2<(int)N2.size(); in2++) {
	    Nweight += weights.at( N2.at(in2) );
	  }
	} else Nweight = (double)N2.size();

	if (Nweight >= minPts) { // expand neighborhood
	  // this is faster than set_union and does not need sort! visited takes
	  // care of duplicates.
	  std::copy(N2.begin(), N2.end(), std::back_inserter(N));
	}

	// for DBSCAN* (borderPoints==FALSE) border points are considered noise
	if(Nweight >= minPts || borderPoints) cluster.push_back(j);
      }

      // add cluster to list
      output.clusters.push_back(cluster);
    }

    // prepare cluster vector
    // unassigned points are noise (cluster 0)
    output.clusterid.resize(npts,0);
    for (std::size_t i=0; i<output.clusters.size(); i++) {
      for (std::size_t j=0; j<output.clusters[i].size(); j++) {
	output.clusterid[output.clusters[i][j]] = i; // R adds one probably because 1-indexed vectors: WTF
      }
    }

    // clean up
    //ann::ANNAlgo::cleanup();    
    
    return output;
    
  }


  // DBSCAN Output Functions
  int dbscanOutput::findMatchingCluster( const std::vector<double>& testpoint, const dbPoints& data, const double radius ) const {
    // inputs
    // ------
    //  testpoint: test point in data space. will try to find closest cluster to this point
    //  data: list of all points in the data. the same data set used to form this cluster output
    //  radius: maximum radius the test point needs to be to some member data point in a cluster
    // output
    // ------
    //  return int: index to cluster that contains a point closest to the test point
    // this is an order N search

    // -------------------------------
    // list of exceptions
    class querypoint_mismatch_ex: public std::exception
    {
      virtual const char* what() const throw()
      {
	return "dbscanOutput::findMatchingCluster. Query Point and data point sizes do not match.";
      }
    } querypoint_mismatch;
    // -------------------------------

    int matching_cluster = -1;
    double r2 = radius*radius;
    double closest_r2 = 0.0;
    for (size_t ic=0; ic<clusters.size(); ic++) {
      const dbCluster &cluster = clusters.at(ic);
      int nhits = clusters.at(ic).size();
      for (int ihit=0; ihit<nhits; ihit++) {
	int hitidx = cluster.at(ihit);
	double dist = 0.;
	if ( testpoint.size()!=data.at(hitidx).size() ) {
	  throw querypoint_mismatch;
	}
	for (size_t i=0; i<testpoint.size();i++) {
	  dist += (data.at(hitidx).at(i)-testpoint.at(i))*(data.at(hitidx).at(i)-testpoint.at(i));
	}
	if (dist<r2 && (matching_cluster==-1 || dist<closest_r2) ) {
	  matching_cluster = ic;
	  closest_r2 = dist;
	}
      }
    }
    return matching_cluster;
  }

  
  void dbscanOutput::closestHitsInCluster( const int clusterid, const std::vector<double>& test_pos, const dbPoints& src_data, 
					   const larcv::ImageMeta& meta, const float cm_per_tick, const float cm_per_wire,
					   std::vector< std::pair<int,double> >& hitlist, const int max_nhits ) const {
    // inputs
    // ------
    //  clusterid: index of cluster in clusters data member
    //  test_pos2d: position to determine how close
    //  src_data: dbPoints used to make these clusters (up to user to not screw this up)
    //  number_of_hits: number of closest hits to return
    // output
    // ------
    //  hitlist: sorted ordered pairs of (hit indices, distance to test point) from closest to furthest
    // this is an order N search. paired with sorting of a list.  not great.  but, we can be more clever in the future if we need to be

    // input checks
    if ( clusterid<0 || clusterid>=(int)clusters.size() ) {
      throw std::runtime_error("dbscanOutput::closestHitsInCluster: invalid cluster id");
    }
    if ( clusters.at(clusterid).size()==0 ) {
      throw std::runtime_error("dbscanOutput::closestHitsInCluster: weird, empty cluster");
    }
    if ( clusters.at(clusterid).at(0)<0 || clusters.at(clusterid).at(0)>=(int)src_data.size() ) {
      throw std::runtime_error("dbscanOutput::closestHitsInCluster: invalid first hit in test cluster");
    }
    if ( src_data.at( clusters.at(clusterid).at(0) ).size()!=test_pos.size() ) {
      std::stringstream ss;
      ss << "dbscanOutput::closestHitsInCluster: test point dimensions (" << test_pos.size() << ") "
	 << " do not match source data (" << src_data.at( clusters.at(clusterid).at(0) ).size() << ")";
      throw std::runtime_error(ss.str());
    }
    
    // if number of hits > number of this in the cluster, just going to return distances for all hits

    // we compare the distance
    struct mycompare_t {
      //bool operator() ( std::pair<int,double>& lhs, std::pair<int,double>& rhs ) {
      bool operator() ( std::pair<int,double> lhs, std::pair<int,double> rhs ) { 
      if ( lhs.second<rhs.second ) 
	return true;
      return false;
      }
    } mycompare;
    
    hitlist.clear();
    //std::cout << " test point: " << test_pos[0] << ", " << test_pos[1] << std::endl;
    //std::cout << " height=" << meta.pixel_height() << " width=" << meta.pixel_width() << " cm/tick=" << cm_per_tick << " cm/wire= " << cm_per_wire << std::endl;
    for (size_t ihit=0; ihit<clusters.at(clusterid).size(); ihit++) {
      int hitidx = clusters.at(clusterid).at(ihit);
      double hit_dist  = 0.0;
      const std::vector<double>& hitpos = src_data.at(hitidx);
      double dt = (hitpos[1]-test_pos[1])*meta.pixel_height()*cm_per_tick;
      double dw = (hitpos[0]-test_pos[0])*meta.pixel_width()*cm_per_wire;
      hit_dist = sqrt( dt*dt + dw*dw ); // in cm
      //std::cout << ihit << " " << hitidx << " " << hit_dist << std::endl;
      std::pair<int,double> test_hit( hitidx, hit_dist );
      if ( max_nhits>0 ) {
	// we have to care about the number of hits in the list
	if ( (int)hitlist.size()<max_nhits ) {
	  // but not now
	  hitlist.emplace_back( test_hit );
	}
	else {
	  bool isbetter = false;
	  for (size_t ilisthit=0; ilisthit<(size_t)max_nhits; ilisthit++) {
	    if ( hitlist.at(ilisthit).second > hit_dist ) {
	      isbetter = true;
	      break;
	    }
	  }
	  if ( isbetter )
	    hitlist.emplace_back( test_hit );
	}
      }
      else {
	// dont care about the number of hits. add it to hit list.
	hitlist.emplace_back( test_hit );
      }

      // sort the current hitlist vector
      std::sort( hitlist.begin(), hitlist.end(), mycompare );
      // truncate the end
      if ( max_nhits>0 && max_nhits<(int)hitlist.size() )
	hitlist.resize(max_nhits);
    }//end of loop over hits in the cluster
    
  }


  // ==========================================================================================
  // UTILITY FUNCTIONS

  dbPoints extractPointsFromImage( const larcv::Image2D& img, const double threshold ) {
    const larcv::ImageMeta& meta = img.meta();
    dbPoints pixels = extractPointsFromImageBounds( img, threshold, 0, meta.rows()-1, 0, meta.cols()-1 );
    return pixels;
  }
  dbPoints extractPointsFromImageBounds( const larcv::Image2D& img, const double threshold, int row_min, int row_max, int col_min, int col_max ) {
    dbPoints pixels;
    const larcv::ImageMeta& meta = img.meta();
    if ( row_min<0 ) row_min = 0;
    if ( row_max>=(int)meta.rows() ) row_max = (int)meta.rows()-1;
    if ( col_min<0 ) col_min = 0;
    if ( col_max>=(int)meta.cols() ) col_max = (int)meta.cols()-1;
    for (int r=row_min; r<=row_max; r++) {
      for (int c=col_min; c<=col_max; c++) {
        if ( img.pixel(r,c)>threshold ) {
          std::vector<double> pixel(2,0.0);
          pixel[0] = (double)c;
          pixel[1] = (double)r;
          pixels.emplace_back( std::move(pixel) );
        }
      }
    }
    return pixels;
  }

  ClusterExtrema ClusterExtrema::FindClusterExtrema( const int cluster_id, const dbscanOutput& clustering_info, const dbPoints& hitlist )  {
    if ( cluster_id <0 || cluster_id >= (int)clustering_info.clusters.size() )
        throw std::runtime_error("ClusterExtrema::FindClusterExtrema[error] cluster id is invalid");

    const dbCluster& cluster = clustering_info.clusters.at(cluster_id);
    return ClusterExtrema::FindClusterExtrema( cluster, hitlist );
  }

  ClusterExtrema ClusterExtrema::FindClusterExtrema( const dbCluster& cluster, const dbPoints& hitlist ) {

    ClusterExtrema extrema;
    if ( cluster.size()==0 ) 
      return extrema;

    // seed the values
    int firsthit_idx = cluster.front();
    const std::vector<double>& hit = hitlist.at(firsthit_idx);
    extrema.leftmost() = hit;
    extrema.rightmost() = hit;
    extrema.topmost() = hit;
    extrema.bottommost() = hit;

    // find the extrema
    for ( int ihit=1; ihit<(int)cluster.size(); ihit++) {
      int hitidx = cluster.at(ihit);
      const std::vector<double>& hit = hitlist.at(hitidx);
      if ( hit[0]<extrema.leftmost()[0]) {
        extrema.leftmost()[0] = hit[0];
        extrema.leftmost()[1] = hit[1];
      }
      if (hit[0]>extrema.rightmost()[0]) {
        extrema.rightmost()[0] = hit[0];
        extrema.rightmost()[1] = hit[1];
      }
      if (hit[1]>extrema.topmost()[1]) {
        extrema.topmost()[0] = hit[0];
        extrema.topmost()[1] = hit[1];
      }
      if (hit[1]<extrema.bottommost()[1]) {
        extrema.bottommost()[0] = hit[0];
        extrema.bottommost()[1] = hit[1];
      }
    } 
    extrema.empty = false;
    return extrema;   
  }

  ClusterExtrema ClusterExtrema::MakeEmptyExtrema() {
    ClusterExtrema empty;
    return empty;
  }


}
