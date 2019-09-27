#ifndef __WeightKeyPoint2D_CXX__
#define __WeightKeyPoint2D_CXX__

#include "WeightKeyPoint2D.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"

namespace larcv {

  static WeightKeyPoint2DProcessFactory __global_WeightKeyPoint2DProcessFactory__;

  WeightKeyPoint2D::WeightKeyPoint2D(const std::string name)
    : ProcessBase(name)
  {}

  void WeightKeyPoint2D::configure(const PSet& cfg)
  {
    _weight_value     = cfg.get<float>("WeightValue",1.);
    _weight_range     = cfg.get<int>  ("WeightRange",6);
    _output_producer  = cfg.get<std::string>("OutputProducer");
    _segment_producer = cfg.get<std::string>("SegmentProducer");
    _weight_producer  = cfg.get<std::string>("WeightProducer","");
  }

  void WeightKeyPoint2D::initialize()
  {}

  bool WeightKeyPoint2D::process(IOManager& mgr)
  {
    auto& event_segment = mgr.get_data<larcv::EventSparseTensor2D>(_segment_producer);

    std::vector<larcv::SparseTensor2D> tensor2d_v;

    if(!_weight_producer.empty()) {
      auto const& event_weight = mgr.get_data<larcv::EventSparseTensor2D>(_weight_producer);
      tensor2d_v = event_weight.as_vector();

      // check dimensionality
      if(tensor2d_v.size() != event_segment.as_vector().size()) {
	LARCV_CRITICAL() << "Weight tensor2d count (" << event_weight.as_vector().size()
			 << ") mismatch with Segment tensor2d count (" << event_segment.as_vector().size()
			 << ")" << std::endl;
	throw larbys();
      }
    }else{
      for(auto const& segment2d : event_segment.as_vector()) {
	larcv::SparseTensor2D tensor2d;
	tensor2d.meta(segment2d.meta());
	tensor2d_v.push_back(tensor2d);
      }
    }

    // verify meta
    for(size_t i=0; i<tensor2d_v.size(); ++i) {
      if(tensor2d_v[i].meta() == event_segment.as_vector()[i].meta()) continue;
      LARCV_CRITICAL() << "Meta mismatch at " << i << "-th tensor2d! " << std::endl;
      throw larbys();
    }

    // loop over tensor2ds
    for(size_t tensor_idx=0; tensor_idx<tensor2d_v.size(); ++tensor_idx) {
      auto& tensor2d = tensor2d_v[tensor_idx];
      auto const& segment2d = event_segment.as_vector()[tensor_idx];
      auto const& meta = tensor2d.meta();
      size_t row, col;
      int irow, icol;
      bool weight=false;
      for(auto const& vox : segment2d.as_vector()) {
	// check surrounding voxels
	meta.index_to_rowcol(vox.id(),row,col);
	irow = (int)row;
	icol = (int)col;
	weight=false;
	// check if this voxel's neighbor has different segment value
	for(int x=icol-1; x<=icol+1; ++x) {
	  if(x<0 || x>=((int)(meta.cols()))) continue;
	  for(int y=irow-1; y<=irow+1; ++y) {
	    if(y<0 || y>=((int)(meta.rows()))) continue;
	    auto const& neighbor = segment2d.find(meta.index((size_t)y,(size_t)x));
	    if(neighbor.id() == larcv::kINVALID_VOXELID) continue;
	    if(neighbor.value() == vox.value()) continue;
	    weight=true;
	    break;
	  }
	  if(weight) break;
	}
	// if so, weight neighbors
	if(weight) {
	  for(int x=icol - _weight_range; x<=icol + _weight_range; ++x) {
	    if(x<0 || x>=((int)(meta.cols()))) continue;
	    for(int y=irow - _weight_range; y<=irow + _weight_range; ++y) {
	      if(y<0 || y>=((int)(meta.rows()))) continue;
	      auto const& neighbor = segment2d.find(meta.index((size_t)y,(size_t)x));
	      if(neighbor.id() == larcv::kINVALID_VOXELID) continue;
	      tensor2d.emplace(neighbor.id(),_weight_value,false);
	    }
	  }
	}
      }
    }

    // store output
    auto& event_output = mgr.get_data<larcv::EventSparseTensor2D>(_output_producer);
    for(size_t i=0; i<tensor2d_v.size(); ++i)
      event_output.emplace(std::move(tensor2d_v[i]));

    return true;
  }

  void WeightKeyPoint2D::finalize()
  {}

}
#endif
