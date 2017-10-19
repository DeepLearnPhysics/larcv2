#ifndef LARBYSRECOHOLDER_CXX
#define LARBYSRECOHOLDER_CXX

#include "LArbysRecoHolder.h"

namespace larcv {

  void
  LArOCVSerial::Clear() {
    _vertex_v.clear();
    _particle_cluster_vvv.clear();
    _track_comp_vvv.clear();
    _match_pvvv.clear();
    _meta_vv.clear();
    _matched_v.clear();
  }
  
  void
  LArbysRecoHolder::SetMeta(const std::vector<Image2D>& adc_img_v) {
    std::vector<ImageMeta> meta_v;
    meta_v.reserve(3);
    auto& meta_vv = _larocvserial->_meta_vv;
    for(const auto& adc_img : adc_img_v) meta_v.emplace_back(adc_img.meta());
    for (size_t vtxid=0; vtxid<this->Verticies().size(); ++vtxid) 
      meta_vv.emplace_back(meta_v);
  }
  
  void
  LArbysRecoHolder::FilterMatches() {
    
    std::vector<const larocv::data::Vertex3D*> vertex_ptr_v;
    std::vector<std::vector<std::vector<const larocv::data::ParticleCluster*> > > particle_cluster_ptr_vvv;
    std::vector<std::vector<std::vector<const larocv::data::TrackClusterCompound*> > > track_comp_ptr_vvv;

    auto& match_pvvv = _larocvserial->_match_pvvv;
    
    for(size_t vertexid=0; vertexid<this->Verticies().size(); ++vertexid) {
      if (match_pvvv[vertexid].empty()) continue;
      vertex_ptr_v.emplace_back(this->Vertex(vertexid));
      particle_cluster_ptr_vvv.emplace_back(this->PlaneParticles(vertexid));
      track_comp_ptr_vvv.emplace_back(this->PlaneTracks(vertexid));
    }
    
    LARCV_DEBUG() <<"Filtered "<<this->Verticies().size()<<" to "<<vertex_ptr_v.size()<<std::endl;
    std::swap(vertex_ptr_v,            _vertex_ptr_v);
    std::swap(particle_cluster_ptr_vvv,_particle_cluster_ptr_vvv);
    std::swap(track_comp_ptr_vvv,      _track_comp_ptr_vvv);
  }

  void
  LArbysRecoHolder::SetMatches(std::vector<int>&& vtxid_match_v) {
    _matched_v = std::move(vtxid_match_v);
    return;
  }
  
  std::vector<std::vector<std::pair<size_t,size_t> > >
  LArbysRecoHolder::Match(size_t vtx_id,
			  const std::vector<cv::Mat>& adc_cvimg_v,
			  bool sort) {
    
    LARCV_DEBUG() << "@ vertex id " << vtx_id << " requested coverage " << _match_coverage << " & "
		  << _match_particles_per_plane << " particles per plane & "
		  << _match_min_number << " min number of matches " << std::endl;
    
    auto match_vv = _vtx_ana.MatchClusters(this->PlaneParticles(vtx_id), // particles per plane
					   adc_cvimg_v,                  // adc cv image
					   _match_coverage,              // required coverage
					   _match_particles_per_plane,   // requires # particles per plane
					   _match_min_number,            // min number of matches
					   _match_check_type,            // ensure particle type is same
					   _match_weight_by_size);       // weight match by particle n pixels

    LARCV_DEBUG() << "returned " << match_vv.size() << " matches" << std::endl;
    if (sort) {
      // Sort the match so that the tracks come first
      std::vector<std::vector<std::pair<size_t,size_t> > > match_temp_vv;
      
      //put the tracks first
      for( auto match_v : match_vv ) {
	auto& plane0     = match_v.front().first;
	auto& id0        = match_v.front().second;
	const auto& par0 = *(this->Particle(vtx_id,plane0,id0));
	auto partype=par0.type;
	if (partype==larocv::data::ParticleType_t::kTrack) {
	  match_temp_vv.push_back(match_v);
	}
      }
      
      // Put the showers second
      for( auto match_v : match_vv ) {
	auto& plane0     = match_v.front().first;
	auto& id0        = match_v.front().second;
	const auto& par0 = *(this->Particle(vtx_id,plane0,id0));
	auto partype=par0.type;
	if (partype==larocv::data::ParticleType_t::kShower) {
	  match_temp_vv.push_back(match_v);
	}
      }

      if(match_vv.size() != match_temp_vv.size()) 
	throw larbys("Invalid match_vv ordering");
      
      std::swap(match_vv,match_temp_vv);
    }
    
    auto& match_pvvv = _larocvserial->_match_pvvv;
    if (vtx_id >= match_pvvv.size())
      match_pvvv.resize(vtx_id+1);
    
    match_pvvv[vtx_id] = match_vv;

    return match_vv;
  }

