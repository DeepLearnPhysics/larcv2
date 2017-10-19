#ifndef __LARBYSIMAGERESULT_CXX__
#define __LARBYSIMAGERESULT_CXX__

#include "LArbysImageResult.h"

namespace larcv {

  LArbysImageResult::LArbysImageResult(const std::string name)
    : LArbysImageAnaBase(name)
  {}
    
  void LArbysImageResult::Configure(const PSet& cfg)
  {
    _combined_vertex_name = cfg.get<std::string>("CombinedVertexName");
    _combined_particle_offset = cfg.get<uint>("ParticleOffset");
  }

  void LArbysImageResult::Initialize()
  {}
  
  bool LArbysImageResult::Analyze(const ::larocv::ImageClusterManager& mgr) 
  {
    const larocv::data::AlgoDataManager& data_mgr   = mgr.DataManager();
    const larocv::data::AlgoDataAssManager& ass_man = data_mgr.AssManager();
    auto track_particle_cluster_id = data_mgr.ID(_combined_vertex_name);
    
    const auto vtx3d_array = (larocv::data::Vertex3DArray*)
      data_mgr.Data(track_particle_cluster_id, 0);
    
    const auto& vtx3d_v = vtx3d_array->as_vector();

    for(const auto& vtx3d : vtx3d_v) {

      for(uint plane_id=0; plane_id<3;  ++plane_id) {

	//get the particle cluster array
	const auto par_array = (larocv::data::ParticleClusterArray*)
	  data_mgr.Data(track_particle_cluster_id, plane_id+_combined_particle_offset);

	//get the compound array
	const auto comp_array = (larocv::data::TrackClusterCompoundArray*)
	  data_mgr.Data(track_particle_cluster_id, plane_id+_combined_particle_offset+3);

	auto par_ass_idx_v = ass_man.GetManyAss(vtx3d,par_array->ID());
	
	for(auto ass_idx : par_ass_idx_v) {

	  if (ass_idx==kINVALID_SIZE) throw larbys("Invalid vertex->particle association detected");
	  const auto& par = par_array->as_vector()[ass_idx];
	  
	  const auto par_ctor = par._ctor;

	  //Here the particle contour and do something, like cluster hits
	  /*
	    ...
	    ...
	   */
	  
	  auto comp_ass_id = ass_man.GetOneAss(par,comp_array->ID());
	  if (comp_ass_id==kINVALID_SIZE) continue;

	  const auto& comp = comp_array->as_vector()[comp_ass_id];
	  // Here we can get the end points, dQdX

	  /*
	    ...
	    ...
	   */
	    
	} // end particle ass
	
      } // end this plane

    } // end this vertex
    
    return true;
  }

  void LArbysImageResult::Finalize(TFile* fout)
  {}

}
#endif
