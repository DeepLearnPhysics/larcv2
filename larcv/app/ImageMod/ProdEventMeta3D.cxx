#ifndef __PRODEVENTMETA3D_CXX__
#define __PRODEVENTMETA3D_CXX__

#include "ProdEventMeta3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
#include "larcv/core/DataFormat/EventMeta.h"

namespace larcv {

  static ProdEventMeta3DProcessFactory __global_ProdEventMeta3DProcessFactory__;

  ProdEventMeta3D::ProdEventMeta3D(const std::string name)
    : ProcessBase(name)
  {}

  void ProdEventMeta3D::configure(const PSet& cfg)
  {
    _image_producer  = cfg.get<std::string> ( "ImageProducer" );
    _output_producer = cfg.get<std::string> ( "OutputProducer" );

  }

  void ProdEventMeta3D::initialize()
  {
  }

  bool ProdEventMeta3D::process(IOManager& mgr)
  {

    auto input_image  = (EventClusterVoxel3D*)(mgr.get_data("cluster3d", _image_producer));
    
    auto output_meta = (EventMeta*)(mgr.get_data("meta", _output_producer));

    output_meta -> set(input_image->run(), input_image->subrun(), input_image->event());

    auto meta_3d = input_image->meta();


    double x_min = 0.0;
    double y_min = 0.0;
    double z_min = 0.0;
    double x_max = meta_3d.num_voxel_x() * meta_3d.size_voxel_x();
    double y_max = meta_3d.num_voxel_y() * meta_3d.size_voxel_y();
    double z_max = meta_3d.num_voxel_z() * meta_3d.size_voxel_z();

    double x_n_pixels = meta_3d.num_voxel_x();
    double y_n_pixels = meta_3d.num_voxel_y();
    double z_n_pixels = meta_3d.num_voxel_z();



    output_meta->store("x_min", x_min);
    output_meta->store("y_min", y_min);
    output_meta->store("z_min", z_min);
    output_meta->store("x_max", x_max);
    output_meta->store("y_max", y_max);
    output_meta->store("z_max", z_max);
    output_meta->store("x_n_pixels", x_n_pixels);
    output_meta->store("y_n_pixels", y_n_pixels);
    output_meta->store("z_n_pixels", z_n_pixels);




    return true;
  }

  void ProdEventMeta3D::finalize()
  {}

}
#endif
