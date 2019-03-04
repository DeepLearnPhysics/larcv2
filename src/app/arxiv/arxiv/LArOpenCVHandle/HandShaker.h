/**
 * \file HandShaker.h
 *
 * \ingroup LArCV2Lite
 * 
 * \brief Class def header for a class HandShaker
 *
 * @author drinkingkazu
 */

/** \addtogroup LArCV2Lite

    @{*/
#ifndef HANDSHAKER_H
#define HANDSHAKER_H

#include <iostream>

#include "Base/GeoTypes.h"

#include "DataFormat/pfpart.h"
#include "DataFormat/vertex.h"
#include "DataFormat/hit.h"
#include "DataFormat/cluster.h"
#include "DataFormat/shower.h"
#include "DataFormat/track.h"
#include "DataFormat/event_ass.h"
#include "DataFormat/mctruth.h"
#include "DataFormat/mcshower.h"
#include "DataFormat/gtruth.h"
#include "DataFormat/simch.h"

#include "DataFormat/EventPGraph.h"
#include "DataFormat/EventROI.h"
#include "DataFormat/EventPixel2D.h"

#include "DataFormat/EventPixel2D.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"

/**
   \class HandShaker
   User defined class HandShaker ... these comments are used to generate
   doxygen documentation!
 */
namespace handshake {

  class HandShaker{
    
  public:
    
    /// Default constructor
    HandShaker(){this->reset();}
    
    /// Default destructor
    ~HandShaker(){}

    void set_larlite_pointers(larlite::event_pfpart*  ev_pfpart,
			      larlite::event_vertex*  ev_vertex,
			      larlite::event_shower*  ev_shower,
			      larlite::event_track*   ev_track,
			      larlite::event_cluster* ev_cluster,
			      larlite::event_hit*     ev_hit,
			      larlite::event_ass*     ev_ass);

    void construct(const larcv::EventPGraph&  ev_pgraph,
		   const larcv::EventPixel2D& ev_pixel2d,
		   const larlite::event_hit*  ev_hit);
    
    
    void reset();

    void pixel_distance_threshold(double dist)
    { _dist_thresh = -1. * dist; }

    bool ready() const;

    void set_pfparticle_types(const std::vector<int>& ptype_v) { _ptype_v = ptype_v; }

  protected:

    double _dist_thresh;
    
    std::vector<int> _ptype_v;

    larocv::GEO2D_Contour_t as_contour(const larcv::Pixel2DCluster& ctor);
    
    larocv::GEO2D_ContourArray_t as_contour_array(const std::vector<larcv::Pixel2DCluster>& ctor_v);
    
    larlite::AssSet_t _ass_pfpart_to_vertex;  // many to 1
    larlite::AssSet_t _ass_vertex_to_track;   // 1 to many
    larlite::AssSet_t _ass_vertex_to_shower;   // 1 to many
    larlite::AssSet_t _ass_pfpart_to_track;   // 1 to 1
    larlite::AssSet_t _ass_pfpart_to_shower;  // 1 to 1
    larlite::AssSet_t _ass_pfpart_to_cluster;  // 1 to many
    larlite::AssSet_t _ass_track_to_cluster;  // 1 to many
    larlite::AssSet_t _ass_shower_to_cluster; // 1 to many
    larlite::AssSet_t _ass_track_to_hit;  // 1 to many
    larlite::AssSet_t _ass_shower_to_hit; // 1 to many
    larlite::AssSet_t _ass_cluster_to_hit;    // 1 to many
    larlite::event_pfpart*  _ev_pfpart;
    larlite::event_vertex*  _ev_vertex;
    larlite::event_shower*  _ev_shower;
    larlite::event_track*   _ev_track;
    larlite::event_cluster* _ev_cluster;
    larlite::event_hit*     _ev_hit;
    larlite::event_ass*     _ev_ass;

  public:
    void copy_here_to_there(larlite::event_hit* ev_in,larlite::event_hit* ev_out);
    void copy_here_to_there(larlite::event_mctruth* ev_in,larlite::event_mctruth* ev_out);
    void copy_here_to_there(larlite::event_mcshower* ev_in,larlite::event_mcshower* ev_out);
    void copy_here_to_there(larlite::event_gtruth* ev_in,larlite::event_gtruth* ev_out);
    void copy_here_to_there(larlite::event_simch* ev_in,larlite::event_simch* ev_out);
    
    
  };
}

#endif
/** @} */ // end of doxygen group 

