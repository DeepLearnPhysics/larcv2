#ifndef HANDSHAKER_CXX
#define HANDSHAKER_CXX

#include "HandShaker.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "Geo2D/Core/Circle.h"
#include "Geo2D/Core/Line.h"
#ifndef __CLING__
#ifndef __CINT__
#include "opencv2/imgproc.hpp"
#include <opencv2/opencv.hpp>
#endif
#endif

namespace handshake {
  
  larocv::GEO2D_Contour_t HandShaker::as_contour(const larcv::Pixel2DCluster& contour)
  {
    ::larocv::GEO2D_Contour_t ctor;
    ctor.resize(contour.size());
    for(size_t pt_idx=0; pt_idx < contour.size(); ++pt_idx) {
      auto const& pt = contour[pt_idx];
      ctor[pt_idx].x = pt.X();
      ctor[pt_idx].y = pt.Y();
    }
    return ctor;
  }

  larocv::GEO2D_ContourArray_t HandShaker::as_contour_array(const std::vector<larcv::Pixel2DCluster>& contour_v)
  {
    ::larocv::GEO2D_ContourArray_t ctor_v;
    ctor_v.reserve(contour_v.size());
    for(auto const& contour : contour_v)
      ctor_v.emplace_back(std::move(as_contour(contour)));
    return ctor_v;
  }

  void HandShaker::reset() 
  {
    _dist_thresh = 1.;
    _ass_pfpart_to_vertex.clear();
    _ass_pfpart_to_track.clear();
    _ass_pfpart_to_shower.clear();
    _ass_pfpart_to_cluster.clear();
    _ass_vertex_to_track.clear();
    _ass_vertex_to_shower.clear();
    _ass_track_to_cluster.clear();
    _ass_shower_to_cluster.clear();
    _ass_track_to_hit.clear();
    _ass_shower_to_hit.clear();
    _ass_cluster_to_hit.clear();
    _ev_pfpart=nullptr;
    _ev_vertex=nullptr;
    _ev_shower=nullptr;
    _ev_track=nullptr;
    _ev_cluster=nullptr;
    _ev_hit=nullptr;
    _ev_ass=nullptr;
  }

  bool HandShaker::ready() const {
    if( !_ev_pfpart ||
	!_ev_vertex ||
	!_ev_shower ||
	!_ev_track  ||
	!_ev_cluster ||
	!_ev_hit    ||
	!_ev_ass) return false;
    return true;
  }
  
  void HandShaker::set_larlite_pointers(larlite::event_pfpart*  ev_pfpart,
					larlite::event_vertex*  ev_vertex,
					larlite::event_shower*  ev_shower,
					larlite::event_track*   ev_track,
					larlite::event_cluster* ev_cluster,
					larlite::event_hit*     ev_hit,
					larlite::event_ass*     ev_ass)
  {
    _ev_pfpart  = ev_pfpart;
    _ev_vertex  = ev_vertex;
    _ev_shower  = ev_shower;
    _ev_track   = ev_track;
    _ev_cluster = ev_cluster;
    _ev_hit     = ev_hit;
    _ev_ass     = ev_ass;
  }
  
