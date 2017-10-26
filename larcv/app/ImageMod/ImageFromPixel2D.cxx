#ifndef __IMAGEFROMPIXEL2D_CXX__
#define __IMAGEFROMPIXEL2D_CXX__

#include "ImageFromPixel2D.h"
#include "larcv/core/DataFormat/EventPixel2D.h"
#include "larcv/core/DataFormat/EventImage2D.h"
namespace larcv {

  static ImageFromPixel2DProcessFactory __global_ImageFromPixel2DProcessFactory__;

  ImageFromPixel2D::ImageFromPixel2D(const std::string name)
    : ProcessBase(name)
  {}

  void ImageFromPixel2D::configure(const PSet& cfg)
  {
    _pixel2d_producer  = cfg.get<std::string>("Pixel2DProducer");
    _output_producer = cfg.get<std::string>("OutputProducer");
    _type_pi  = (PIType_t)(cfg.get<unsigned short>("PIType", (unsigned short)(PIType_t::kPITypeFixedPI)));
    _fixed_pi = cfg.get<float>("FixedPI", 100);
  }

  void ImageFromPixel2D::initialize()
  {}

  bool ImageFromPixel2D::process(IOManager& mgr)
  {
    auto const& ev_pixel2d = mgr.get_data<larcv::EventPixel2D>(_pixel2d_producer);
    auto& ev_out_image = mgr.get_data<larcv::EventImage2D>(_output_producer);

    std::vector<larcv::Image2D> image_v;
    static std::vector<float> voxel_value_v;
    for(auto meta : ev_pixel2d.get_meta_array()) {

      if(image_v.size() <= meta.id()) image_v.resize(meta.id()+1);

      std::vector<float> img_data(meta.rows() * meta.cols());

      if(voxel_value_v.size() < img_data.size())
        voxel_value_v.resize(img_data.size());

      for(size_t i=0; i<voxel_value_v.size(); ++i) voxel_value_v[i] = 0.;

      auto const& voxel_set_v = ev_pixel2d.get_voxel_set_array(meta.id());

      for(size_t cluster_id=0; cluster_id < voxel_set_v.size(); ++cluster_id) {
        auto const& voxels = voxel_set_v.get_voxel_set(cluster_id);

        for(auto const& vox : voxels.voxel_array()) {
          float val = _fixed_pi;
          switch (_type_pi) {
          case PIType_t::kPITypeFixedPI:
            break;
          case PIType_t::kPITypeInputVoxel:
            val = std::max(img_data[vox.id()],vox.value());
            break;
          case PIType_t::kPITypeClusterIndex:
            val = cluster_id;
            if(vox.value() > voxel_value_v[vox.id()]) {
              val = cluster_id;
              voxel_value_v[vox.id()] = vox.value();
            }
            break;
          case PIType_t::kPITypeUndefined:
            throw larbys("PITypeUndefined not supported!");
            break;
          }
          img_data[vox.id()] = val;
        }
      }
      larcv::Image2D img2d(std::move(meta),std::move(img_data));
      ev_out_image.emplace(std::move(img2d));
    }
    return true;
  }

  void ImageFromPixel2D::finalize()
  {}

}
#endif