  void
  LArbysRecoHolder::Filter() {

    std::vector<const larocv::data::Vertex3D*> vertex_ptr_v;
    std::vector<std::vector<std::vector<const larocv::data::ParticleCluster*> > > particle_cluster_ptr_vvv;
    std::vector<std::vector<std::vector<const larocv::data::TrackClusterCompound*> > > track_comp_ptr_vvv;

    for(size_t vertexid=0; vertexid<this->Verticies().size(); ++vertexid) {
      
      if (_require_two_multiplicity) { 
	auto multiplicity=_vtx_ana.RequireParticleCount(this->PlaneParticles(vertexid),2,2);
	if (!multiplicity) {
	  LARCV_DEBUG() << "Filtered id " << vertexid << " @ multiplicity" << std::endl;
	  continue;
	}
      }
      if (_require_fiducial) {
	auto fiduciality=_vtx_ana.CheckFiducial(*this->Vertex(vertexid));
	if (!fiduciality) {
	  LARCV_DEBUG() << "Filtered id " << vertexid << " @ fiduciality" << std::endl;
	  continue;
	}
      }
      
      vertex_ptr_v.emplace_back(this->Vertex(vertexid));
      particle_cluster_ptr_vvv.emplace_back(this->PlaneParticles(vertexid));
      track_comp_ptr_vvv.emplace_back(this->PlaneTracks(vertexid));
    }
    
    LARCV_DEBUG() <<"Filtered "<<this->Verticies().size()<<" to "<<vertex_ptr_v.size()<<std::endl;
    std::swap(vertex_ptr_v,            _vertex_ptr_v);
    std::swap(particle_cluster_ptr_vvv,_particle_cluster_ptr_vvv);
    std::swap(track_comp_ptr_vvv,      _track_comp_ptr_vvv);
  }

  void
  LArbysRecoHolder::Reset() {
    LARCV_DEBUG() << "Reset pointers" << std::endl;
    _vertex_ptr_v.clear();
    _particle_cluster_ptr_vvv.clear();
    _track_comp_ptr_vvv.clear();
    _matched_v.clear();
    LARCV_DEBUG() << "done." << std::endl;
  }

  void
  LArbysRecoHolder::ResetOutput() {
    LARCV_DEBUG() << "Reset output copies" << std::endl;

    _larocvserial->Clear();
    
    _run   = kINVALID_INT;
    _subrun= kINVALID_INT;
    _event = kINVALID_INT;
    _entry = kINVALID_INT;

    this->Reset();
    LARCV_DEBUG() << "done." << std::endl;
  }

  void
  LArbysRecoHolder::Write() {
    const auto& vertex_v = _larocvserial->_vertex_v;
    LARCV_DEBUG() << "Writing " << vertex_v.size() << " verticies" << std::endl;
    //if(vertex_v.empty()) return;
    _out_tree->Fill();
    LARCV_DEBUG() << "done." << std::endl;
  }
  