  void HandShaker::construct(const larcv::EventPGraph&  ev_pgraph,
			     const larcv::EventPixel2D& ev_pixel2d,
			     const larlite::event_hit*  ev_hit)
  {
    if(ev_pgraph.PGraphArray().empty())
      return;
    //
    // handle clusters
    //
    std::vector<std::vector<std::vector<unsigned int> > > pxcluster_to_hit;
    std::vector<std::vector<unsigned int> > pxcluster_to_cluster;
    std::vector<larocv::GEO2D_ContourArray_t> contours_v;
    std::vector<std::pair<double,double> > time_bounds;
    std::vector<std::pair<double,double> > wire_bounds;
    std::vector<std::vector<larcv::ImageMeta> > meta_vv;

    for(auto const& key_value : ev_pixel2d.Pixel2DClusterArray()) {
      
      auto const& plane = key_value.first;

      if(pxcluster_to_hit.size() <= plane) pxcluster_to_hit.resize(plane+1);
      if(pxcluster_to_cluster.size() <= plane) pxcluster_to_cluster.resize(plane+1);
      if(contours_v.size()  <= plane) contours_v.resize(plane+1);
      if(time_bounds.size() <= plane) time_bounds.resize(plane+1);
      if(wire_bounds.size() <= plane) wire_bounds.resize(plane+1);
      if(meta_vv.size() <= plane) meta_vv.resize(plane+1);
      
      meta_vv[plane] = ev_pixel2d.ClusterMetaArray(plane);
      auto const& meta_v = meta_vv[plane];

      pxcluster_to_hit[plane].resize(key_value.second.size());
      pxcluster_to_cluster[plane].resize(key_value.second.size());
      double time_min=1e9;
      double time_max=0;
      double wire_min=1e9;
      double wire_max=0;

      for(unsigned int contour_idx=0; contour_idx < key_value.second.size(); ++contour_idx) {
	auto const& contour = key_value.second[contour_idx];
	auto const& image_meta = meta_v[contour_idx];
	double ctime_min=1e9;
	double ctime_max=0;
	double cwire_min=1e9;
	double cwire_max=0;
	for(auto const& pt : contour) {
	  double time = image_meta.max_y() - image_meta.pixel_height() * pt.Y();
	  double wire = image_meta.min_x() + image_meta.pixel_width() * pt.X();
	  if(ctime_min > time) ctime_min = time;
	  if(ctime_max < time) ctime_max = time;
	  if(cwire_min > wire) cwire_min = wire;
	  if(cwire_max < wire) cwire_max = wire;
	}
	pxcluster_to_cluster[plane][contour_idx] = _ev_cluster->size();
	larlite::cluster c;
	c.set_id(_ev_cluster->size());
	larlite::geo::PlaneID c_plane_id(larcv::kINVALID_UINT,larcv::kINVALID_UINT,plane);
	c.set_planeID(c_plane_id);

	_ev_cluster->push_back(c);

	if(time_min > ctime_min) time_min = ctime_min;
	if(time_max < ctime_max) time_max = ctime_max;
	if(wire_min > cwire_min) wire_min = cwire_min;
	if(wire_max < cwire_max) wire_max = cwire_max;

	// std::cout<<"    cluster " << plane << "-" << contour_idx << " time bounds: " << ctime_min << " => " << ctime_max
	// 	 <<" ... wire bounds: " << cwire_min << " => " << cwire_max << std::endl;
	
      }
      wire_min -=2;
      wire_max +=2;
      time_min -=2;
      time_max +=2;
      
      time_bounds[plane].first  = time_min;
      time_bounds[plane].second = time_max;
      wire_bounds[plane].first  = wire_min;
      wire_bounds[plane].second = wire_max;

      // std::cout<<"plane " << plane << " time bounds: " << time_min << " => " << time_max
      // 	       <<" ... wire bounds: " << wire_min << " => " << wire_max << std::endl;

      contours_v[plane] = std::move(this->as_contour_array(key_value.second));

    }
    
    // loop over hits and assign to a cluster
    ::geo2d::Vector<float> pt;
    size_t plane=0;
    double wire=0;
    double time=0;
    for(auto const& h : *ev_hit) {
      plane = h.WireID().Plane;
      auto const& meta_v     = meta_vv.at(plane);
      auto const& time_bound = time_bounds.at(plane);
      auto const& wire_bound = wire_bounds.at(plane);
      time = h.PeakTime() + 2400;
      wire = h.WireID().Wire;
      if(time > time_bound.second) continue;
      if(time < time_bound.first ) continue;
      if(wire > wire_bound.second) continue;
      if(wire < wire_bound.first ) continue;
      auto const& contours = contours_v[plane];
      double max_dist = -1.e9;
      double dist = 0.;
      size_t parent_ctor_idx  = larcv::kINVALID_INDEX;
      size_t parent_ctor_size = 0;
      for(size_t contour_idx=0; contour_idx<contours.size(); ++contour_idx) {
	auto const& contour = contours[contour_idx];
	if(contour.empty()) continue;
	auto const& meta    = meta_v[contour_idx];
	if(meta.min_x() > wire || meta.max_x() < wire) continue;
	if(meta.min_y() > time || meta.max_y() < time) continue;
	pt.x = (wire - meta.min_x()) / meta.pixel_width();
	pt.y = (meta.max_y() - time) / meta.pixel_height();
	dist = cv::pointPolygonTest(contour,pt,true);
	if(dist < _dist_thresh || dist < max_dist) continue;
	//std::cout <<"wire " << wire << " time " << time << " px " << pt.x << " py " << pt.y <<  std::endl;
	if(dist >= 0 && parent_ctor_size > contour.size()) continue;
	//std::cout <<"good!"<<std::endl;
	parent_ctor_idx = contour_idx;
	parent_ctor_size = contour.size();
	max_dist = dist;
      }
      if(parent_ctor_idx == larcv::kINVALID_INDEX) continue;
      pxcluster_to_hit[plane][parent_ctor_idx].push_back(_ev_hit->size());
      _ev_hit->push_back(h);
    }
    
    // take care of an association: cluster=>hit
    _ass_cluster_to_hit.clear();
    _ass_cluster_to_hit.resize(_ev_cluster->size());
    for(size_t plane=0; plane<pxcluster_to_cluster.size(); ++plane) {
      for(size_t pxcluster_idx=0; pxcluster_idx<pxcluster_to_cluster[plane].size(); ++pxcluster_idx) {
	auto const& cindex = pxcluster_to_cluster[plane][pxcluster_idx];
	auto const& hindex_v = pxcluster_to_hit[plane][pxcluster_idx];
	_ass_cluster_to_hit[cindex] = hindex_v;
      }
    }

    // 
    // handle particles
    //
    auto const& pgraph_v = ev_pgraph.PGraphArray();
    const std::vector<size_t> empty_daughters;

    _ass_pfpart_to_track.clear();
    _ass_pfpart_to_shower.clear();
    std::vector<unsigned int> one_ass;
    one_ass.resize(1,0.);
    const std::vector<unsigned int> empty_ass;

    larlite::AssSet_t _ass_pfpart_to_vertex;  // many to 1
    larlite::AssSet_t _ass_track_to_cluster;  // 1 to many
    larlite::AssSet_t _ass_shower_to_cluster; // 1 to many
    
    for(size_t pgraph_idx=0; pgraph_idx < pgraph_v.size(); ++pgraph_idx) {
      // Get nu
      auto const& pgraph = pgraph_v[pgraph_idx];
      size_t parent_id = _ev_pfpart->size();
      double vtx_xyz[3];
      // Get secondaries
      auto const& roi_v = pgraph.ParticleArray();
      auto const& pxcluster_idx_v = pgraph.ClusterIndexArray();
      std::vector<larlite::pfpart> child_pfpart_v;
      std::vector<size_t> child_id_v;
      child_pfpart_v.reserve(roi_v.size());
      child_id_v.reserve(roi_v.size());

      _ass_pfpart_to_track.resize   (parent_id + 1 + roi_v.size());
      _ass_pfpart_to_shower.resize  (parent_id + 1 + roi_v.size());
      _ass_pfpart_to_cluster.resize (parent_id + 1 + roi_v.size());
      _ass_pfpart_to_vertex.resize  (parent_id + 1 + roi_v.size());

      for(size_t roi_idx=0; roi_idx<roi_v.size(); ++roi_idx) {
	auto const& roi = roi_v[roi_idx];
	// Get vertex from 1st particle
	if(!roi_idx) {
	  vtx_xyz[0] = roi.Position().X();
	  vtx_xyz[1] = roi.Position().Y();
	  vtx_xyz[2] = roi.Position().Z();
	  larlite::vertex vtx(vtx_xyz,_ev_vertex->size());
	  _ev_vertex->push_back(vtx);
	  _ass_vertex_to_track.resize(_ev_vertex->size());
	  _ass_vertex_to_shower.resize(_ev_vertex->size());
	}
	// Define a child particle
	size_t child_id = parent_id + 1 + child_pfpart_v.size();

	//
	// record particle type from analysis
	//

	larlite::pfpart child_pfpart(_ptype_v.at(child_id),
				     child_id,
				     parent_id,
				     empty_daughters);
	child_pfpart_v.push_back(child_pfpart);
	child_id_v.push_back(child_id);

	if(roi.Shape() == larcv::kShapeShower) {
	  larlite::shower s;
	  s.set_id(_ev_shower->size());
	  TVector3 xyz;
	  xyz[0] = roi.Position().X();
	  xyz[1] = roi.Position().Y();
	  xyz[2] = roi.Position().Z();
	  s.set_start_point(xyz);
	  one_ass[0] = _ev_shower->size();
	  _ev_shower->emplace_back(std::move(s));
	  _ass_pfpart_to_shower[child_id] = one_ass;
	  _ass_pfpart_to_shower[parent_id].push_back(one_ass[0]);
	  _ass_vertex_to_shower[_ev_vertex->size()-1].push_back(one_ass[0]);
	  _ass_shower_to_cluster.resize(_ev_shower->size());
	  _ass_shower_to_hit.resize(_ev_shower->size());
	}else if(roi.Shape() == larcv::kShapeTrack) {
	  larlite::track t;
	  t.set_track_id(_ev_track->size());
	  TVector3 xyz;
	  xyz[0] = roi.Position().X();
	  xyz[1] = roi.Position().Y();
	  xyz[2] = roi.Position().Z();
	  t.add_vertex(xyz);
	  xyz[0] = roi.EndPosition().X();
	  xyz[1] = roi.EndPosition().Y();
	  xyz[2] = roi.EndPosition().Z();
	  t.add_vertex(xyz);
	  one_ass[0] = _ev_track->size();
	  _ev_track->emplace_back(std::move(t));
	  _ass_pfpart_to_track[child_id] = one_ass;
	  _ass_pfpart_to_track[parent_id].push_back(one_ass[0]);
	  _ass_vertex_to_track[_ev_vertex->size()-1].push_back(one_ass[0]);
	  _ass_track_to_cluster.resize(_ev_track->size());
	  _ass_track_to_hit.resize(_ev_track->size());
	}

	auto const& pxcluster_idx = pxcluster_idx_v[roi_idx];
	for(size_t plane=0; plane<pxcluster_to_cluster.size(); ++plane) {
	  auto const& cindex = pxcluster_to_cluster[plane][pxcluster_idx];
	  _ass_pfpart_to_cluster[child_id].push_back(cindex);
	  if(roi.Shape() == larcv::kShapeShower) {
	    auto shower_id = _ev_shower->size()-1;
	    _ass_shower_to_cluster[shower_id].push_back(cindex);
	    auto const& hindex_v = _ass_cluster_to_hit[cindex];
	    _ass_shower_to_hit[shower_id].reserve( _ass_shower_to_hit[shower_id].size() + hindex_v.size() );
	    for(auto const& hindex : hindex_v) _ass_shower_to_hit[shower_id].push_back(hindex);
	  }
	  if(roi.Shape() == larcv::kShapeTrack) {
	    auto track_id = _ev_track->size()-1;
	    _ass_track_to_cluster[track_id].push_back(cindex);
	    auto const& hindex_v = _ass_cluster_to_hit[cindex];
	    _ass_track_to_hit[track_id].reserve( _ass_track_to_hit[track_id].size() + hindex_v.size() );
	    for(auto const& hindex : hindex_v) _ass_track_to_hit[track_id].push_back(hindex);
	  }
	}
	_ass_pfpart_to_vertex[child_id].push_back(_ev_vertex->size()-1);
      }

      // Record nu
      _ev_pfpart->emplace_back(larlite::pfpart(12,parent_id, parent_id, std::move(child_id_v)));
      _ass_pfpart_to_vertex[parent_id].push_back(_ev_vertex->size()-1);
      // Record secondaries
      for(size_t child_idx=0; child_idx<child_pfpart_v.size(); ++child_idx)
	_ev_pfpart->emplace_back(std::move(child_pfpart_v[child_idx]));
    }
    _ev_ass->set_association( _ev_pfpart->id(),  _ev_vertex->id(),  _ass_pfpart_to_vertex  );
    _ev_ass->set_association( _ev_pfpart->id(),  _ev_shower->id(),  _ass_pfpart_to_shower  );
    _ev_ass->set_association( _ev_pfpart->id(),  _ev_track->id(),   _ass_pfpart_to_track   );
    _ev_ass->set_association( _ev_pfpart->id(),  _ev_cluster->id(), _ass_pfpart_to_cluster );
    _ev_ass->set_association( _ev_vertex->id(),  _ev_shower->id(),  _ass_vertex_to_shower  );
    _ev_ass->set_association( _ev_vertex->id(),  _ev_track->id(),   _ass_vertex_to_track   );
    _ev_ass->set_association( _ev_shower->id(),  _ev_cluster->id(), _ass_shower_to_cluster );
    _ev_ass->set_association( _ev_shower->id(),  _ev_hit->id(),     _ass_shower_to_hit     );
    _ev_ass->set_association( _ev_track->id(),   _ev_cluster->id(), _ass_track_to_cluster  );
    _ev_ass->set_association( _ev_track->id(),   _ev_hit->id(),     _ass_track_to_hit      );
    _ev_ass->set_association( _ev_cluster->id(), _ev_hit->id(),     _ass_cluster_to_hit    );
  }

  void HandShaker::copy_here_to_there(larlite::event_hit* ev_in,     larlite::event_hit* ev_out)      { (*ev_in) = (*ev_out); }
  void HandShaker::copy_here_to_there(larlite::event_mctruth* ev_in, larlite::event_mctruth* ev_out)  { (*ev_in) = (*ev_out); }
  void HandShaker::copy_here_to_there(larlite::event_mcshower* ev_in,larlite::event_mcshower* ev_out) { (*ev_in) = (*ev_out); }
  void HandShaker::copy_here_to_there(larlite::event_gtruth* ev_in,larlite::event_gtruth* ev_out) { (*ev_in) = (*ev_out); }
  void HandShaker::copy_here_to_there(larlite::event_simch* ev_in,larlite::event_simch* ev_out) { (*ev_in) = (*ev_out); }


}

#endif
