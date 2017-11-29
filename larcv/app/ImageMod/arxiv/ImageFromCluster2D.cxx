#ifndef __IMAGEFROMCluster2D_CXX__
#define __IMAGEFROMCluster2D_CXX__

#include "ImageFromCluster2D.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"
#include "larcv/core/DataFormat/EventImage2D.h"
namespace larcv {

  static ImageFromCluster2DProcessFactory __global_ImageFromCluster2DProcessFactory__;

  ImageFromCluster2D::ImageFromCluster2D(const std::string name)
    : ProcessBase(name)
  {}

  void ImageFromCluster2D::configure(const PSet& cfg)
  {
    _cluster2d_producer  = cfg.get<std::string>("Cluster2DProducer");
    _output_producer = cfg.get<std::string>("OutputProducer");
    _type_pi  = (PIType_t)(cfg.get<unsigned short>("PIType", (unsigned short)(PIType_t::kPITypeFixedPI)));
    _fixed_pi = cfg.get<float>("FixedPI", 100);
  }

  void ImageFromCluster2D::initialize()
  {}

  bool ImageFromCluster2D::process(IOManager& mgr)
  {
    auto const& ev_cluster2d = mgr.get_data<larcv::EventClusterPixel2D>(_cluster2d_producer);
    auto& ev_out_image = mgr.get_data<larcv::EventImage2D>(_output_producer);

    std::vector<larcv::Image2D> image_v;
    static std::vector<float> voxel_value_v;

    LARCV_INFO() << "processing " << ev_cluster2d.as_vector().size()
		 << " clusters from " << _cluster2d_producer << std::endl;

    for(auto const& cluster2d_v : ev_cluster2d.as_vector()) {

      auto const& meta = cluster2d_v.meta();

      if(image_v.size() <= meta.id()) image_v.resize(meta.id()+1);

      std::vector<float> img_data(meta.rows() * meta.cols());

      if(voxel_value_v.size() < img_data.size())
        voxel_value_v.resize(img_data.size());

      for(size_t i=0; i<voxel_value_v.size(); ++i) voxel_value_v[i] = 0.;

      auto const& voxel_set_v = cluster2d_v.as_vector();

      for(size_t cluster_id=0; cluster_id < voxel_set_v.size(); ++cluster_id) {

        auto const& voxels = voxel_set_v[cluster_id];

	LARCV_INFO() << "Projection " << meta.id()
		     << " cluster id " << cluster_id
		     << " with " << voxels.as_vector().size() << " voxels" << std::endl;

        for(auto const& vox : voxels.as_vector()) {
          float val = _fixed_pi;
          switch (_type_pi) {
          case PIType_t::kPITypeFixedPI:
            break;
          case PIType_t::kPITypeInputVoxel:
            val = std::max(img_data[vox.id()],vox.value());
            break;
          case PIType_t::kPITypeClusterIndex:
            if(vox.value() > voxel_value_v[vox.id()]) {
              val = cluster_id+1;
              voxel_value_v[vox.id()] = vox.value();
            }else
	      val = img_data[vox.id()];
            break;
          case PIType_t::kPITypeUndefined:
            throw larbys("PITypeUndefined not supported!");
            break;
          }
	  LARCV_DEBUG() << "Voxel " << vox.id() << " value " << vox.value() << " => " << val << std::endl;
	  img_data[vox.id()] = val;
        }
      }
      larcv::Image2D img2d(std::move(meta),std::move(img_data));
      ev_out_image.emplace(std::move(img2d));
    }
    return true;
  }

  void ImageFromCluster2D::finalize()
  {}

}
#endif
