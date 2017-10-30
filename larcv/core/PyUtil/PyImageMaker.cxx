#ifndef __PYIMAGEMAKER_CXX__
#define __PYIMAGEMAKER_CXX__

#include "PyImageMaker.h"
#include "larcv/core/DataFormat/EventImage2D.h"

namespace larcv {

  static PyImageMakerProcessFactory __global_PyImageMakerProcessFactory__;

  PyImageMaker::PyImageMaker(const std::string name)
    : ProcessBase(name)
  {}
    
  void PyImageMaker::configure(const PSet& cfg)
  { _producer_name = cfg.get<std::string>("ImageProducer"); }

  void PyImageMaker::initialize()
  {
    _run = _subrun = _event = kINVALID_SIZE;
    _image_v.clear();
  }
  
  void PyImageMaker::append_ndarray(PyObject* img)
  { 
    LARCV_INFO() << "Appending an image (current size=" << _image_v.size() << ")" << std::endl;
    _image_v.emplace_back(std::move(as_image2d(img))); 
  }

  void PyImageMaker::append_ndarray_meta(PyObject* img,const ImageMeta& meta)
  { 
    LARCV_INFO() << "Appending an image (current size=" << _image_v.size() << ")" << std::endl;
    _image_v.emplace_back(std::move(as_image2d_meta(img,meta))); 
  }

  bool PyImageMaker::process(IOManager& mgr)
  {
    auto image_v = (EventImage2D*)(mgr.get_data("image2d",_producer_name));

    if(!image_v) {
      LARCV_CRITICAL() << "Could not create Image2D with name " << _producer_name << std::endl;
      throw larbys();
    }

    if(image_v->image2d_array().size()) {
      LARCV_CRITICAL() << "Image2D by " << _producer_name << " is not empty!" << std::endl;
      throw larbys();
    }

    LARCV_INFO() << "Moving " << _image_v.size() << " images into EventImage2D..." << std::endl;
    
    image_v->emplace(std::move(_image_v));

    _image_v.clear();

    auto const& id = mgr.event_id();
    if(id.run() != kINVALID_SIZE || id.subrun() != kINVALID_SIZE || id.event() != kINVALID_SIZE) {
      if(id.run()    != _run    ) LARCV_WARNING() << "Overwriting run number: "    << id.run()    << " => " << _run    << std::endl;
      if(id.subrun() != _subrun ) LARCV_WARNING() << "Overwriting subrun number: " << id.subrun() << " => " << _subrun << std::endl;
      if(id.event()  != _event  ) LARCV_WARNING() << "Overwriting event number: "  << id.event()  << " => " << _event  << std::endl;
    }

    mgr.set_id(_run,_subrun,_event);

    _run = _subrun = _event = kINVALID_SIZE;
    return true;
  }

  void PyImageMaker::finalize()
  {}

}
#endif
