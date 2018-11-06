#ifndef __SPARCIFYIMAGE2D_CXX__
#define __SPARCIFYIMAGE2D_CXX__

#include "SparcifyImage2D.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"
#include "larcv/core/DataFormat/EventImage2D.h"

namespace larcv {

  static SparcifyImage2DProcessFactory __global_SparcifyImage2DProcessFactory__;

  SparcifyImage2D::SparcifyImage2D(const std::string name)
    : ProcessBase(name)
  {}
    
  void SparcifyImage2D::configure(const PSet& cfg)
  {
    _image2d_producer = cfg.get<std::string>("Image2DProducer");
    _image2d_channel  = cfg.get<size_t>("Image2DChannel",0);
    _output_producer  = cfg.get<std::string>("OutputProducer");
  }

  void SparcifyImage2D::initialize()
  {}

  bool SparcifyImage2D::process(IOManager& mgr)
  {
    auto const& event_image = mgr.get_data<EventImage2D>(_image2d_producer);
    auto const& image = event_image.as_vector().at(_image2d_channel).as_vector();
    ImageMeta meta = event_image.as_vector().at(_image2d_channel).meta();
    VoxelSet vs;
    float nullvalue=0.;
    for(size_t i=0; i<image.size(); ++i) {
      if(image[i] == nullvalue) continue;
      vs.emplace(i,image[i],true);
    }
    auto& event_output = mgr.get_data<EventSparseTensor2D>(_output_producer);
    event_output.emplace(std::move(vs),std::move(meta));
    return true;
  }

  void SparcifyImage2D::finalize()
  {}

}
#endif
