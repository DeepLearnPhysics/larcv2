#ifndef __FIELDRESPONSE_CXX__
#define __FIELDRESPONSE_CXX__

#include "FieldResponse.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static FieldResponseProcessFactory __global_FieldResponseProcessFactory__;

  FieldResponse::FieldResponse(const std::string name)
    : ProcessBase(name)
  {}

  void FieldResponse::configure(const PSet& cfg)
  {
    // Configure the module parameters
    _tensor3d_producer = cfg.get<std::string>("Tensor3DProducer");
    _voxel_value = cfg.get<float>("VoxelValue");
  }

  void FieldResponse::initialize()
  {}

  bool FieldResponse::process(IOManager& mgr)
  {
    // Retrieve input
    auto const& input_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>(_tensor3d_producer);

    // Create output data container
    larcv::VoxelSet vs;
    vs.reserve(input_tensor3d.size());

    // Loop over input tensor elements and do something
    for(auto const& vox : input_tensor3d.as_vector())
        vs.emplace(vox.id(), _voxel_value,true);
 
    // store output
    auto& output_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>("field_response");
    output_tensor3d.emplace(std::move(vs),input_tensor3d.meta());
    return true;
  }

  void FieldResponse::finalize()
  {}

}
#endif
