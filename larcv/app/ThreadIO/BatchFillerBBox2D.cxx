#ifndef __BatchFillerBBox2D_CXX__
#define __BatchFillerBBox2D_CXX__

#include "BatchFillerBBox2D.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventBBox.h"
#include <random>

namespace larcv {

  static BatchFillerBBox2DProcessFactory __global_BatchFillerBBox2DProcessFactory__;

  BatchFillerBBox2D::BatchFillerBBox2D(const std::string name)
    : BatchFillerTemplate<float>(name)
  {}

  void BatchFillerBBox2D::configure(const PSet& cfg)
  {
    _bbox2d_producer = cfg.get<std::string>("BBox2DProducer");
    _projid = (ProjectionID_t)cfg.get<int>("ProjectionID");
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
    
    auto const& event_bbox2d = mgr.get_data<larcv::EventBBox2D>(_bbox2d_producer);

    // if slow, one thing to try is to allocate this only once
    // by keeping as member data
    std::vector< std::array<float,4> > tempstore;
    tempstore.reserve( event_bbox2d.size() );


    for ( auto const& bbox2d : event_bbox2d ) {
      if ( bbox2d.id()!=_projid )
	continue;

      std::array<float,4> xybh;
      xybh[0] = bbox2d.center_x();
      xybh[1] = bbox2d.center_y();
      xybh[2] = bbox2d.width();
      xybh[3] = bbox2d.height();

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
