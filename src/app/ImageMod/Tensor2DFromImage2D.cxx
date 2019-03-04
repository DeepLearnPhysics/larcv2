#ifndef __TENSOR2DFROMIMAGE2D_CXX__
#define __TENSOR2DFROMIMAGE2D_CXX__

#include "Tensor2DFromImage2D.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"
#include "larcv/core/DataFormat/EventImage2D.h"
namespace larcv {

  static Tensor2DFromImage2DProcessFactory
  __global_Tensor2DFromImage2DProcessFactory__;

  Tensor2DFromImage2D::Tensor2DFromImage2D(const std::string name)
    : ProcessBase(name) {}

  void Tensor2DFromImage2D::configure_labels(const PSet& cfg)
  {

    _image2d_producer   = cfg.get<std::string>("ImageProducer", "");
    _output_producer    = cfg.get<std::string>("OutputProducer", "");
    _reference_tensor2d = cfg.get<std::string>("ReferenceTensor2D", "");

    if (_image2d_producer == ""){
      LARCV_CRITICAL() << "You must specify an image producer!" << std::endl;
      throw larbys();      
    }

    if (_output_producer == ""){
      _output_producer = _image2d_producer + "tensor2d";
      LARCV_WARNING() << "OutputProducer not specified, using " << _output_producer << std::endl;
    }
    
  }

  void Tensor2DFromImage2D::configure(const PSet& cfg) {
    configure_labels(cfg);

    _thresholds_v     = cfg.get<std::vector<float> >("Thresholds",_thresholds_v);
    _projection_ids_v = cfg.get<std::vector<int> >  ("ProjectionIDs",_projection_ids_v);

    // If there are not projection IDs specified, the default is to use all of them
    // If there are not thresholds specified, the default is to not apply thresholding
    // and only prune non zero pixels

    // Thresholding is applied to the absolute value of a pixel

    if (!_thresholds_v.empty() && ! _projection_ids_v.empty() && _thresholds_v.size() != _projection_ids_v.size()){
      LARCV_CRITICAL() << "Specified Thesholds and projections IDs must have the same length!" << std::endl;
      throw larbys();
    }

  }

  void Tensor2DFromImage2D::initialize() {}

  bool Tensor2DFromImage2D::process(IOManager& mgr) {


    // Get the image 2d data from the specified producer.
    auto const& ev_image2d =
      mgr.get_data<larcv::EventImage2D>(_image2d_producer);


    // Get the reference tensor 2d, if it's specified:
    larcv::EventSparseTensor2D * reference_tensor = NULL;
    if (_reference_tensor2d != ""){
      reference_tensor = & mgr.get_data<larcv::EventSparseTensor2D>(_reference_tensor2d);
    }

    if (_reference_tensor2d != "" && (reference_tensor == NULL || reference_tensor->as_vector().size() == 0) ){
      LARCV_CRITICAL() << "Reference tensor2d by " << _reference_tensor2d << " requested but not found." << std::endl;
      throw larbys();
    }

    if (_reference_tensor2d != "" && reference_tensor -> as_vector().size() != ev_image2d.as_vector().size()){
      LARCV_CRITICAL() << "Reference tensor2d by " << _reference_tensor2d 
                       << " is not the same size as image by " << _image2d_producer <<"." << std::endl;
      throw larbys();
    }

    // Create a placeholder for the output tensor2d
    auto& ev_output = mgr.get_data<larcv::EventSparseTensor2D>(_output_producer);



    // Loop over projection IDs, and convert each image into a tensor 2D and preserve the image meta.
    for (size_t projection_id = 0; projection_id < ev_image2d.as_vector().size(); projection_id ++){
      
      auto image2d = ev_image2d.at(projection_id);
      auto meta = image2d.meta();

      larcv::VoxelSet vs;

      float this_threshold = 0.0;

      if (_thresholds_v.size() != 0){
        this_threshold = _thresholds_v.at(projection_id);
      }

      // If there is no reference tensor 2d, use non zero pixels:
      // Loop over the pixels in the image and convert the non-zero ones into voxels, add them to the voxel set:
      if (_reference_tensor2d == ""){
        for (size_t i = 0; i < image2d.size(); ++i){
          if (fabs(image2d.as_vector().at(i)) > this_threshold){
            vs.add(larcv::Voxel(i, image2d.as_vector().at(i)));
          }
        }
      }
      else{
        // if there is a reference 2d, use the indexes from it to pick points from the image:
        auto & reference_set = reference_tensor->sparse_tensor_2d(projection_id);
        if (reference_set.meta().size() != meta.size()){
          LARCV_CRITICAL() << "Meta size mismatch between target image and reference voxel set" << std::endl;
          throw larbys();
        }
        for (auto & ref_voxel : reference_set.as_vector()){
          float value = image2d.as_vector().at(ref_voxel.id());
          vs.add(larcv::Voxel(ref_voxel.id(), value) );
        }
      }


    
      // Add this voxel set to the output:
      ev_output.emplace(std::move(vs), std::move(meta));

    }



    // The end



    return true;
  }

  void Tensor2DFromImage2D::finalize() {}
}
#endif
