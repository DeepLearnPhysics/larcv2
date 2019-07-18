#ifndef __SEGLABELFIVETypes2D_CXX__
#define __SEGLABELFIVETypes2D_CXX__

#include "SegLabelFiveTypes2D.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include <assert.h>
namespace larcv {

  static SegLabelFiveTypes2DProcessFactory __global_SegLabelFiveTypes2DProcessFactory__;

  SegLabelFiveTypes2D::SegLabelFiveTypes2D(const std::string name)
    : ProcessBase(name)
  {}

  void SegLabelFiveTypes2D::configure(const PSet& cfg)
  {
    _cluster2d_producer = cfg.get<std::string>("Cluster2DProducer");
    _sparse2d_producer  = cfg.get<std::string>("Sparse2DProducer");
    _particle_producer  = cfg.get<std::string>("ParticleProducer");
    _output_producer    = cfg.get<std::string>("OutputProducer");
    // Set the minimum number of voxels across the projections to pass this event
    _min_num_voxel      = cfg.get<size_t>("MinVoxelCount",0);
  }

  void SegLabelFiveTypes2D::initialize()
  {}

  bool SegLabelFiveTypes2D::process(IOManager& mgr)
  {
    // Get event-level data products
    auto const& event_cluster2d = mgr.get_data<larcv::EventClusterPixel2D>(_cluster2d_producer);
    auto const& event_sparse2d  = mgr.get_data<larcv::EventSparseTensor2D>(_sparse2d_producer);
    auto const& event_particle  = mgr.get_data<larcv::EventParticle>(_particle_producer);
    auto event_tensor2d = (larcv::EventSparseTensor2D*)(mgr.get_data("sparse2d",_output_producer));

    // For convenience, treat list of particles using std::vector
    auto const& particle_v  = event_particle.as_vector();


    assert(event_cluster2d.as_vector().size() == event_sparse2d.as_vector().size());

    // Loop over set of clusters, each set represent 2D projection
    //for(auto const& cluster2d_v : event_cluster2d.as_vector()) {
    for(size_t i=0; i<event_cluster2d.as_vector().size(); ++i) {
      auto const& cluster2d_v = event_cluster2d.as_vector()[i];
      auto const& sparse2d_vs = event_sparse2d.as_vector()[i].as_vector();
      // Goal: fill EventSparseTensor2D for THIS projection
      //  - Need meta
      //  - Need VoxelSet (sparse matrix data for this projection)
      auto meta = cluster2d_v.meta();
      VoxelSet vs;



      // Sanity check: length of event_particle +1 should be same as length of event_cluster2d
      if((particle_v.size()+1) != cluster2d_v.size()) {
        LARCV_CRITICAL() << "Logic error: input EventClusterPixel2D has " << cluster2d_v.size() << " clusters"
        << " but EventParticle has " << particle_v.size() << " particles!" << std::endl;
        //throw larbys();
        return false;
      }

      // Fill tensor2d (vs)
      for(size_t cindex=0; cindex<particle_v.size(); ++cindex) {
        auto const& particle = particle_v[cindex];
        auto const& cluster  = cluster2d_v.as_vector()[cindex];
        // figure out label
        auto pdg = abs(particle.pdg_code());
        auto prc = particle.creation_process();
        float class_def = -1;
        if(pdg == 2212) class_def = 0;
        else if(pdg != 22 && pdg != 11) class_def = 1;
        else if(pdg == 22) class_def = 2;
        else {
	  if( prc == "primary" || prc == "nCapture" || prc == "conv") class_def = 2;
	  else if( prc == "muIoni" || prc == "hIoni") class_def = 3;
	  else if( prc == "muMinusCaptureAtRest" || prc == "muPlusCaptureAtRest" || prc == "Decay" )
	    class_def = 4;
	  else if( prc == "muPairProd" )
	    class_def = 2;
	  else {
      	    std::cout << prc << std::endl;
      	    std::cout << particle.parent_pdg_code() << " => " << particle.pdg_code() << std::endl;
      	    std::cout << particle.energy_init() << std::endl;
      	  }
        }
        if(class_def<0) continue;
        // set the label
        for(auto const& vox : cluster.as_vector()) {
          auto const& prev_vox = vs.find(vox.id());
          if(prev_vox.id() == larcv::kINVALID_VOXELID) {
            vs.emplace(vox.id(),class_def,false);
            continue;
          }
          // prioritize lower class values
          if(prev_vox.value() <= class_def) continue;
            vs.emplace(vox.id(),class_def,false);
        }
      }

      for(auto const& vox : sparse2d_vs) {
	if(vs.find(vox.id()).id() != larcv::kINVALID_VOXELID)
	  continue;
	vs.emplace(vox.id(),5,false);
      }

      // Done
      event_tensor2d->emplace(std::move(vs),std::move(meta));
    }

    size_t min_num_voxel = 1e9;
    for(auto const& vs : event_tensor2d->as_vector())
      min_num_voxel = min_num_voxel > vs.size() ? vs.size() : min_num_voxel;


    if(_min_num_voxel > 0  && min_num_voxel < _min_num_voxel ) {
      LARCV_NORMAL() << "Skipping event " << event_tensor2d->event_key()
		     << " due to voxel count (" << min_num_voxel
		     << " < " << _min_num_voxel << ")" << std::endl;
      return false;
    }
    return true;
  }

  void SegLabelFiveTypes2D::finalize()
  {}

}
#endif
