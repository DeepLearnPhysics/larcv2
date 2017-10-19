#ifndef __SEGWEIGHTTRACKSHOWER_CXX__
#define __SEGWEIGHTTRACKSHOWER_CXX__

#include "SegWeightTrackShower.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventPixel2D.h"

namespace larcv {

  static SegWeightTrackShowerProcessFactory __global_SegWeightTrackShowerProcessFactory__;

  SegWeightTrackShower::SegWeightTrackShower(const std::string name)
    : ProcessBase(name)
  {}
    
  void SegWeightTrackShower::configure(const PSet& cfg)
  {

    _label_producer = cfg.get<std::string>("LabelProducer");

    _keypt_pixel2d_producer = cfg.get<std::string>("KeyPointProducer","");
    
    _weight_producer = cfg.get<std::string>("WeightProducer");

    _plane_v = cfg.get<std::vector<size_t> >("PlaneID");

    _weight_max = cfg.get<unsigned int>("WeightMax");

    if(_label_producer.empty()) {
      LARCV_CRITICAL() << "Label producer empty!" << std::endl;
      throw larbys();
    }

    if(_weight_producer.empty()) {
      LARCV_CRITICAL() << "Weight producer empty!" << std::endl;
      throw larbys();
    }

    if(_plane_v.empty()) {
      LARCV_CRITICAL() << "No plane ID requested for process" << std::endl;
      throw larbys();
    }

    _pool_type = (PoolType_t)(cfg.get<unsigned short>("PoolType",(unsigned short)kSumPool));

    _weight_surrounding = cfg.get<bool>("WeightSurrounding",true);

    _weight_vertex = cfg.get<bool>("WeightVertex",false);

    _dist_surrounding = cfg.get<size_t>("DistSurrounding",1);

    if(_weight_surrounding && !_dist_surrounding) {
      LARCV_CRITICAL() << "You want to weight surrounding pixels but did not provide DistSurrounding!" << std::endl;
      throw larbys();
    }

    if(_weight_vertex) 
      LARCV_WARNING() << "Vertex weighting is not supported yet." << std::endl;

  }

  void SegWeightTrackShower::initialize()
  {}

