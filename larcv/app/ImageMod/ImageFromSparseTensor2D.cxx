#ifndef __IMAGEFROMSPARSETENSOR2D_CXX__
#define __IMAGEFROMSPARSETENSOR2D_CXX__

#include "ImageFromSparseTensor2D.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"
#include "larcv/core/DataFormat/EventImage2D.h"
namespace larcv {

  static ImageFromSparseTensor2DProcessFactory __global_ImageFromSparseTensor2DProcessFactory__;

  ImageFromSparseTensor2D::ImageFromSparseTensor2D(const std::string name)
    : ProcessBase(name)
  {}

  void ImageFromSparseTensor2D::configure(const PSet& cfg)
  {
    _tensor2d_producer  = cfg.get<std::string>("Tensor2DProducer");
    _output_producer = cfg.get<std::string>("OutputProducer");
    _type_pi  = (PIType_t)(cfg.get<unsigned short>("PIType", (unsigned short)(PIType_t::kPITypeFixedPI)));
    _fixed_pi = cfg.get<float>("FixedPI", 100);
  }

  void ImageFromSparseTensor2D::initialize()
  {}

  bool ImageFromSparseTensor2D::process(IOManager& mgr)
  {
    auto const& ev_tensor2d = mgr.get_data<larcv::EventSparseTensor2D>(_tensor2d_producer);
    auto& ev_out_image = mgr.get_data<larcv::EventImage2D>(_output_producer);

    std::vector<larcv::Image2D> image_v;
    static std::vector<float> voxel_value_v;
    for(auto const& tensor2d_v : ev_tensor2d.as_vector()) {

      auto const& meta = tensor2d_v.meta();
      
      if(image_v.size() <= meta.id()) image_v.resize(meta.id()+1);

      std::vector<float> img_data(meta.rows() * meta.cols());

      if(voxel_value_v.size() < img_data.size())
        voxel_value_v.resize(img_data.size());

      for(size_t i=0; i<voxel_value_v.size(); ++i) voxel_value_v[i] = 0.;

      for(auto const& vox : tensor2d_v.as_vector()) {
	float val = _fixed_pi;
	switch (_type_pi) {
	case PIType_t::kPITypeFixedPI:
	  break;
	case PIType_t::kPITypeInputVoxel:
	  val = std::max(img_data[vox.id()],vox.value());
	  break;
	case PIType_t::kPITypeClusterIndex:
	case PIType_t::kPITypeUndefined:
	  throw larbys("PITypeUndefined and kPITypeClusterIndex not supported!");
	  break;
	}
	img_data[vox.id()] = val;
      }
      larcv::Image2D img2d(std::move(meta),std::move(img_data));
      ev_out_image.emplace(std::move(img2d));
    }
    return true;
  }

  void ImageFromSparseTensor2D::finalize()
  {}

}
#endif
