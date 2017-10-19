#ifndef __ROIMERGER_CXX__
#define __ROIMERGER_CXX__

#include "ROIMerger.h"
#include "Geo2D/Core/BoundingBox.h"
#include "DataFormat/ImageMeta.h"
#include "DataFormat/EventROI.h"
#include <algorithm>
#include <array>

namespace larcv {

  static ROIMergerProcessFactory __global_ROIMergerProcessFactory__;

  ROIMerger::ROIMerger(const std::string name)
    : ProcessBase(name)
  {}
    
  void ROIMerger::configure(const PSet& cfg)
  {
    LARCV_DEBUG() << "start" << std::endl;
    _input_roi_producer = cfg.get<std::string>("InputROIProducer");
    _output_roi_producer = cfg.get<std::string>("OutputROIProducer");
    _iou_threshold = cfg.get<float>("IOUThreshold",0.5);
    LARCV_DEBUG() << "end" << std::endl;
  }

  void ROIMerger::initialize()
  {}

  geo2d::Rect MakeRect(const larcv::ImageMeta& meta) {
    auto xl = meta.tl().x;
    auto yl = meta.tl().y;
    auto xr = meta.br().x;
    auto yr = meta.br().y;
    auto dx = xr - xl;
    auto dy = yl - yr;
    return geo2d::Rect(xl,yr,dx,dy);
  }
  
  bool ROIMerger::process(IOManager& mgr)
  {
    LARCV_DEBUG() << "start" << std::endl;
    if (_input_roi_producer.empty()) throw larbys("No roi producer specified");
    LARCV_DEBUG() << "Reading ev roi @ producer " << _input_roi_producer << std::endl;

    auto in_ev_roi = (EventROI*)(mgr.get_data(kProductROI,_input_roi_producer));
    if (!in_ev_roi) throw larbys("input ROI not found!");
    
    auto out_ev_roi = (EventROI*)(mgr.get_data(kProductROI,_output_roi_producer));
    if (!out_ev_roi) throw larbys("output ROI failed creation");


    const auto& in_roi_v = in_ev_roi->ROIArray();
    const size_t nrois=in_roi_v.size();
    
    std::vector<std::array<geo2d::Rect,3>> roi_rect_v;
    roi_rect_v.resize(nrois);
      
    std::vector<float> roi_size_v(nrois,0.0);
    
    for(size_t roiid=0;roiid<nrois;++roiid) {
      const auto& roi = in_roi_v[roiid];
      auto& roi_size = roi_size_v[roiid];
      for(size_t plane=0;plane<3;++plane) {
	const auto& bb = roi.BB(plane);
	LARCV_DEBUG() << "@ roi " << roiid << "," << bb.dump();
	auto& rect = roi_rect_v[roiid][plane];
	rect = MakeRect(bb);
	roi_size+=rect.area();
      }
    }

    size_t n=0;
    std::vector<size_t> roi_idx_v(nrois,kINVALID_SIZE);
    for(auto& v : roi_idx_v) {v = n; ++n;}
    
    std::sort(std::begin(roi_idx_v),std::end(roi_idx_v),
	      [&roi_size_v](size_t i1, size_t i2) { return roi_size_v[i1] > roi_size_v[i2]; } );

    /*
    std::stringstream ss;
    ss.str("");
    ss << "Area ordered ["; 
    for(auto srid : roi_idx_v) ss << srid << ",";
    ss << "]";
    LARCV_DEBUG() << ss.str() << std::endl;
    ss.str("");
    */
    

    std::vector<std::vector<std::pair<size_t,float> > > scores_pvv;
    scores_pvv.resize(nrois);
    
    for(size_t rid1=0;rid1<nrois;++rid1) {
      for(size_t rid2=0;rid2<nrois;++rid2) {
	if (rid1==rid2) continue;
	const auto& sroi1 = roi_rect_v[rid1];
	const auto& sroi2 = roi_rect_v[rid2];
	float iomin=0.0;
	for(size_t plane=0;plane<3;++plane) {
	  auto rect1 = sroi1[plane];
	  auto rect2 = sroi2[plane];
	  auto inter_ = geo2d::Intersection(rect1,rect2).area();
	  //auto union_ = geo2d::Union(rect1,rect2).area();
	  // if (union_>0)
	  iomin += inter_/std::min(rect1.area(),rect2.area());
	}
	scores_pvv[rid1].emplace_back(rid2,iomin/3.0);
      }
    }

    /*
    ss.str("")
    ss << "Scores " << std::endl;
    for(size_t id_=0;id_<scores_pvv.size();++id_) {
      auto& scores_pv = scores_pvv[id_];
      ss << "\t\t" << id_<<")";
      for(auto& scores_p : scores_pv) {
	ss << "("<<scores_p.first<<","<<scores_p.second<<") -- ";
      }
      ss << std::endl;
    }
    LARCV_DEBUG() << ss.str() << std::endl;
    */
    
    std::vector<std::set<size_t> > merge_id_vv;
    merge_id_vv.resize(nrois);
    
    for(auto srid : roi_idx_v) {
      std::vector<bool> seek_v(nrois,false);
      seek_v[srid]=true;
      auto& scores_pv = scores_pvv[srid];
      SearchScore(srid,srid,scores_pvv,merge_id_vv,seek_v);
    }

    /*
    ss << "Merging... " << std::endl;
    for(size_t merged_id=0;merged_id<merge_id_vv.size();++merged_id) {
      auto& merged_id_v = merge_id_vv[merged_id];
      ss << "\t\t" << merged_id<<") [";
      for(auto merged_id_ : merged_id_v)
	ss << merged_id_ << ",";
      ss << "]" << std::endl;
    }
    LARCV_DEBUG() << ss.str() << std::endl;
    ss.str("");
    */
    
    std::vector<bool> seek_v(nrois,false);
    for(auto srid : roi_idx_v) {
      if (seek_v[srid]) continue;
      seek_v[srid]=true;
      auto roi = in_roi_v[srid];
      auto res_meta_v = roi.BB();
      for(auto merge_id : merge_id_vv[srid]) {
	seek_v[merge_id]=true;
	for(size_t plane=0;plane<3;++plane) {
	  const auto& merge_meta = in_roi_v[merge_id].BB(plane);
	  auto& res_meta = res_meta_v[plane];
	  res_meta = res_meta.inclusive(merge_meta);
	}
      }
      roi.SetBB(res_meta_v);
      out_ev_roi->Emplace(std::move(roi));
    }

    return true;
  }

  void ROIMerger::SearchScore(size_t this_id,
			      size_t that_id,
			      std::vector<std::vector<std::pair<size_t,float> > >& scores_pvv,
			      std::vector<std::set<size_t> >& merge_id_vv,
			      std::vector<bool>& seek_v) {
    for(auto score : scores_pvv[that_id]) {
      if ( seek_v[score.first] ) continue;
      seek_v[score.first] = true;
      if (score.second<_iou_threshold)  continue;
      merge_id_vv[this_id].insert(score.first);
      SearchScore(this_id,that_id,scores_pvv,merge_id_vv,seek_v);
    }
    return;
  }

  
  void ROIMerger::finalize()
  {}

}
#endif
