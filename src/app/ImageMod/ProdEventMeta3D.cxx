#ifndef __PRODEVENTMETA3D_CXX__
#define __PRODEVENTMETA3D_CXX__

#include "ProdEventMeta3D.h"
#include "larcv/core/DataFormat/EventMeta.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

static ProdEventMeta3DProcessFactory __global_ProdEventMeta3DProcessFactory__;

ProdEventMeta3D::ProdEventMeta3D(const std::string name) : ProcessBase(name) {}

void ProdEventMeta3D::configure(const PSet& cfg) {
  _image_producer = cfg.get<std::string>("ImageProducer");
  _input_type = cfg.get<std::string>("InputType", "cluster3d");
  _output_producer = cfg.get<std::string>("OutputProducer");
}

void ProdEventMeta3D::initialize() {}

bool ProdEventMeta3D::process(IOManager& mgr) {

  Voxel3DMeta reference_meta;
  int run, subrun, event;

  if (_input_type == "sparse3d") {
    auto input_image =
        (EventSparseTensor3D*)mgr.get_data(_input_type, _image_producer);
    reference_meta = input_image->meta();
    run = input_image->run();
    subrun = input_image->subrun();
    event = input_image->event();
  } else if (_input_type == "cluster3d") {
    auto input_image =
        (EventClusterVoxel3D*)mgr.get_data(_input_type, _image_producer);
    reference_meta = input_image->meta();
    run = input_image->run();
    subrun = input_image->subrun();
    event = input_image->event();
  } else {
    LARCV_CRITICAL() << "Can not use input type " << _input_type
                   << " to create viewer meta." << std::endl;
    throw larbys();
  }

  auto output_meta = (EventMeta*)(mgr.get_data("meta", _output_producer));

  output_meta->set_id(run, subrun, event);

  double x_min = reference_meta.min_x();
  double y_min = reference_meta.min_y();
  double z_min = reference_meta.min_z();
  double x_max = reference_meta.max_x();
  double y_max = reference_meta.max_y();
  double z_max = reference_meta.max_z();

  double x_n_pixels = reference_meta.num_voxel_x();
  double y_n_pixels = reference_meta.num_voxel_y();
  double z_n_pixels = reference_meta.num_voxel_z();

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

void ProdEventMeta3D::finalize() {}
}
#endif