  void
  LArbysRecoHolder::Configure(const PSet& pset) {


    this->set_verbosity((msg::Level_t)pset.get<int>("Verbosity"));
    
    _require_two_multiplicity = false;
    _require_fiducial = false;
      
    _match_coverage            = pset.get<float>("MatchCoverage",0.5);
    _match_particles_per_plane = pset.get<float>("MatchParticlesPerPlane",2);
    _match_min_number          = pset.get<float>("MatchMinimumNumber",2);
    _match_check_type          = pset.get<bool>("MatchCheckType",true);
    _match_weight_by_size      = pset.get<bool>("MatchWeightBySize",false);
    
    _output_module_name   = pset.get<std::string>("OutputModuleName");

    _output_module_offset = pset.get<size_t>("OutputModuleOffset",kINVALID_SIZE);
    
    return;
  }

  void LArbysRecoHolder::Initialize()
  {
    _out_tree = new TTree("RecoTree","");
    _out_tree->Branch("run"   ,&_run   ,"run/i");
    _out_tree->Branch("subrun",&_subrun,"subrun/i");
    _out_tree->Branch("event" ,&_event ,"event/i");
    _out_tree->Branch("entry" ,&_entry ,"entry/i");
    _larocvserial = new LArOCVSerial();
    _out_tree->Branch("AlgoData" ,&_larocvserial);
  }

  void
  LArbysRecoHolder::ShapeData(const larocv::ImageClusterManager& mgr) {

    const larocv::data::AlgoDataManager& data_mgr   = mgr.DataManager();
    const larocv::data::AlgoDataAssManager& ass_man = data_mgr.AssManager();
    
    auto output_module_id = data_mgr.ID(_output_module_name);
    if (output_module_id==kINVALID_SIZE)  {
      if(_output_module_name.empty()) {
	LARCV_WARNING() << "Empty algo name specified, nothing to do" << std::endl;
	return;
      }
      LARCV_WARNING() << "Invalid algmodule name (" << _output_module_name <<") specified" << std::endl;
      throw larbys();
    }
    const auto vtx3d_array = (larocv::data::Vertex3DArray*) data_mgr.Data(output_module_id, 0);
    const auto& vertex3d_v = vtx3d_array->as_vector();
    
    LARCV_DEBUG() << "Observed " << vertex3d_v.size() << " verticies" << std::endl;
    for(size_t vtxid=0;vtxid<vertex3d_v.size();++vtxid) {
      const auto& vtx3d = vertex3d_v[vtxid];
      LARCV_DEBUG() << "On vertex " << vtxid
		    << " of type " << (uint) vtx3d.type
		    << " @ " << &vtx3d << std::endl;

      LARCV_DEBUG() << "Current size " << _vertex_ptr_v.size() << std::endl;
      _vertex_ptr_v.push_back(&vtx3d);
      LARCV_DEBUG() << ".. now size  " << _vertex_ptr_v.size() << std::endl;

      std::vector<std::vector<const larocv::data::ParticleCluster* > > pcluster_vv;
      std::vector<std::vector<const larocv::data::TrackClusterCompound* > > tcluster_vv;
      pcluster_vv.resize(3);
      tcluster_vv.resize(3);
      
      for(size_t plane=0;plane<3;++plane) {
	
	auto& pcluster_v=pcluster_vv[plane];
	auto& tcluster_v=tcluster_vv[plane];
	
	auto output_module_id = data_mgr.ID(_output_module_name);
	
	const auto par_array = (larocv::data::ParticleClusterArray*)
	  data_mgr.Data(output_module_id, plane+_output_module_offset);

	const auto comp_array = (larocv::data::TrackClusterCompoundArray*)
	  data_mgr.Data(output_module_id, plane+_output_module_offset+3);
	
	auto par_ass_idx_v = ass_man.GetManyAss(vtx3d,par_array->ID());
	pcluster_v.resize(par_ass_idx_v.size());
	tcluster_v.resize(par_ass_idx_v.size());
	
	for(size_t ass_id=0;ass_id<par_ass_idx_v.size();++ass_id) {
	  auto ass_idx = par_ass_idx_v[ass_id];
	  if (ass_idx==kINVALID_SIZE) throw larbys("Invalid vertex->particle association detected");
	  const auto& par = par_array->as_vector()[ass_idx];
	  pcluster_v[ass_id] = &par;
	  auto comp_ass_id = ass_man.GetOneAss(par,comp_array->ID());
	  if (comp_ass_id==kINVALID_SIZE && par.type==larocv::data::ParticleType_t::kTrack) {
	    throw larbys("Track particle with no track!");
	  }
	  else if (comp_ass_id==kINVALID_SIZE) {
	    tcluster_v[ass_id] = nullptr;
	    continue;
	  }
	  const auto& comp = comp_array->as_vector()[comp_ass_id];
	  tcluster_v[ass_id] = &comp;
	} 
	_vtx_ana.ResetPlaneInfo(mgr.InputImageMetas(larocv::ImageSetID_t::kImageSetWire).at(plane));
      }
      _particle_cluster_ptr_vvv.emplace_back(std::move(pcluster_vv));
      _track_comp_ptr_vvv.emplace_back(std::move(tcluster_vv));
    } //end this vertex
  }

