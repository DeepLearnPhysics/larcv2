#ifndef __LARBYSIMAGEFILTER_CXX__
#define __LARBYSIMAGEFILTER_CXX__
#include "LArbysImageFilter.h"
#include "TFile.h"
#include "TTree.h"
namespace larcv {
  static LArbysImageFilterProcessFactory __global_LArbysImageFilterProcessFactory__;
  LArbysImageFilter::LArbysImageFilter(const std::string name)
    : ProcessBase(name),
      _larbysana_ptr(nullptr),
      _larbysext_ptr(nullptr),
      _vtx_ana(),
      _event_tree(nullptr),
      _vtx3d_tree(nullptr)
  {}
  void LArbysImageFilter::configure(const PSet& cfg)
  {
    _require_two_multiplicity = cfg.get<bool>("RequireMultiplicityTwo",true);
    _require_match            = cfg.get<bool>("RequireMatch",true);
    _require_fiducial         = cfg.get<bool>("RequireFiducial",true);
    _filtervertextreename     = cfg.get<std::string>("FilterVertexTreeName","FilterVertexTree");
    _filtereventtreename      = cfg.get<std::string>("FilterEventTreeName","FilterEventTree");
  }
  void LArbysImageFilter::initialize()
  {
    _vtx3d_tree = new TTree(_filtervertextreename.c_str(),"");
    _vtx3d_tree->Branch("run",&_run,"run/i");
    _vtx3d_tree->Branch("subrun",&_subrun,"subrun/i");
    _vtx3d_tree->Branch("event",&_event,"event/i");
    _vtx3d_tree->Branch("entry",&_entry,"entry/i");
    _vtx3d_tree->Branch("id",&_vtx3d_id,"id/i");
    _vtx3d_tree->Branch("type", &_vtx3d_type,"type/i");
    _vtx3d_tree->Branch("x",&_vtx3d_x,"x/D");
    _vtx3d_tree->Branch("y",&_vtx3d_y,"y/D");
    _vtx3d_tree->Branch("z",&_vtx3d_z,"z/D");
    _vtx3d_tree->Branch("vtx2d_x_v",&_vtx2d_x_v );
    _vtx3d_tree->Branch("vtx2d_y_v",&_vtx2d_y_v );
    _vtx3d_tree->Branch("multi_v",&_par_multi);

    _event_tree = new TTree(_filtereventtreename.c_str(),"");
    _event_tree->Branch("run",&_run,"run/i");
    _event_tree->Branch("subrun",&_subrun,"subrun/i");
    _event_tree->Branch("event",&_event,"event/i");
    _event_tree->Branch("entry",&_entry,"entry/i");
    _event_tree->Branch("Vertex3D_v",&_vertex3d_v);
    _event_tree->Branch("ParticleCluster_vvv",&_particle_cluster_vvv);
    _event_tree->Branch("TrackClusterCompound_vvv",&_track_compound_vvv);
  }
  void LArbysImageFilter::ClearVertex() {
    _vtx3d_id=kINVALID_INT;
    _vtx3d_n_planes=kINVALID_INT;
    _vtx3d_type=kINVALID_INT;
    _vtx3d_x=kINVALID_DOUBLE;
    _vtx3d_y=kINVALID_DOUBLE;
    _vtx3d_z=kINVALID_DOUBLE;
    _vtx2d_x_v.clear();
    _vtx2d_y_v.clear();
    _par_multi.clear();
  }
  void LArbysImageFilter::ClearEvent() {
    _vertex3d_v.clear();
    _vertex3d_v.reserve(_larbysana_ptr->Verticies().size());
    _particle_cluster_vvv.clear();
    _particle_cluster_vvv.reserve(_larbysana_ptr->Verticies().size());
    _track_compound_vvv.clear();
    _track_compound_vvv.reserve(_larbysana_ptr->Verticies().size());
  }
  void LArbysImageFilter::WriteOut(size_t vertexid) {
    const auto& vtx3d = _larbysana_ptr->Vertex(vertexid);
    _vertex3d_v.emplace_back(vtx3d);
    _particle_cluster_vvv.emplace_back(_larbysana_ptr->PlaneParticles(vertexid));
    _track_compound_vvv.emplace_back(_larbysana_ptr->PlaneTracks(vertexid));
    ClearVertex();
    _vtx3d_id=vertexid;
    _vtx3d_n_planes=vtx3d.num_planes;
    _vtx3d_type=(uint)vtx3d.type;
    _vtx3d_x=vtx3d.x;
    _vtx3d_y=vtx3d.y;
    _vtx3d_z=vtx3d.z;
    for(size_t plane=0; plane<3;  ++plane) {
      _vtx2d_x_v.push_back(vtx3d.cvtx2d_v[plane].center.x);
      _vtx2d_y_v.push_back(vtx3d.cvtx2d_v[plane].center.y);
      _par_multi.push_back(_larbysana_ptr->Particles(vertexid,plane).size());
    }
  }
  bool LArbysImageFilter::process(IOManager& mgr)
  {
    ClearEvent();
    const auto& event_id = mgr.event_id();
    _run    = (uint) event_id.run();
    _subrun = (uint) event_id.subrun();
    _event  = (uint) event_id.event();
    _entry =  (uint) mgr.current_entry();
    for(size_t vertexid=0; vertexid<_larbysana_ptr->Verticies().size(); ++vertexid) {
      std::vector<std::vector<larocv::data::ParticleCluster> > pcluster_vv;
      for(size_t planeid=0;planeid<3;++planeid)  {
	pcluster_vv.push_back(_larbysana_ptr->Particles(vertexid,planeid));
	_vtx_ana.ResetPlaneInfo(_larbysext_ptr->ADCMeta(planeid));
      }
      if (_require_two_multiplicity) { 
	auto multiplicity=_vtx_ana.RequireParticleCount(pcluster_vv,2,2);
	if (!multiplicity) continue;
      }
      if (_require_fiducial) {
	auto fiduciality=_vtx_ana.CheckFiducial(_larbysana_ptr->Vertex(vertexid));
	if (!fiduciality) continue;
      }
      if(_require_match) {
	auto match_vv = _vtx_ana.MatchClusters(pcluster_vv,_larbysext_ptr->ADCImages(),0.5,2,2);
	if (match_vv.empty()) continue;
      }
      WriteOut(vertexid);
      _vtx3d_tree->Fill();
    }
    LARCV_DEBUG()<<"Filtered "<<_larbysana_ptr->Verticies().size()<<" to "<<_vertex3d_v.size()<<std::endl;
    if (_vertex3d_v.empty()) return false;
    _event_tree->Fill();
    return true;
  }
  void LArbysImageFilter::finalize()
  {

    _event_tree->Write();
    _vtx3d_tree->Write();

    std::string oldfilename;
    std::string oldtreename;
	
    oldfilename = _larbysana_ptr->GetRootName();
    oldtreename = _larbysana_ptr->GetMCTreeName();
    
    TFile *oldfile = new TFile(oldfilename.c_str(),"read");
    TTree *oldtree = (TTree*)oldfile->Get(oldtreename.c_str());
    
    //_mc_tree->Print();
    _mc_tree = oldtree->CloneTree();
    //_mc_tree->CopyEntries(oldtree);
    ana_file().cd();
    _mc_tree->Write();
    
  }
}
#endif
