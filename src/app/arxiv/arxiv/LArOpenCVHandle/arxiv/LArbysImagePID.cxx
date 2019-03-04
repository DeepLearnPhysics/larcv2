#ifndef __LARBYSIMAGEPID_CXX__
#define __LARBYSIMAGEPID_CXX__

#include "LArbysImagePID.h"

namespace larcv {

  static LArbysImagePIDProcessFactory __global_LArbysImagePIDProcessFactory__;

  LArbysImagePID::LArbysImagePID(const std::string name, 
				 const std::vector<std::string> particle_types, 
				 const std::vector<std::string> interaction_types)
    : ProcessBase(name)
    , _particle_types_v(particle_types)
    , _interaction_types_v(interaction_types)
  {}
    
  void LArbysImagePID::Clear(){
    _ptype_ctr_v.clear();
    _ptype_ctr_v.resize(5,0);

    _rois_score_vv.clear();
    _rois_score_vv.resize(2);
    _ptype_scores_vvv.clear();
    
    _score_sum_v.clear();
    
    _interaction_pair_v.clear();

    //_roi_score.resize(5,0.0);
    //_rois_score_vv[0]=_roi_score;
    //_rois_score_vv[1]=_roi_score;

  }
  
  void LArbysImagePID::configure(const PSet& cfg)
  {
    _use_shape = cfg.get<bool>("UseShape");
    _verbosity = cfg.get<bool>("Verbosity");
  }

  void LArbysImagePID::initialize()
  {}

  bool LArbysImagePID::process(IOManager& mgr)
  {
    Clear();
    _ptype_ctr_v.resize(5,0);
    
    auto ev_pgraph = (EventPGraph*)mgr.get_data(kProductPGraph,"test");
    _run    = ev_pgraph->run();
    _subrun = ev_pgraph->subrun();
    _event  = ev_pgraph->event();
    _entry  = mgr.current_entry();
    
    auto pgraphs = ev_pgraph->PGraphArray();
    
    LARCV_DEBUG()<< "Entry: "<<_entry
		 <<", Run: "<<_run
		 <<", Subrun: "<<_subrun
		 <<", Event: "<<_event
		 <<", has "<<pgraphs.size()<<" vertices."
		 <<std::endl;

    _ptype_scores_vvv.resize(pgraphs.size());
    _score_sum_v.resize(pgraphs.size(),0.0);
    LARCV_DEBUG()<<"number of vtx is "<<pgraphs.size()<<std::endl;
    std::vector<std::vector<float>> ptype_scor(2, std::vector<float>(5,0.0));
    for(auto &ptype_score : _ptype_scores_vvv) ptype_score = ptype_scor;
        
    for (size_t vtx_idx=0; vtx_idx< pgraphs.size(); ++vtx_idx)
      {
	auto rois_obj = pgraphs[vtx_idx];
	if (rois_obj.NumParticles()!=2){ 
	  LARCV_CRITICAL()<<"    The vertex has >>>> "<< rois_obj.NumParticles()<<" <<<< rois !"<<std::endl;
	  //throw larbys("Go to match, LArbysImageFilter(LArbysImage)");
	}
	
	auto rois = rois_obj.ParticleArray();
	for (int roi_idx=0; roi_idx<2; ++roi_idx){
	  auto roi = rois[roi_idx];
	  _rois_score_vv[roi_idx] = roi.TypeScore();
	  if (_use_shape){
	    if (roi.Shape()==0&& roi.Type()==0){
	      _ptype_ctr_v[roi.Type()]++;  
	      _ptype_scores_vvv[vtx_idx][roi_idx][roi.Type()]=roi.TypeScore()[roi.Type()];
	    }
	    if (roi.Shape()==0&& roi.Type()==1){
	      _ptype_ctr_v[roi.Type()]++;  
	      _ptype_scores_vvv[vtx_idx][roi_idx][roi.Type()]=roi.TypeScore()[roi.Type()];
	    }
	    if (roi.Shape()==1&& roi.Type()==2){
	      _ptype_ctr_v[roi.Type()]++;  
	      _ptype_scores_vvv[vtx_idx][roi_idx][roi.Type()]=roi.TypeScore()[roi.Type()];
	    }
	    if (roi.Shape()==0&& roi.Type()==3){
	      _ptype_ctr_v[roi.Type()]++;  
	      _ptype_scores_vvv[vtx_idx][roi_idx][roi.Type()]=roi.TypeScore()[roi.Type()];
	    }
	    if (roi.Shape()==1&& roi.Type()==4){
	      _ptype_ctr_v[roi.Type()]++;  
	      _ptype_scores_vvv[vtx_idx][roi_idx][roi.Type()]=roi.TypeScore()[roi.Type()];
	    }
	  }
	  if(!_use_shape){
	    for (int type=0; type<5; ++type){
	      if (roi.Type()==type) {
		_ptype_ctr_v[type]++;
		_ptype_scores_vvv[vtx_idx][roi_idx][type]=roi.TypeScore()[type];
		_score_sum_v[vtx_idx]+=roi.TypeScore()[type];
	      }
	    }
	  }
	  //LARCV_DEBUG()<<roi.Index()<<std::endl;
	  //LARCV_DEBUG()<<particle_types[roi.Type()]<<std::endl;
	}
	
	for (auto x : _ptype_ctr_v) if(_verbosity) std::cout<< x <<" ";
	if(_verbosity) std::cout<<std::endl;
	for (auto scores : _rois_score_vv) {
	  for (auto score : scores) if(_verbosity) std::cout<<score<<" ";
	  if(_verbosity) std::cout<<std::endl;
	}
	if(_verbosity) std::cout<<std::endl;
	for (auto scores : _ptype_scores_vvv) {
	  for (auto score : scores) {
	    for (auto scor : score)if(_verbosity) std::cout<<scor<<" ";
	  }
	  if(_verbosity) std::cout<<std::endl;
	}
	if(_verbosity) std::cout<<std::endl;
	_ptype_ctr_v.clear();
	_ptype_ctr_v.resize(5,0);
      }
    
    return true;
  }
  
  void LArbysImagePID::finalize()
  {}

}
#endif
