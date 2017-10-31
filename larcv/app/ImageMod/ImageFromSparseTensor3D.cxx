#ifndef __IMAGEFROMSPARSETENSOR3D_CXX__
#define __IMAGEFROMSPARSETENSOR3D_CXX__

#include "ImageFromSparseTensor3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
#include "larcv/core/DataFormat/EventImage2D.h"
namespace larcv {

  static ImageFromSparseTensor3DProcessFactory __global_ImageFromSparseTensor3DProcessFactory__;

  ImageFromSparseTensor3D::ImageFromSparseTensor3D(const std::string name)
    : ProcessBase(name)
  {}

  void ImageFromSparseTensor3D::configure(const PSet& cfg)
  {
    _voxel3d_producer = cfg.get<std::string>("SparseTensor3DProducer");
    _output_producer = _voxel3d_producer + "_xyz";
    _output_producer = cfg.get<std::string>("OutputProducer", _output_producer);
    _xy = cfg.get<int>("XY", 1);
    _yz = cfg.get<int>("YZ", 1);
    _zx = cfg.get<int>("ZX", 1);

  }

  void ImageFromSparseTensor3D::initialize()
  {}

  bool ImageFromSparseTensor3D::process(IOManager& mgr)
  {
    auto const& ev_voxel3d = mgr.get_data<larcv::EventSparseTensor3D>(_voxel3d_producer);
    auto const& meta3d = ev_voxel3d.meta();

    auto& ev_image2d = mgr.get_data<larcv::EventImage2D>(_output_producer);

    ProjectionID_t current_id = 0;

    ImageMeta meta_xy(meta3d.min_x(), meta3d.min_y(), meta3d.max_x(), meta3d.max_y(),
                      meta3d.num_voxel_x(), meta3d.num_voxel_y(),
                      (_xy ? current_id : larcv::kINVALID_PROJECTIONID), meta3d.unit());
    if (_xy) current_id += 1;

    ImageMeta meta_yz(meta3d.min_y(), meta3d.min_z(), meta3d.max_y(), meta3d.max_z(),
                      meta3d.num_voxel_y(), meta3d.num_voxel_z(),
                      (_yz ? current_id : larcv::kINVALID_PROJECTIONID),  meta3d.unit());
    if (_yz) current_id += 1;

    ImageMeta meta_zx(meta3d.min_z(), meta3d.min_x(), meta3d.max_z(), meta3d.max_x(),
                      meta3d.num_voxel_z(), meta3d.num_voxel_x(),
                      (_zx ? current_id : larcv::kINVALID_PROJECTIONID), meta3d.unit());

    std::vector<float> img_data_xy;
    std::vector<float> img_data_yz;
    std::vector<float> img_data_zx;

    if (_xy) img_data_xy.resize(meta_xy.rows() * meta_xy.cols(), 0.);
    if (_yz) img_data_yz.resize(meta_yz.rows() * meta_yz.cols(), 0.);
    if (_zx) img_data_zx.resize(meta_zx.rows() * meta_zx.cols(), 0.);

    LARCV_DEBUG() << "Processing " << ev_voxel3d.as_vector().size()
                  << " voxel 3D clusters" << std::endl;

    for (auto const& vox : ev_voxel3d.as_vector()) {
      auto const pos = meta3d.position(vox.id());
      LARCV_DEBUG() << "Voxel " << vox.id() << " value " << vox.value() << std::endl;
      if (_xy) {
        LARCV_DEBUG() << "(x,y) = (" << pos.x << "," << pos.y << ")" << std::endl
                      << meta_xy.dump() << std::endl;
        img_data_xy[meta_xy.index(meta_xy.row(pos.y), meta_xy.col(pos.x))] += vox.value();
      }
      if (_yz) {
        LARCV_DEBUG() << "(y,z) = (" << pos.y << "," << pos.z << ")" << std::endl
                      << meta_yz.dump() << std::endl;
        img_data_yz[meta_yz.index(meta_yz.row(pos.z), meta_yz.col(pos.y))] += vox.value();
      }
      if (_zx) {
        LARCV_DEBUG() << "(z,x) = (" << pos.z << "," << pos.x << ")" << std::endl
                      << meta_zx.dump() << std::endl;
        img_data_zx[meta_zx.index(meta_zx.row(pos.x), meta_zx.col(pos.z))] += vox.value();
      }
    }

    if (_xy) { Image2D res(std::move(meta_xy), std::move(img_data_xy)); ev_image2d.emplace(std::move(res)); }
    if (_yz) { Image2D res(std::move(meta_yz), std::move(img_data_yz)); ev_image2d.emplace(std::move(res)); }
    if (_zx) { Image2D res(std::move(meta_zx), std::move(img_data_zx)); ev_image2d.emplace(std::move(res)); }

    return true;
  }

  void ImageFromSparseTensor3D::finalize()
  {}

}
#endif
