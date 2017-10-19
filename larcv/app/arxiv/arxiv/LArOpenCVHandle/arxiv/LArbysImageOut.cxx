#ifndef __LARBYSIMAGEOUT_CXX__
#define __LARBYSIMAGEOUT_CXX__
#include "LArbysImageOut.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "CVUtil/CVUtil.h"

namespace larcv {

  static LArbysImageOutProcessFactory __global_LArbysImageOutProcessFactory__;
  
  LArbysImageOut::LArbysImageOut(const std::string name)
    : ProcessBase(name),
      _vtx3d_tree(nullptr)
  {}
  
  void LArbysImageOut::configure(const PSet& cfg)
  {}
  
  void LArbysImageOut::ClearVertex() {
    _vtx2d_x_v.clear();
    _vtx2d_y_v.clear();
    _circle_x_v.clear();
    _circle_y_v.clear();
    _vtx2d_x_v.resize(3);
    _vtx2d_y_v.resize(3);
    _circle_x_v.resize(3);
    _circle_y_v.resize(3);
    _circle_xs_v.resize(3);
    _par_multi.clear();
    _par_multi.resize(3);
    _ntrack_par_v.clear();
    _nshower_par_v.clear();
    _ntrack_par_v.resize(3);
    _nshower_par_v.resize(3);
    std::fill(_ntrack_par_v.begin(), _ntrack_par_v.end(), 0);
    std::fill(_nshower_par_v.begin(), _nshower_par_v.end(), 0);
  }
  
  void LArbysImageOut::initialize()
  {
    _vtx3d_tree = new TTree("Vertex3DTree","");
    _vtx3d_tree->Branch("run"   ,&_run       ,"run/i");
    _vtx3d_tree->Branch("subrun",&_subrun    ,"subrun/i");
    _vtx3d_tree->Branch("event" ,&_event     ,"event/i");
    _vtx3d_tree->Branch("entry" ,&_entry     ,"entry/i");
    _vtx3d_tree->Branch("id"    ,&_vtx3d_id  ,"id/i");
    _vtx3d_tree->Branch("type"  ,&_vtx3d_type,"type/i");
    _vtx3d_tree->Branch("x"     ,&_vtx3d_x   ,"x/D");
    _vtx3d_tree->Branch("y"     ,&_vtx3d_y   ,"y/D");
    _vtx3d_tree->Branch("z"     ,&_vtx3d_z   ,"z/D");
    _vtx3d_tree->Branch("vtx2d_x_v"    ,&_vtx2d_x_v);
    _vtx3d_tree->Branch("vtx2d_y_v"    ,&_vtx2d_y_v);
    _vtx3d_tree->Branch("cvtx2d_x_v"   ,&_circle_x_v);
    _vtx3d_tree->Branch("cvtx2d_y_v"   ,&_circle_y_v);
    _vtx3d_tree->Branch("cvtx2d_xs_v"  ,&_circle_xs_v);
    _vtx3d_tree->Branch("multi_v"      ,&_par_multi);
    _vtx3d_tree->Branch("ntrack_par_v" ,&_ntrack_par_v);
    _vtx3d_tree->Branch("nshower_par_v",&_nshower_par_v);
  }
  
  bool LArbysImageOut::process(IOManager& mgr)
  {
    
    if(!_LArbysImageAna) throw larbys("Invalid pointer to LArbysImageAna instance specified");

    const auto& event_id = mgr.event_id();
    _run    = event_id.run();
    _subrun = event_id.subrun();
    _event  = event_id.event();
    _entry  = mgr.current_entry();

    LARCV_DEBUG()<<"At (r,s,e,e) -- ["<<_run<<","<< _subrun<<","<<_event<<","<<_entry<<"]"<<std::endl;
    
    const auto& vertex_v = _LArbysImageAna->Verticies();

    if (vertex_v.empty()) return false;

    LARCV_DEBUG() << "... " << vertex_v.size() << " verticies" << std::endl;
    _n_vtx3d = vertex_v.size();
    
    for(size_t vtxid=0;vtxid<_n_vtx3d;++vtxid) { 
      ClearVertex();
      _vtx3d_id=vtxid;

      const auto& vtx3d = vertex_v[vtxid];

      _vtx3d_type = (uint) vtx3d.type;

      _vtx3d_x = vtx3d.x;
      _vtx3d_y = vtx3d.y;
      _vtx3d_z = vtx3d.z;
      _vtx3d_n_planes = (uint) vtx3d.num_planes;
      
      for(size_t plane=0; plane<3;  ++plane) {
    	if (_vtx3d_type < 2) {
    	  // store circle vertex information
    	  const auto& circle_vtx   = vtx3d.cvtx2d_v.at(plane);
    	  const auto& circle_vtx_c = circle_vtx.center;
    	  auto& circle_x  = _circle_x_v [plane];
    	  auto& circle_y  = _circle_y_v [plane];
    	  auto& circle_xs = _circle_xs_v[plane];
    	  circle_x = circle_vtx_c.x;
    	  circle_y = circle_vtx_c.y;
    	  circle_xs = (uint) circle_vtx.xs_v.size();
    	}
    	auto& vtx2d_x = _vtx2d_x_v[plane];
    	auto& vtx2d_y = _vtx2d_y_v[plane];
    	vtx2d_x = vtx3d.cvtx2d_v[plane].center.x;
    	vtx2d_y = vtx3d.cvtx2d_v[plane].center.y;
      } // end plane
    _vtx3d_tree->Fill();
    } //end loop over vertex

    return true;
  }

  void LArbysImageOut::finalize()
  {
    _vtx3d_tree->Write();
  }
}
#endif