  bool
  LArbysRecoHolder::WriteOut(TFile* fout) {
    if (fout && _out_tree) {
      fout->cd();
      _out_tree->Write();
      return true;
    }
    return false;
  }
  
  void
  LArbysRecoHolder::StoreEvent(size_t run, size_t subrun, size_t event, size_t entry) {

    _run    = run;
    _subrun = subrun;
    _event  = event;
    _entry  = entry;

    
    LARCV_DEBUG() << "Copying " << _vertex_ptr_v.size() << " verticies" << std::endl;
    if (_vertex_ptr_v.empty()) return;

    auto& vertex_v             = _larocvserial->_vertex_v;
    auto& particle_cluster_vvv = _larocvserial->_particle_cluster_vvv;
    auto& track_comp_vvv       = _larocvserial->_track_comp_vvv;
    auto& matched_v            = _larocvserial->_matched_v;
      
    size_t n_old   = vertex_v.size();
    size_t n_new   = _vertex_ptr_v.size();
    size_t n_total = n_old + n_new;
    LARCV_DEBUG() << "Already stored " << n_old << " vertices" << std::endl;
    LARCV_DEBUG() << "Now adding     " << n_new << " vertices" << std::endl;
    LARCV_DEBUG() << "Total          " << n_total << std::endl;
    
    vertex_v.resize(n_total);
    particle_cluster_vvv.resize(n_total);
    track_comp_vvv.resize(n_total);
    matched_v.resize(n_total);
    
    for(size_t vertexid=0; vertexid<this->Verticies().size(); ++vertexid) {

      auto offset = n_old + vertexid;

      auto& vertex              = vertex_v.at(offset);
      auto& particle_cluster_vv = particle_cluster_vvv.at(offset);
      auto& track_comp_vv       = track_comp_vvv.at(offset);

      //
      // Copy the vertex, particles, and track cluster compounds
      //
      vertex = *(this->Vertex(vertexid));

      size_t nplanes = 3;
      particle_cluster_vv.resize(nplanes);
      track_comp_vv.resize(nplanes);
      for(size_t plane=0;plane<nplanes;++plane) {
	auto& particle_cluster_v = particle_cluster_vv[plane];
	auto& track_comp_v = track_comp_vv[plane];
	auto npars = this->Particles(vertexid,plane).size();
	particle_cluster_v.resize(npars);
	track_comp_v.resize(npars);
	for(size_t particleid=0;particleid<npars;++particleid) {
	  auto& particle_cluster = particle_cluster_v[particleid];
	  auto& track_comp = track_comp_v[particleid];
	  particle_cluster = *(this->Particle(vertexid,plane,particleid));
	  if (!(this->Track(vertexid,plane,particleid))) {
	    LARCV_DEBUG() << "SKIP!" << std::endl;
	    continue;
	  }
	  track_comp = *(this->Track(vertexid,plane,particleid));
	} //particle id
      } //plane id

      //
      // Set the vertex as good match or not
      //
      auto& matched = matched_v.at(offset);
      matched = _matched_v.at(vertexid);
      
    } //vertex id
  }
}

#endif

