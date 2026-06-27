#ifndef __COMBINETENSOR3D_CXX__
#define __COMBINETENSOR3D_CXX__

#include "CombineTensor3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static CombineTensor3DProcessFactory __global_CombineTensor3DProcessFactory__;

  namespace {

    void merge_sorted_voxels(const std::vector<larcv::Voxel>& input,
                             std::vector<larcv::VoxelID_t>& output_id_v,
                             std::vector<float>& output_value_v,
                             CombineTensor3D::PoolType_t pool_type)
    {
      if (input.empty()) return;

      if (output_id_v.empty()) {
        output_id_v.reserve(input.size());
        output_value_v.reserve(input.size());
        for (auto const& vox : input) {
          output_id_v.push_back(vox.id());
          output_value_v.push_back(vox.value());
        }
        return;
      }

      std::vector<larcv::VoxelID_t> merged_id_v;
      std::vector<float> merged_value_v;
      merged_id_v.reserve(output_id_v.size() + input.size());
      merged_value_v.reserve(output_value_v.size() + input.size());

      size_t output_index = 0;
      size_t input_index = 0;
      while (output_index < output_id_v.size() && input_index < input.size()) {
        auto const output_id = output_id_v[output_index];
        auto const& input_vox = input[input_index];

        if (output_id < input_vox.id()) {
          merged_id_v.push_back(output_id);
          merged_value_v.push_back(output_value_v[output_index]);
          ++output_index;
          continue;
        }

        if (input_vox.id() < output_id) {
          merged_id_v.push_back(input_vox.id());
          merged_value_v.push_back(input_vox.value());
          ++input_index;
          continue;
        }

        float value = output_value_v[output_index];
        switch (pool_type) {
        case CombineTensor3D::kSumPool:
          value += input_vox.value();
          break;
        case CombineTensor3D::kMaxPool:
          if (input_vox.value() > value) value = input_vox.value();
          break;
        case CombineTensor3D::kMinPool:
          if (input_vox.value() < value) value = input_vox.value();
          break;
        }
        merged_id_v.push_back(output_id);
        merged_value_v.push_back(value);
        ++output_index;
        ++input_index;
      }

      while (output_index < output_id_v.size()) {
        merged_id_v.push_back(output_id_v[output_index]);
        merged_value_v.push_back(output_value_v[output_index]);
        ++output_index;
      }

      while (input_index < input.size()) {
        auto const& input_vox = input[input_index];
        merged_id_v.push_back(input_vox.id());
        merged_value_v.push_back(input_vox.value());
        ++input_index;
      }

      output_id_v = std::move(merged_id_v);
      output_value_v = std::move(merged_value_v);
    }

  }

  CombineTensor3D::CombineTensor3D(const std::string name)
    : ProcessBase(name)
  {}

  void CombineTensor3D::configure(const PSet& cfg)
  {
    _output_producer = cfg.get<std::string>("OutputProducer");
    _tensor3d_producer_v = cfg.get<std::vector<std::string> >("Tensor3DProducers");
    _pool_type = (PoolType_t)(cfg.get<unsigned short>("PoolType",(unsigned short)kSumPool));
    _fuzzy_distance = cfg.get<double>("FuzzyDistance", -1.);
    if(_tensor3d_producer_v.empty()) {
      LARCV_CRITICAL() << "Tensor3DProducers parameter cannot be empty" << std::endl;
      throw larbys();
    }
  }

  void CombineTensor3D::initialize()
  {}

  bool CombineTensor3D::process(IOManager& mgr)
  {
    //larcv::VoxelSet vs;
    larcv::SparseTensor3D vs;
    Voxel3DMeta meta;
    std::vector<larcv::VoxelID_t> merged_id_v;
    std::vector<float> merged_value_v;

    for(auto const& producer : _tensor3d_producer_v) {
      auto const& ev_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>(producer);
      if(!meta.valid()) {
        meta = ev_tensor3d.meta();
        vs.meta(meta);
      }
      if(meta != ev_tensor3d.meta()) {
        LARCV_CRITICAL() << "Producer " << producer << " has incompatible Voxel3DMeta" << std::endl;
        throw larbys();
      }
      if (_fuzzy_distance <= 0.) {
        merge_sorted_voxels(ev_tensor3d.as_vector(), merged_id_v, merged_value_v, _pool_type);
        continue;
      }
      switch(_pool_type) {
      case kSumPool:
	for(auto const& vox : ev_tensor3d.as_vector())
          vs.add(vox);
        break;
      case kMaxPool:
      case kMinPool:
        for(auto const& vox : ev_tensor3d.as_vector()) {
          auto const& exist_vox = (_fuzzy_distance <= 0.) ? vs.find(vox.id()) : vs.close(vox.id(), _fuzzy_distance);
          if(exist_vox.id() == kINVALID_VOXELID) {
            vs.add(vox);
            continue;
          }
          if(_pool_type == kMaxPool && vox.value() > exist_vox.value())
            vs.emplace(vox.id(),vox.value(),false);
	  else if(_pool_type == kMinPool && vox.value() < exist_vox.value())
	    vs.emplace(vox.id(),vox.value(),false);
        }
        break;
      }
    }

    if (_fuzzy_distance <= 0.) {
      larcv::VoxelSet merged_voxels(merged_id_v, merged_value_v);
      vs.emplace(std::move(merged_voxels), meta);
    }

    auto& out_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>(_output_producer);
    out_tensor3d.emplace(std::move(vs),meta);

    return true;
  }

  void CombineTensor3D::finalize()
  {}

}
#endif
