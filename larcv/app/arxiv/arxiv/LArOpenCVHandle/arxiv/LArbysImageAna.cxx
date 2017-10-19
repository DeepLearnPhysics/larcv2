#ifndef __LARBYSIMAGEANA_CXX__
#define __LARBYSIMAGEANA_CXX__

#include "LArbysImageAna.h"

namespace larcv {

  static LArbysImageAnaProcessFactory __global_LArbysImageAnaProcessFactory__;

  LArbysImageAna::LArbysImageAna(const std::string name)
    : ProcessBase(name),
      _mc_chain(nullptr),
      _reco_chain(nullptr),
      _vtx2d_w_v(nullptr),
      _vtx2d_t_v(nullptr),
      _reco_vertex_v(nullptr),
      _particle_cluster_vvv(nullptr),
      _track_cluster_comp_vvv(nullptr)
  {}
  
  void LArbysImageAna::configure(const PSet& cfg)
  {
    _mc_tree_name = cfg.get<std::string>("MCTreeName");
    _reco_tree_name = cfg.get<std::string>("RecoTreeName");
    _mc_exists = cfg.get<bool>("MCExists");
  }

  void LArbysImageAna::initialize()
  {

    if (_mc_exists) {
      if (_input_larbys_mc_root_file.empty()) throw larbys("No input root file specified");
      LARCV_DEBUG() << "Setting input MC ROOT file: " << _input_larbys_mc_root_file << std::endl;
      _mc_chain = new TChain(_mc_tree_name.c_str());
      _mc_chain->AddFile(_input_larbys_mc_root_file.c_str());
      _mc_chain->SetBranchAddress("run",&_mc_run);
      _mc_chain->SetBranchAddress("subrun",&_mc_subrun);
      _mc_chain->SetBranchAddress("event",&_mc_event);
      _mc_chain->SetBranchAddress("entry",&_mc_entry);
      _mc_chain->SetBranchAddress("parentX",&_true_x);
      _mc_chain->SetBranchAddress("parentY",&_true_y);
      _mc_chain->SetBranchAddress("parentZ",&_true_z);
      _mc_chain->SetBranchAddress("vtx2d_w",&_vtx2d_w_v);
      _mc_chain->SetBranchAddress("vtx2d_t",&_vtx2d_t_v);
      _mc_index=0;
      _mc_chain->GetEntry(_mc_index);
      _mc_entries = _mc_chain->GetEntries();
      LARCV_DEBUG() << "Got " << _reco_entries << " mc entries" << std::endl;
    }
    
    if (_input_larbys_reco_root_file.empty()) throw larbys("No input root file specified");
    LARCV_DEBUG() << "Setting input Reco ROOT file: " << _input_larbys_reco_root_file << std::endl;
    _reco_chain = new TChain(_reco_tree_name.c_str());
    _reco_chain->AddFile(_input_larbys_reco_root_file.c_str());
    _reco_chain->SetBranchAddress("run",&_reco_run);
    _reco_chain->SetBranchAddress("subrun",&_reco_subrun);
    _reco_chain->SetBranchAddress("event",&_reco_event);
    _reco_chain->SetBranchAddress("entry",&_reco_entry);
    _reco_chain->SetBranchAddress("Vertex3D_v",&_reco_vertex_v);
    _reco_chain->SetBranchAddress("ParticleCluster_vvv",&_particle_cluster_vvv);
    _reco_chain->SetBranchAddress("TrackClusterCompound_vvv",&_track_cluster_comp_vvv);
    _reco_index=0;
    _reco_chain->GetEntry(_reco_index);
    _reco_entries = _reco_chain->GetEntries();
    LARCV_DEBUG() << "Got " << _reco_entries << " reco entries" << std::endl;
  }

  bool LArbysImageAna::increment(uint entry)
  {
    if(_mc_exists)
      _mc_chain->GetEntry(_mc_index);
    _reco_chain->GetEntry(_reco_index);

    if ((_mc_entry   <= entry) && (_mc_index   != _mc_entries))   { _mc_index++;   }
    if ((_reco_entry <= entry) && (_reco_index != _reco_entries)) { _reco_index++; }

    if ( entry     != _reco_entry) return false;
    if (_mc_exists) {
      if ( entry     !=  _mc_entry ) return false;
      if ( _mc_entry != _reco_entry) return false;
    }

    return true;
  }
  
  bool LArbysImageAna::process(IOManager& mgr)
  {
    uint entry = mgr.current_entry();

    LARCV_DEBUG() << "(this,mc,reco) entry & index @ (" << entry <<","<<_mc_entry<<","<<_reco_entry<<")"
		  << " & " << "(-,"<<_mc_index<<","<<_reco_index<<")"<<std::endl;

    if ( !increment(entry) ) return false;

    const auto& event_id = mgr.event_id();
    auto run    = event_id.run();
    auto subrun = event_id.subrun();
    auto event  = event_id.event();

    LARCV_DEBUG()<<"Passed @ (r,s,e,e) -- ["<<run<<","<<subrun<<","<<event<<","<<entry<<"]"<<std::endl;
    
    if (!_mc_exists)
      return true;
    
    _mc_vertex._Clear_();
    _mc_vertex.x=_true_x;
    _mc_vertex.y=_true_y;
    _mc_vertex.z=_true_z;
    _mc_vertex.vtx2d_v.clear();

    for(size_t plane=0;plane<_vtx2d_w_v->size();++plane) {
      larocv::data::Vertex2D vtx2d;
      vtx2d.pt.x=(*_vtx2d_t_v)[plane];
      vtx2d.pt.y=(*_vtx2d_w_v)[plane];
      _mc_vertex.vtx2d_v.emplace_back(std::move(vtx2d));
    }
    
    return true;
  }
  
  void LArbysImageAna::finalize()
  {}

}
#endif
