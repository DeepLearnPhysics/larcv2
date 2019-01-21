#ifndef __PRODEVENTMETA2D_CXX__
#define __PRODEVENTMETA2D_CXX__

#include "ProdEventMeta2D.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"
#include "larcv/core/DataFormat/EventMeta.h"

namespace larcv {

static ProdEventMeta2DProcessFactory __global_ProdEventMeta2DProcessFactory__;

ProdEventMeta2D::ProdEventMeta2D(const std::string name) : ProcessBase(name) {}

void ProdEventMeta2D::configure(const PSet& cfg) {
  _image_producer  = cfg.get<std::string>("ImageProducer", "");
  _image_type      = cfg.get<std::string>("ImageType", "");
  _output_producer = cfg.get<std::string>("OutputProducer");
}

void ProdEventMeta2D::initialize() {}


bool ProdEventMeta2D::process(IOManager& mgr) {

  // Create a pointer to the reference data product:

  std::string data_type = "";
  std::string data_producer = "";
  std::vector<std::string> options = {"image2d", "sparse2d", "cluster2d"};

  // There are a couple scenarios.
  // Producer and Type both set. Just use that.
  // Producer set, type not set.  Find a matching type if possible.
  // Producer not set, type set.  Find a matching producer if possible.
  // Neither set.  Take any match possible.

  if (_image_producer != ""){
    // Producer is set.
    data_producer = _image_producer;

    if (_image_type != ""){
      // Type is set.
      data_type = _image_type;
    }
    else{
      // Type is not set.  Try to find a match.
      
      //  The list of options:
      
      // Loop over the options:
      for (auto & type_option : options){
        // Get all producers for this datatype:
        std::vector<std::string> _producer_list = mgr.producer_list(type_option);
        
        // Loop over the producers and look for a match:
        for (auto prod : _producer_list) {
          // Match found?
          if (prod == data_producer) {
            data_type = type_option;
            break;
          }
        }
        // Break the loop if we found one
        if (data_type != ""){
          break;
        }
      }


    }
  }
  else{
    // In this path, the producer is not set.

    if (_image_type != ""){
      // We have a type, at least.  Find any producer of that type.
      std::vector<std::string> _producer_list = mgr.producer_list(_image_type);
      if (_producer_list.size() != 0){
        data_producer = _producer_list.front();

      }

    }
    else{
      // We have no producer, and no datatype.
      // Try all options of datatype and look for any producer:
      for (auto & type_option : options){
        std::vector<std::string> _producer_list = mgr.producer_list(type_option);
        if (_producer_list.size() != 0){
          data_producer = _producer_list.front();
          data_type = type_option;
          break;
        }

      }
    }
  }


  if (data_type == "" || data_producer == "") {
    // We don't know what to do in this situation, so abandon hope:
    LARCV_CRITICAL() << "Producer " << data_producer
                     << " not in file, error producing viewer meta by product "
                     << data_type << "."
                     << std::endl;
    throw larbys();
  }


  auto output_meta = (EventMeta*)(mgr.get_data("meta", _output_producer));



  // Read in the data and cast it to the proper type:
  if (data_type == "image2d"){
    auto * reference = (EventImage2D*)(mgr.get_data(data_type, data_producer));
    int n_planes = reference->as_vector().size();

    std::vector<double> x_min(n_planes);
    std::vector<double> y_min(n_planes);
    std::vector<double> x_max(n_planes);
    std::vector<double> y_max(n_planes);

    std::vector<double> x_n_pixels(n_planes);
    std::vector<double> y_n_pixels(n_planes);



    for (auto image : reference->as_vector()) {
      auto meta = image.meta();
      x_min[meta.id()] = meta.min_x();
      y_min[meta.id()] = meta.min_y();
      x_max[meta.id()] = meta.max_x();
      y_max[meta.id()] = meta.max_y();
      x_n_pixels[meta.id()] = meta.cols();
      y_n_pixels[meta.id()] = meta.rows();
    }

    output_meta->store("n_planes", n_planes);
    output_meta->store("x_min", x_min);
    output_meta->store("y_min", y_min);
    output_meta->store("x_max", x_max);
    output_meta->store("y_max", y_max);
    output_meta->store("x_n_pixels", x_n_pixels);
    output_meta->store("y_n_pixels", y_n_pixels);

  }
  else if(data_type == "sparse2d"){
    auto * reference = (EventSparseTensor2D*)(mgr.get_data(data_type, data_producer));
    int n_planes = reference->as_vector().size();

    std::vector<double> x_min(n_planes);
    std::vector<double> y_min(n_planes);
    std::vector<double> x_max(n_planes);
    std::vector<double> y_max(n_planes);

    std::vector<double> x_n_pixels(n_planes);
    std::vector<double> y_n_pixels(n_planes);



    for (auto image : reference->as_vector()) {
      auto meta = image.meta();
      x_min[meta.id()] = meta.min_x();
      y_min[meta.id()] = meta.min_y();
      x_max[meta.id()] = meta.max_x();
      y_max[meta.id()] = meta.max_y();
      x_n_pixels[meta.id()] = meta.cols();
      y_n_pixels[meta.id()] = meta.rows();
    }

    output_meta->store("n_planes", n_planes);
    output_meta->store("x_min", x_min);
    output_meta->store("y_min", y_min);
    output_meta->store("x_max", x_max);
    output_meta->store("y_max", y_max);
    output_meta->store("x_n_pixels", x_n_pixels);
    output_meta->store("y_n_pixels", y_n_pixels);

  }
  else{
    auto * reference = (EventClusterPixel2D*)(mgr.get_data(data_type, data_producer));
    int n_planes = reference->as_vector().size();

    std::vector<double> x_min(n_planes);
    std::vector<double> y_min(n_planes);
    std::vector<double> x_max(n_planes);
    std::vector<double> y_max(n_planes);

    std::vector<double> x_n_pixels(n_planes);
    std::vector<double> y_n_pixels(n_planes);



    for (auto image : reference->as_vector()) {
      auto meta = image.meta();
      x_min[meta.id()] = meta.min_x();
      y_min[meta.id()] = meta.min_y();
      x_max[meta.id()] = meta.max_x();
      y_max[meta.id()] = meta.max_y();
      x_n_pixels[meta.id()] = meta.cols();
      y_n_pixels[meta.id()] = meta.rows();
    }

    output_meta->store("n_planes", n_planes);
    output_meta->store("x_min", x_min);
    output_meta->store("y_min", y_min);
    output_meta->store("x_max", x_max);
    output_meta->store("y_max", y_max);
    output_meta->store("x_n_pixels", x_n_pixels);
    output_meta->store("y_n_pixels", y_n_pixels);

  }
  
  
  




  // LARCV_NORMAL() << output_meta->event_key() << std::endl;

  return true;
}

void ProdEventMeta2D::finalize() {}
}
#endif
