#ifndef __BatchFillerBBox2D_CXX__
#define __BatchFillerBBox2D_CXX__

#include "BatchFillerBBox2D.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventBBox.h"
#include <random>
#include <array>

namespace larcv {

  static BatchFillerBBox2DProcessFactory __global_BatchFillerBBox2DProcessFactory__;

  BatchFillerBBox2D::BatchFillerBBox2D(const std::string name)
    : BatchFillerTemplate<float>(name)
  {}

  void BatchFillerBBox2D::configure(const PSet& cfg)
  {
    _bbox2d_producer = cfg.get<std::string>("BBox2DProducer");
    _projid = (ProjectionID_t)cfg.get<int>("ProjectionID");
    _convert_xy_to_pix = cfg.get<bool>("ConvertXYtoPixel",false);
    _imageproducer = cfg.get<std::string>("ImageProducerForMeta","");
    _maxboxes = cfg.get<int>("MaxNumBoxes");
  }

  void BatchFillerBBox2D::initialize()
  {}

  void BatchFillerBBox2D::_batch_begin_()
  {
    
    
    if (!batch_data().dim().empty() && (int)(batch_size()) != batch_data().dim().front()) {
      auto dim = batch_data().dim();
      dim[0] = batch_size();
      this->set_dim(dim);
    }

    _entry_data.resize(batch_size()*4*_maxboxes,0);    
  }

  void BatchFillerBBox2D::_batch_end_()
  {
  }

  void BatchFillerBBox2D::finalize()
  {}

  bool BatchFillerBBox2D::process(IOManager & mgr)
  {

    LARCV_DEBUG() << "start" << std::endl;

    // get the bboxes for the entry
    auto const& event_bbox2d = mgr.get_data<larcv::EventBBox2D>(_bbox2d_producer);

    // if we need to convert coordinate systems
    //  from (x,y) to (row,col)
    //  we need the image2d event container as well
    larcv::EventImage2D* ev_img2d = NULL;
    std::vector< const larcv::ImageMeta* > meta_v;
    if ( _convert_xy_to_pix ) {
      ev_img2d = (larcv::EventImage2D*)mgr.get_data("image2d",_imageproducer);

      // we need the metas for the projections
      // whats't he max id?
      int maxid = 0 ;
      for ( auto const& img : ev_img2d->image2d_array() ) {
	if ( maxid<img.meta().id() )
	  maxid = img.meta().id();
      }
      meta_v.resize( maxid+1, 0 );
      // store pointers to the metas
      for ( auto const& img : ev_img2d->image2d_array() ) {
	if ( !meta_v[img.meta().id()] )
	  meta_v[img.meta().id()] = &(img.meta());
      }
    }

    // if slow, one thing to try is to allocate this only once
    // by keeping as member data
    std::vector< std::array<float,4> > tempstore;
    tempstore.reserve( event_bbox2d.size() );

    for ( auto const& bbox2d : event_bbox2d ) {
      if ( bbox2d.id()!=_projid )
	continue;
      
      const ImageMeta* pmeta = meta_v[bbox2d.id()];
      
      std::array<float,4> xybh;
      xybh[0] = bbox2d.center_x();
      xybh[1] = bbox2d.center_y();
      xybh[2] = 0.5*bbox2d.width();
      xybh[3] = 0.5*bbox2d.height();

      if  (_convert_xy_to_pix ) {
	xybh[0] = pmeta->col( xybh[0] );
	xybh[1] = pmeta->row( xybh[1] );
	xybh[2] /= pmeta->pixel_width();
	xybh[3] /= pmeta->pixel_height();
      }
      
      tempstore.push_back( xybh );
    }
      
    int nboxes = tempstore.size();
    LARCV_DEBUG() << "Number of boxes in event: " << nboxes << std::endl;    
    if ( nboxes>_maxboxes )
      nboxes = _maxboxes;
    
    if (batch_data().dim().empty()) {
      LARCV_DEBUG() << "Setting batch size" << std::endl;
      std::vector<int> dim(4);
      dim[0] = batch_size();
      dim[1] = _maxboxes;
      dim[2] = 1;
      dim[3] = 4;
      this->set_dim(dim);
    }
    _entry_data.resize(4*_maxboxes,0);
    for (auto& v : _entry_data) v = 0.;
	  
    for ( int ibox=0; ibox<nboxes; ibox++ ) {
      auto& bbox2d = tempstore[ibox];
      for (int i=0; i<4; i++) 
	_entry_data[4*ibox+i] = bbox2d[i];
    }
    this->set_entry_data(_entry_data);

    LARCV_DEBUG() << "end" << std::endl;    
    return true;
  }

}
#endif
