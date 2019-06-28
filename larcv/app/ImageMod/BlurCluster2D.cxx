#ifndef __BLURCLUSTER2D_CXX__
#define __BLURCLUSTER2D_CXX__

#include "BlurCluster2D.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"

namespace larcv {

  static BlurCluster2DProcessFactory __global_BlurCluster2DProcessFactory__;

  BlurCluster2D::BlurCluster2D(const std::string name)
  : ProcessBase(name)
  {}

  void BlurCluster2D::configure_labels(const PSet& cfg)
  {
    _cluster2d_producer_v.clear();
    _output_producer_v.clear();
    _cluster2d_producer_v = cfg.get<std::vector<std::string> >("Cluster2DProducerList",_cluster2d_producer_v);
    _output_producer_v   = cfg.get<std::vector<std::string> >("OutputProducerList",_output_producer_v);
    _threshold = cfg.get<float>("Threshold",0);
    _division_threshold = cfg.get<float>("DivisionThreshold",0);
    if(_cluster2d_producer_v.empty()) {
      auto cluster2d_producer = cfg.get<std::string>("Cluster2DProducer","");
      auto output_producer   = cfg.get<std::string>("OutputProducer","");
      if(!cluster2d_producer.empty()) {
        _cluster2d_producer_v.push_back(cluster2d_producer);
        _output_producer_v.push_back(output_producer);
      }
    }

    if(_output_producer_v.empty()) {
      _output_producer_v.resize(_cluster2d_producer_v.size(),"");
    }
    else if(_output_producer_v.size() != _cluster2d_producer_v.size()) {
      LARCV_CRITICAL() << "Cluster2DProducer and OutputProducer must have the same array length!" << std::endl;
      throw larbys();
    }
  }

  void BlurCluster2D::configure(const PSet& cfg)
  {
    configure_labels(cfg);
    _sigma_v   = cfg.get<std::vector<double> >("SigmaXY");
    _numvox_v  = cfg.get<std::vector<size_t> >("NumPixelsXY");
    _normalize = cfg.get<bool>("Normalize", true);
    if (_sigma_v.size() != 2) {
      LARCV_CRITICAL() << "SigmaXY parameter must be length 2 floating point vector!" << std::endl;
      throw larbys();
    }
    if (_numvox_v.size() != 2) {
      LARCV_CRITICAL() << "NumPixelsXY parameter must be length 2 unsigned integer vector!" << std::endl;
      throw larbys();
    }
    
    // create smearing matrix
    _scale_vv.resize(_numvox_v[0] + 1);
    for (auto& scale_v : _scale_vv)
      scale_v.resize(_numvox_v[1] + 1, 0.);
  }

  void BlurCluster2D::initialize()
  {}

  bool BlurCluster2D::process(IOManager& mgr)
  {
    for (size_t producer_index = 0; producer_index < _cluster2d_producer_v.size(); ++producer_index) {
      
      auto const& cluster2d_producer = _cluster2d_producer_v[producer_index];
      auto const& output_producer   = _output_producer_v[producer_index];
      
      auto const& ev_cluster2d = mgr.get_data<larcv::EventClusterPixel2D>(cluster2d_producer);
      auto ev_output = (larcv::EventClusterPixel2D*)(mgr.get_data("cluster2d",output_producer));
      
      for (size_t projection=0; projection<ev_cluster2d.as_vector().size(); ++projection) {
	
	auto const& input_cluster2d = ev_cluster2d.as_vector()[projection];
	
	auto const& meta = input_cluster2d.meta();
	for (size_t xshift = 0; xshift <= _numvox_v[0]; ++xshift) {
	  for (size_t yshift = 0; yshift <= _numvox_v[1]; ++yshift) {
	    double val = exp( - pow(xshift * meta.pixel_width(), 2) / (2. * _sigma_v[0])
			      - pow(yshift * meta.pixel_height(), 2) / (2. * _sigma_v[1]) );
	    _scale_vv[xshift][yshift] = val;
	  }
	}
	
	
	double scale_sum = 1.;
	if(_normalize) {
	  scale_sum = 0.;
	  int x_ctr = -((int)_numvox_v[0]);
	  while(x_ctr <= (int)(_numvox_v[0])) {
	    int y_ctr = -((int)_numvox_v[1]);
	    while(y_ctr <= (int)(_numvox_v[1])) {
	      scale_sum += _scale_vv[std::abs(x_ctr)][std::abs(y_ctr)];
	      ++y_ctr;
	    }
	    ++x_ctr;
	  }
	}
	
	LARCV_INFO() << "scale_sum: " << scale_sum << std::endl;
	
	larcv::VoxelSetArray vsa_output;
	for(size_t cluster_index=0; cluster_index<input_cluster2d.as_vector().size(); ++cluster_index) {
	  
	  auto const& cluster = input_cluster2d.as_vector()[cluster_index];
	  larcv::VoxelSet res_data;
	  for (auto const& vox : cluster.as_vector()) {
	    LARCV_DEBUG() << "Re-mapping vox ID " << vox.id() << " charge " << vox.value() << std::endl;
	    float sum_charge = 0.;
	    auto const pos = meta.position(vox.id());
	    double xpos = pos.x - _numvox_v[0] * meta.pixel_width();
	    double xmax = pos.x + (_numvox_v[0] + 0.5) * meta.pixel_width();
	    int x_ctr = 0;
	    while (xpos < xmax) {
	      double ypos = pos.y - _numvox_v[1] * meta.pixel_height();
	      double ymax = pos.y + (_numvox_v[1] + 0.5) * meta.pixel_height();
	      int y_ctr = 0;
	      while (ypos < ymax) {
		auto const id = meta.index(meta.row(ypos),meta.col(xpos));
		if (id != kINVALID_VOXELID) {
		  
		  int xindex = std::abs(((int)(_numvox_v[0])) - x_ctr);
		  int yindex = std::abs(((int)(_numvox_v[1])) - y_ctr);
		  
		  float scale_factor = _scale_vv[xindex][yindex];
		  float charge = vox.value() * scale_factor / scale_sum;
		  
		  if(charge > _division_threshold) {
		    LARCV_DEBUG() << "... to ID " << id << " charge " << charge << std::endl;
		    res_data.emplace(id, charge, true);
		    sum_charge += charge;
		  }
		}
		ypos += meta.pixel_height();
		++y_ctr;
	      }
	      xpos += meta.pixel_width();
	      ++x_ctr;
	    }
	    LARCV_DEBUG() << "Re-mapped sum charge " << sum_charge << std::endl;
	  }
	  // Only keep voxels above threshold
	  VoxelSet res_data_threshold;
	  for(auto const& vox : res_data.as_vector()) {
	    if(vox.value() <= _threshold) continue;
	    res_data_threshold.emplace(vox.id(), vox.value(), true);
	  }
	  
	  LARCV_INFO() << "Before: vox count = " << cluster.as_vector().size() << " charge = " << cluster.sum()
		       << " ... "
		       << res_data.sum() << " ... " 
		       << "After: vox count = " << res_data_threshold.as_vector().size() << " charge = " << res_data_threshold.sum() << std::endl;
	
	  vsa_output.emplace(std::move(res_data));
	}
	larcv::ClusterPixel2D cp2d_output(std::move(vsa_output),meta);
	ev_output->emplace(std::move(cp2d_output));
      }
    }
    return true;    
  }

void BlurCluster2D::finalize()
{}

}
#endif