  bool SegWeightTrackShower::process(IOManager& mgr)
  {
    auto const& label_image_v = ((EventImage2D*)(mgr.get_data(kProductImage2D,_label_producer)))->Image2DArray();

    auto event_weight_v = ((EventImage2D*)(mgr.get_data(kProductImage2D,_weight_producer)));

    std::vector<larcv::Image2D> weight_image_v;
    event_weight_v->Move(weight_image_v);

    // make sure plane id is available
    for(auto const& plane : _plane_v) {
      if(plane < label_image_v.size()) continue;
      LARCV_CRITICAL() << "Plane ID " << plane << " not found!" << std::endl;
      throw larbys();
    }


    //
    // Initialize 
    //
    std::vector<float> temp_weight_data;
    std::vector<float> boundary_data;
    for(auto const& plane : _plane_v) {
      
      weight_image_v.resize(plane+1);
      auto& weight_image = weight_image_v[plane];
      auto const& label_image = label_image_v[plane];
      auto const& label_data  = label_image.as_vector();

      if(!weight_image.as_vector().empty()) {
	// image already exists. check the dimension
	if( weight_image.meta().min_x() != label_image.meta().min_x() ||
	    weight_image.meta().max_y() != label_image.meta().max_y() ||
	    weight_image.meta().cols()  != label_image.meta().cols()  ||
	    weight_image.meta().rows()  != label_image.meta().rows() ) {
	  LARCV_CRITICAL() << "Plane " << plane << "has incompatible label/weight image meta" << std::endl;
	  throw larbys();
	}
      }else{
	// create weight image
	weight_image = larcv::Image2D(label_image.meta());
	weight_image.paint(1.);
      }

      // compute # shower pixels, # track pixels
      size_t npx_shower = 0;
      size_t npx_track  = 0;
      for(auto const& v : label_image.as_vector()) {
	if(v == kROIEminus || v == kROIGamma || v == kROIPizero)
	  ++npx_shower;
	else if(v == kROIMuminus || v == kROIKminus || v == kROIPiminus || v == kROIProton)
	  ++npx_track;
      }

      float shower_weight = 1.;
      float track_weight  = 1.;

      if( npx_track  ) track_weight  = (int)((float)(label_image.as_vector().size()) / (float)(npx_track ));
      if( npx_shower ) shower_weight = (int)((float)(label_image.as_vector().size()) / (float)(npx_shower));

      LARCV_INFO() << npx_shower << " shower pixels and " << npx_track << " track pixels found..." << std::endl;
      LARCV_INFO() << "Weights: shower = " << shower_weight << " and track = " << track_weight << std::endl;
      auto const& weight_data = weight_image.as_vector();
      temp_weight_data.resize(weight_data.size());
      boundary_data.resize(weight_data.size());
      for(size_t idx=0; idx<weight_data.size(); ++idx)
	temp_weight_data[idx] = boundary_data[idx] = 0.;

      for(size_t idx=0; idx<weight_data.size(); ++idx) {
	ROIType_t v = (ROIType_t)(label_data[idx]);
	if(v == kROIEminus || v == kROIGamma || v == kROIPizero)
	  temp_weight_data[idx] = (float)shower_weight;
	else if(v == kROIMuminus || v == kROIKminus || v == kROIPiminus || v == kROIProton)
	  temp_weight_data[idx] = (float)track_weight;
	else
	  temp_weight_data[idx] = 0.;
      }
      
      float surrounding_weight=0;
      if(_weight_surrounding && _dist_surrounding) {
	int target_row, target_col;
	int nrows = weight_image.meta().rows();
	int ncols = weight_image.meta().cols();
	size_t n_surrounding_pixel = 0;
	bool flag = false;
	auto const& label_meta = label_image.meta();
	for(int row=0; row<nrows; ++row) {
	  for(int col=0; col<ncols; ++col) {
	    ROIType_t v = (ROIType_t)(label_image.pixel(row,col));
	    if(v == kROIEminus || v == kROIGamma || v == kROIPizero ||
	       v == kROIMuminus || v == kROIKminus || v == kROIPiminus || v == kROIProton) {
	      boundary_data[label_meta.index(row,col)] = 0.;
	      continue;
	    }
	    flag = false;
	    for(target_row = (int)(row+_dist_surrounding); (int)(target_row + _dist_surrounding) >= row; --target_row) {
	      if(target_row >= nrows) continue;
	      if(target_row < 0) break;
	      for(target_col = (int)(col+_dist_surrounding); (int)(target_col + _dist_surrounding) >= col; --target_col) {
		if(target_col >= ncols) continue;
		if(target_col < 0) break;
		v = (ROIType_t)(label_image.pixel(target_row,target_col));
		if(v == kROIEminus || v == kROIGamma || v == kROIPizero ||
		   v == kROIMuminus || v == kROIKminus || v == kROIPiminus || v == kROIProton) {
		  flag=true;
		  break;
		}
	      }
	      if(flag) break;
	    }
	    boundary_data[label_meta.index(row,col)] = (flag ? 1. : 0.);
	    if(flag) ++n_surrounding_pixel;
	  }
	}

	if(n_surrounding_pixel)
	  surrounding_weight = (int)((float)(label_data.size()) / (float)(n_surrounding_pixel));

	LARCV_INFO() << "Found " << n_surrounding_pixel << " pixels around interesting ones..." << std::endl;
	LARCV_INFO() << "Weight: " << surrounding_weight << std::endl;

	for(size_t idx=0; idx < weight_data.size(); ++idx) 
	  temp_weight_data[idx] += (boundary_data[idx] * surrounding_weight);
      }

      //
      // Compute keypoint weghts
      //
      
      if(!_keypt_pixel2d_producer.empty()) {
	auto ev_pixel = ((EventPixel2D*)(mgr.get_data(kProductPixel2D,_keypt_pixel2d_producer)));
	auto const& pcluster_m = ev_pixel->Pixel2DClusterArray();
	auto const& meta_m     = ev_pixel->ClusterMetaArray();

	auto meta_iter = meta_m.find(plane);
	if(meta_iter == meta_m.end()) {
	  LARCV_CRITICAL() << "Plane " << plane << " not found in ClusterMetaArray()!" << std::endl;
	  throw larbys();
	}
	
	auto clus_iter = pcluster_m.find(plane);
	if(clus_iter==pcluster_m.end()) {
	  LARCV_CRITICAL() << "Plane " << plane << " not found in Pixel2DClusterArray()!" << std::endl;
	  throw larbys();
	}

	auto const& ref_meta   = label_image.meta();
	auto const& ref_data   = label_image.as_vector();
	auto const& pcluster_v = (*clus_iter).second;
	auto const& meta_v     = (*meta_iter).second;

	// sanity check
	if(meta_v.size() != pcluster_v.size() ) {
	  LARCV_CRITICAL() << "# cluster meta and # cluster does not match!" << std::endl;
	  throw larbys();
	}
	for(auto const& meta : meta_v) {
	  if(meta == ref_meta) continue;
	  LARCV_CRITICAL() << "Found non-compatible image meta!" << std::endl
			   << "      Cluster: " << meta.dump()
			   << "      Ref    : " << ref_meta.dump();
	  throw larbys();
	}
      
	for(size_t cluster_idx=0; cluster_idx<pcluster_v.size(); ++cluster_idx){
	  auto const& pcluster = pcluster_v[cluster_idx];
	  auto const& meta     = meta_v[cluster_idx];
	  for(auto const& pixel2d : pcluster) {
	    size_t index = (pixel2d.X() * ref_meta.rows() + pixel2d.Y());
	    if(ref_data[index]>0) temp_weight_data[index] += _weight_max;
	  }
	}
      }

      //
      // Combine weights
      //
      float v=0;
      switch(_pool_type) {
      case kSumPool:
	for(size_t idx=0; idx<weight_data.size(); ++idx) {
	  v = weight_data[idx] + temp_weight_data[idx];
	  if(v > (float)(_weight_max)) v = _weight_max;
	  weight_image.set_pixel(idx,v);
	}
	break;
      case kMaxPool:
	for(size_t idx=0; idx<weight_data.size(); ++idx) {
	  v = std::max(weight_data[idx],temp_weight_data[idx]);
	  if(v > (float)(_weight_max)) v = _weight_max;
	  weight_image.set_pixel(idx,v);
	}
	break;
      case kAveragePool:
	for(size_t idx=0; idx<weight_data.size(); ++idx) {
	  v = (weight_data[idx] + temp_weight_data[idx])/2.;
	  if(v > (float)(_weight_max)) v = _weight_max;
	  weight_image.set_pixel(idx,v);
	}
	break;
      case kOverwrite:
	for(size_t idx=0; idx<weight_data.size(); ++idx) {
	  v = temp_weight_data[idx];
	  if(v > (float)(_weight_max)) v = _weight_max;
	  weight_image.set_pixel(idx,v);
	}
	break;
      }

    }
    
    event_weight_v->Emplace(std::move(weight_image_v));
    return true;
  }

  void SegWeightTrackShower::finalize()
  {}

}
#endif
