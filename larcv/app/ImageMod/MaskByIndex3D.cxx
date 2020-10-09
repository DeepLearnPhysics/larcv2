#ifndef __MaskByIndex3D_CXX__
#define __MaskByIndex3D_CXX__

#include "MaskByIndex3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static MaskByIndex3DProcessFactory __global_MaskByIndex3DProcessFactory__;

  MaskByIndex3D::MaskByIndex3D(const std::string name)
    : ProcessBase(name)
  {}

  void MaskByIndex3D::configure_labels(const PSet& cfg)
  {
    _target_tensor3d_v.clear();
    _output_tensor3d_v.clear();
    _target_tensor3d_v    = cfg.get<std::vector<std::string> >("TargetTensor3DList",    _target_tensor3d_v    );
    _output_tensor3d_v    = cfg.get<std::vector<std::string> >("OutputTensor3DList",    _output_tensor3d_v    );
    if (_target_tensor3d_v.empty()) {
      auto target_tensor3d    = cfg.get<std::string>("TargetTensor3D",  "");
      auto output_tensor3d    = cfg.get<std::string>("OutputTensor3D",    "");
      if (!target_tensor3d.empty()) {
        _target_tensor3d_v.push_back(target_tensor3d);
        _output_tensor3d_v.push_back(output_tensor3d);
      }
    }

    if (_output_tensor3d_v.empty()) {
      _output_tensor3d_v.resize(_target_tensor3d_v.size(), "");
    }
    else if (_output_tensor3d_v.size() != _target_tensor3d_v.size()) {
      LARCV_CRITICAL() << "TargetTensor3D and OutputTensor3D must have the same array length!" << std::endl;
      throw larbys();
    }
      
    _target_cluster3d_v.clear();
    _output_cluster3d_v.clear();
    _target_cluster3d_v    = cfg.get<std::vector<std::string> >("TargetCluster3DList",    _target_cluster3d_v    );
    _output_cluster3d_v    = cfg.get<std::vector<std::string> >("OutputCluster3DList",    _output_cluster3d_v    );
    if (_target_tensor3d_v.empty()) {
      auto target_cluster3d    = cfg.get<std::string>("TargetCluster3D",  "");
      auto output_cluster3d    = cfg.get<std::string>("OutputCluster3D",    "");
      if (!target_cluster3d.empty()) {
        _target_cluster3d_v.push_back(target_cluster3d);
        _output_cluster3d_v.push_back(output_cluster3d);
      }
    }

    if (_output_cluster3d_v.empty()) {
      _output_cluster3d_v.resize(_target_cluster3d_v.size(), "");
    }
    else if (_output_cluster3d_v.size() != _target_cluster3d_v.size()) {
      LARCV_CRITICAL() << "TargetCluster3D and OutputCluster3D must have the same array length!" << std::endl;
      throw larbys();
    }
      
  }

  void MaskByIndex3D::configure(const PSet& cfg)
  {
    configure_labels(cfg);

    _tensor3d_min_v = cfg.get<std::vector<float> >("MinTensor3DValueList", _tensor3d_min_v);
    if (_tensor3d_min_v.empty()) {
      auto tensor3d_min = cfg.get<float>("MinTensor3DValue", 0.);
      _tensor3d_min_v.resize(_target_tensor3d_v.size(), tensor3d_min);
    } else if (_tensor3d_min_v.size() != _target_tensor3d_v.size()) {
      LARCV_CRITICAL() << "MinTensor3DValueList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

    _cluster3d_min_v = cfg.get<std::vector<float> >("MinCluster3DValueList", _cluster3d_min_v);
    if (_cluster3d_min_v.empty()) {
      auto cluster3d_min = cfg.get<float>("MinCluster3DValue", 0.);
      _cluster3d_min_v.resize(_target_cluster3d_v.size(), cluster3d_min);
    } else if (_cluster3d_min_v.size() != _target_cluster3d_v.size()) {
      LARCV_CRITICAL() << "MinCluster3DValueList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }
    
    std::vector<size_t> xmin,xmax,ymin,ymax,zmin,zmax;
    xmin = cfg.get<std::vector<size_t> >("XRangeMin",xmin);
    ymin = cfg.get<std::vector<size_t> >("YRangeMin",ymin);
    zmin = cfg.get<std::vector<size_t> >("ZRangeMin",zmin);
    xmax = cfg.get<std::vector<size_t> >("XRangeMax",xmax);
    ymax = cfg.get<std::vector<size_t> >("YRangeMax",ymax);
    zmax = cfg.get<std::vector<size_t> >("ZRangeMax",zmax);
    
    for(size_t i=0; i<xmin.size(); ++i) {
      size_t imin = xmin.at(i);
      size_t imax = xmax.at(i);
      assert(imin<=imax);
      for(size_t idx=imin; idx<=imax; ++idx) {
        if(_xmask_v.size() <= idx) _xmask_v.resize(idx+1,false);
        _xmask_v[idx]=true;
      }        
    }
    for(size_t i=0; i<ymin.size(); ++i) {
      size_t imin = ymin.at(i);
      size_t imax = ymax.at(i);
      assert(imin<=imax);
      for(size_t idx=imin; idx<=imax; ++idx) {
        if(_ymask_v.size() <= idx) _ymask_v.resize(idx+1,false);
        _ymask_v[idx]=true;
      }        
    }
    for(size_t i=0; i<zmin.size(); ++i) {
      size_t imin = zmin.at(i);
      size_t imax = zmax.at(i);
      assert(imin<=imax);
      for(size_t idx=imin; idx<=imax; ++idx) {
        if(_zmask_v.size() <= idx) _zmask_v.resize(idx+1,false);
        _zmask_v[idx]=true;
      }        
    }
  }

  void MaskByIndex3D::initialize()
  {}

  bool MaskByIndex3D::process(IOManager& mgr)
  {
      
    // Tensor3D
    for (size_t producer_index = 0; producer_index < _target_tensor3d_v.size(); ++producer_index) {

      auto const& target_producer = _target_tensor3d_v[producer_index];
      auto output_producer = _output_tensor3d_v[producer_index];
      if (output_producer.empty()) output_producer = target_producer + "_masked";

      auto const& ev_target = mgr.get_data<larcv::EventSparseTensor3D>(target_producer);
      auto& ev_output = mgr.get_data<larcv::EventSparseTensor3D>(output_producer);

      if (ev_output.meta().valid()) {
        static bool one_time_warning = true;
        if (_output_tensor3d_v[producer_index].empty()) {
          LARCV_CRITICAL() << "Over-writing existing EventSparseTensor3D data for label "
                           << output_producer << std::endl;
          throw larbys();
        }
        else if (one_time_warning) {
          LARCV_WARNING() << "Output EventSparseTensor3D producer " << output_producer
                          << " already holding valid data will be over-written!" << std::endl;
          one_time_warning = false;
        }
      }

      larcv::VoxelSet vs;
      auto const& value_min = _tensor3d_min_v[producer_index];
      size_t x,y,z;
      auto meta = ev_target.meta();
      for(auto const& vox : ev_target.as_vector()) {
        meta.id_to_xyz_index(vox.id(),x,y,z);
        if(vox.value() < value_min) continue;
        if( (x<_xmask_v.size() && _xmask_v[x]) || 
            (y<_ymask_v.size() && _ymask_v[y]) || 
            (z<_zmask_v.size() && _zmask_v[z]) )
          continue;
        vs.emplace(vox.id(),vox.value(),false);
      }
      std::cout<<ev_target.size()<<" => "<<vs.size()<<std::endl;
      ev_output.emplace(std::move(vs), ev_target.meta());
    }
      
      
    // Cluster3D  
    for (size_t producer_index = 0; producer_index < _target_cluster3d_v.size(); ++producer_index) {

      auto const& target_producer = _target_cluster3d_v[producer_index];
      auto output_producer = _output_cluster3d_v[producer_index];
      if (output_producer.empty()) output_producer = target_producer + "_masked";

      auto const& ev_target = mgr.get_data<larcv::EventClusterVoxel3D>(target_producer);
      auto ev_output = (EventClusterVoxel3D*)(mgr.get_data("cluster3d",output_producer));

      auto const meta = ev_target.meta();
      if (ev_output->meta().valid()) {
        static bool one_time_warning = true;
        if (_output_cluster3d_v[producer_index].empty()) {
          LARCV_CRITICAL() << "Over-writing existing EventClusterVoxel3D data for label "
                           << output_producer << std::endl;
          throw larbys();
        }
        else if (one_time_warning) {
          LARCV_WARNING() << "Output EventClusterVoxel3D producer " << output_producer
                          << " already holding valid data will be over-written!" << std::endl;
          one_time_warning = false;
        }
      }

      auto const& value_min = _cluster3d_min_v[producer_index];
      size_t x,y,z;
      std::vector<larcv::VoxelSet> vs_v;
      for(auto const& target : ev_target.as_vector()) {
        larcv::VoxelSet vs;
        for(auto const& vox : target.as_vector()) {
          meta.id_to_xyz_index(vox.id(),x,y,z);
          if(vox.value() < value_min) continue;
          if(_xmask_v[x] || _ymask_v[y] || _zmask_v[z]) continue;
          vs.emplace(vox.id(), vox.value(), false);
        }
        vs_v.emplace_back(std::move(vs));
      }
      larcv::VoxelSetArray vsa;
      vsa.emplace(std::move(vs_v));
      ev_output->emplace(std::move(vsa),meta);
    }
      
      
    return true;
  }

  void MaskByIndex3D::finalize()
  {}

}
#endif
