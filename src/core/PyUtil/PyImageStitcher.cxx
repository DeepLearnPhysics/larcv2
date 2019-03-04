#ifndef __PYIMAGESTITCHER_CXX__
#define __PYIMAGESTITCHER_CXX__

#include "PyImageStitcher.h"
#include "larcv/core/DataFormat/EventImage2D.h"

namespace larcv {

  static PyImageStitcherProcessFactory __global_PyImageStitcherProcessFactory__;

  PyImageStitcher::PyImageStitcher(const std::string name)
    : ProcessBase(name)
  {}
    
  void PyImageStitcher::configure(const PSet& cfg)
  { _producer_name = cfg.get<std::string>("ImageProducer"); }

  void PyImageStitcher::initialize()
  {
    _run = _subrun = _event = kINVALID_SIZE;
    _image_vv.clear();
  }
  
  void PyImageStitcher::append_ndarray(PyObject* img,size_t ch)
  { 
    LARCV_INFO() << "Appending an image (ch =" << _image_vv.size() << ")" << std::endl;
    if(_image_vv.size() <= ch) _image_vv.resize(ch+1);
    _image_vv[ch].emplace_back(std::move(as_image2d(img))); 
  }

  void PyImageStitcher::append_ndarray_meta(PyObject* img,const ImageMeta& meta, size_t ch)
  { 
    LARCV_INFO() << "Appending an image (ch =" << ch << ")" << std::endl;
    if(_image_vv.size() <= ch) _image_vv.resize(ch+1);
    _image_vv[ch].emplace_back(std::move(as_image2d_meta(img,meta))); 
  }

  bool PyImageStitcher::process(IOManager& mgr)
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

    auto const& id = mgr.event_id();
    if(id.run() != kINVALID_SIZE || id.subrun() != kINVALID_SIZE || id.event() != kINVALID_SIZE) {
      if(id.run()    != _run    ) LARCV_WARNING() << "Overwriting run    number: " << id.run()    << " => " << _run    << std::endl;
      if(id.subrun() != _subrun ) LARCV_WARNING() << "Overwriting subrun number: " << id.subrun() << " => " << _subrun << std::endl;
      if(id.event()  != _event  ) LARCV_WARNING() << "Overwriting event  number: " << id.event()  << " => " << _event  << std::endl;
    }

    mgr.set_id(_run,_subrun,_event);

    _run = _subrun = _event = kINVALID_SIZE;

    std::vector<larcv::Image2D> out_img_v;

    for(size_t ch=0; ch<_image_vv.size(); ++ch) {
      
      if(_image_vv[ch].empty()) {
	out_img_v.push_back(Image2D());
	continue;
      }

      ImageMeta meta_combined = _image_vv[ch].front().meta();
      for(auto const& img : _image_vv[ch])
	meta_combined = meta_combined.inclusive(img.meta());
      
      Image2D out_img(meta_combined);
      out_img.paint(0.);
      for(auto const& img : _image_vv[ch])
	out_img.overlay(img,Image2D::kOverWrite);

      out_img_v.emplace_back(std::move(out_img));
    }

    image_v->emplace(std::move(out_img_v));
    _image_vv.clear();
    return true;
  }

  void PyImageStitcher::finalize()
  {}

}
#endif
