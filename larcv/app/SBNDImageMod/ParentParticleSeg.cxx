#ifndef __PARENTPARTICLESEG_CXX__
#define __PARENTPARTICLESEG_CXX__

#include "ParentParticleSeg.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

static ParentParticleSegProcessFactory
    __global_ParentParticleSegProcessFactory__;

ParentParticleSeg::ParentParticleSeg(const std::string name)
    : ProcessBase(name) {}

void ParentParticleSeg::configure(const PSet& cfg) {
  _cluster2d_producer = cfg.get<std::string>("Cluster2dProducer", "");
  _cluster3d_producer = cfg.get<std::string>("Cluster3dProducer", "");
  _output_producer = cfg.get<std::string>("OutputProducer");
  _particle_producer = cfg.get<std::string>("ParticleProducer");

  if (_cluster2d_producer == "" && _cluster3d_producer == ""){
        LARCV_CRITICAL() << "Must specify at least one cluster producer!" << std::endl;
        throw larbys();
  }

}

void ParentParticleSeg::initialize() {}

bool ParentParticleSeg::process(IOManager& mgr) {


  // std::cout << "Enter ParentParticleSeg::process " << std::endl;
  // Read in the particles that define the pdg types:
  auto const& ev_particle =
      mgr.get_data<larcv::EventParticle>(_particle_producer);

  // Also output a corresponding particle 2d to match the clusters:
  auto& ev_particle_output =
      mgr.get_data<larcv::EventParticle>(_output_producer);

  ev_particle_output.clear();

  bool debug = false;

  if (debug){
    std::cout << "Initial number of particles: " << ev_particle.as_vector().size() << std::endl;
    std::cout << "Initial number of output particles: " << ev_particle_output.as_vector().size() << std::endl;
  }

  // Loop over all of the input particles and determine their ancestor


  // Make a particle node for every particle (to build a particle tree)
  std::vector<particle_node*> particle_nodes;

  // Additionally, keep a short list of just the primaries (pointers)
  std::vector<particle_node*> primary_nodes;

  particle_node * orphanage = new particle_node();


  std::set<int> _ancestor_trackids;
  std::set<int> _found_ancestor_nodes;

  for (auto& particle : ev_particle.as_vector()) {
    if (debug){
      std::cout << "Ancestor track ID, ancestor PDG, id, parent ID, PDG, track id: ("
                << particle.ancestor_track_id() << ", "
                << particle.ancestor_pdg_code() << ", "
                << particle.id() << ", "
                << particle.parent_track_id() << ", "
                << particle.pdg_code() << ", "
                << particle.track_id() << ")" << std::endl;
    }

    // Particles are top level if their ancestor ID == their own track ID

    // Create a particle_node object:
    particle_node * node = new particle_node();
    particle_nodes.push_back(node);
    node->reference = &particle;
    node->trackID = particle.track_id();
    node->parentID = particle.parent_track_id();
    node->ancestorID = particle.ancestor_track_id();

    // Primary?
    if (particle.ancestor_track_id() == particle.track_id() ||
        particle.ancestor_pdg_code() == 111 ||  // neutral particles make new primaries
        particle.ancestor_pdg_code() == 2112
      ){
      // Yes, it's a primary
      // std::cout << "primary " << particle.track_id() << std::endl;
      // std::cout << "Address: " << node << std::endl;
      node->primary = true;
      node->parent = NULL;
      primary_nodes.push_back(node);
      _found_ancestor_nodes.insert(particle.ancestor_track_id());
    }
    else{
      // No, not a primary
      node->primary = false;
    }
    //
    // Keep track of all ancestor ids found:
    _ancestor_trackids.insert(particle.ancestor_track_id());

  }

  // // Fill out the rest of the ancestor nodes:
  for (auto& trackID : _ancestor_trackids) {
    if (_found_ancestor_nodes.find(trackID) == _found_ancestor_nodes.end()) {
      // This ancestor id isn't a tangible particle
      particle_node * this_node = new particle_node;
      particle_nodes.push_back(this_node);
      primary_nodes.push_back(this_node);
      this_node->reference = NULL;
      this_node->parent = NULL;
      this_node->trackID = trackID;
      this_node->primary = true;
    }
    // std::cout << "Ancestor ID " << trackID << std::endl;
  }

  if (debug){
    for (auto node : primary_nodes){
      std::cout << "Found the following primary: " << node->trackID << std::endl;
    }
  }

  if (debug){
    std::cout << "Created " << particle_nodes.size() << " total nodes." << std::endl;
  }

  // Now, every particle has a corresponding particle node,
  // And there are particle nodes made for the trackIDs that aren't actually
  // particles (Didn't ionize, probably)

  // Loop through the list of particle nodes, and find the parent node
  // of each particle.  To speed up a little, since most particles
  // are only one layer deep, check the ancestor nodes first
  for (auto node : particle_nodes){
    if (node->primary) continue;

    bool found = false;

    // This code sorts into a full tree:
    for (auto primary_node : primary_nodes){
      if (node->parentID == primary_node -> trackID){
        primary_node -> daughters.push_back(node);
        // std::cout << "Found node " << primary_node->trackID << " as parent of " << node->trackID << std::endl;
        found = true;
        break;
      }
    }
    if (!found){
      for (auto & node2 : particle_nodes){
        if (node->parentID == node2->trackID){
          node2->daughters.push_back(node);
          found = true;
          break;
        }
      }
    }
    if (! found){
      // Try to match this particle's ancestor with a primary node
      found = false;
      for (auto primary_node : primary_nodes){
        if (node->ancestorID == primary_node -> trackID){
          primary_node -> daughters.push_back(node);
          // std::cout << "Found node " << primary_node->trackID << " as parent of " << node->trackID << std::endl;
          found = true;
          break;
        }
      }
      // std::cout << "node with track ID " << node->trackID << " is orphaned." << std::endl;
      if (!found)
        orphanage -> daughters.push_back(node);
    }

  }


  if (debug){
    for (auto ancestor_node : primary_nodes){
      std::cout << "Top level particle.  TrackID is "
                << ancestor_node->trackID
                << ", number of daughers: "
                << ancestor_node->daughters.size()
                << ", reference: " << ancestor_node->reference
                << std::endl;
      for (auto daughter : ancestor_node->daughters){
        std::cout << "--> daughter trackID " << daughter->trackID << std::endl;
        // std::cout << "--> daughter trackID " << daughter->trackID << ", id " << daughter->reference->id() << std::endl;
      }
    }
    // Print out the orphaned particles too:
    std::cout << "Orphanage: " <<std::endl;
    for (auto & particle : orphanage->daughters){
        std::cout << "--> orphan trackID " << particle->trackID << std::endl;
    }
  }
  // Here, every particle is sorted into it's own group by ancestor.

  // Make the appropriate list of new particles:
  for (auto node : primary_nodes){
    if (node -> reference != NULL){
      ev_particle_output.append(*(node->reference));
    }
    else{
      ev_particle_output.append(Particle());
    }
  }

  // We now loop over the clusters indicated and merge them together based on

  if (_cluster2d_producer != ""){
    // Read in the original source of segmentation, the cluster indexes:
    auto const& ev_cluster2d =
        mgr.get_data<larcv::EventClusterPixel2D>(_cluster2d_producer);


    // The output is an instance of cluster2d, so prepare that:
    auto& ev_cluster2d_output =
        mgr.get_data<larcv::EventClusterPixel2D>(_output_producer);


    for (size_t projection_index = 0;
         projection_index < ev_cluster2d.as_vector().size(); ++projection_index) {
      // For each projection index, get the list of clusters
      auto const& clusters = ev_cluster2d.cluster_pixel_2d(projection_index);

      larcv::ClusterPixel2D new_clusters;
      new_clusters.meta(clusters.meta());

      int i = 0;
      for (auto ancestor_node : primary_nodes) {
        auto out_cluster = cluster_merger(clusters, ancestor_node);
        out_cluster.id(i);
        i++;
        new_clusters.emplace(std::move(out_cluster));
      }
      // Handle the orphanage, as well:
      orphanage->trackID = i;
      auto out_cluster = cluster_merger(clusters, orphanage);
      new_clusters.emplace(std::move(out_cluster));
      // std::cout << "Number of primary_nodes: " << primary_nodes.size()
      //           << std::endl;
      // std::cout << "Number of new clusters: " << new_clusters.size() << std::endl;
      // Append the output image2d:
      ev_cluster2d_output.emplace(std::move(new_clusters));
    }
  }
  if (_cluster3d_producer != ""){

    // Read in the original source of segmentation, the cluster indexes:
    auto const& ev_cluster3d =
        mgr.get_data<larcv::EventClusterVoxel3D>(_cluster3d_producer);

    // The output is an instance of cluster3d, so prepare that:
    auto& ev_cluster3d_output =
        mgr.get_data<larcv::EventClusterVoxel3D>(_output_producer);


    // For each projection index, get the list of clusters
    larcv::ClusterVoxel3D new_clusters;
    new_clusters.meta(ev_cluster3d.meta());

    int i = 0;
    for (auto ancestor_node : primary_nodes) {
      auto out_cluster = cluster_merger(ev_cluster3d, ancestor_node);
      out_cluster.id(i);
      i++;
      new_clusters.insert(out_cluster);
    }
    // Handle the orphanage, as well:
    orphanage->trackID = i;
    auto out_cluster = cluster_merger(ev_cluster3d, orphanage);
    new_clusters.insert(out_cluster);
    // std::cout << "Number of primary_nodes: " << primary_nodes.size()
    //           << std::endl;
    // std::cout << "Number of new clusters: " << new_clusters.size() << std::endl;
    // Append the output image2d:
    ev_cluster3d_output.emplace(std::move(new_clusters), ev_cluster3d.meta());

  }



  // Final numbers:
  // std::cout << "Final number of particles: " << ev_particle_output.as_vector().size() << std::endl;
  // std::cout << "Final number of clusters: " << ev_cluster2d_output.cluster_pixel_2d(0).as_vector().size() << std::endl;

  // Clean up:
  for (auto node : particle_nodes){
    delete node;
  }
  delete orphanage;

  // std::cout << "Exit ParentParticleSeg::process " << std::endl;


  return true;
}

void ParentParticleSeg::get_all_daughter_ids(std::vector<int> & ids, const particle_node * node){
  if (node -> reference != NULL){
    ids.push_back(node->reference->id());
  }
  for (auto const daughter : node -> daughters){
    get_all_daughter_ids(ids, daughter);
  }
  return;
}

larcv::VoxelSet ParentParticleSeg::cluster_merger(
    const larcv::ClusterPixel2D& clusters, particle_node * primary_node) {
  // Create an empty voxelset to hold the output:
  larcv::VoxelSet output_set;
  output_set.id(primary_node->trackID);

  // if (primary_node -> reference == NULL){
  //   return output_set;
  // }

  std::vector<int> cluster_indexes;

  get_all_daughter_ids(cluster_indexes, primary_node);

  // std::cout << "For particle with track ID " << primary_node->trackID << ", "
  //           << "the following cluster indexes are to be merged: " << std::endl;
  // for (auto & index : cluster_indexes) std::cout << " - " << index << std::endl;


  for (auto id : cluster_indexes) {
    auto& input_cluster = clusters.voxel_set(id);
    for (auto& voxel : input_cluster.as_vector()) {
      output_set.add(larcv::Voxel(voxel.id(), voxel.value()));
    }
  }


  return output_set;
}

larcv::VoxelSet ParentParticleSeg::cluster_merger(
    const larcv::ClusterVoxel3D & clusters, particle_node * primary_node) {
  // Create an empty voxelset to hold the output:
  larcv::VoxelSet output_set;
  output_set.id(primary_node->trackID);

  if (primary_node -> reference == NULL){
    return output_set;
  }

  std::vector<int> cluster_indexes;

  get_all_daughter_ids(cluster_indexes, primary_node);


  for (auto id : cluster_indexes) {
    auto& input_cluster = clusters.voxel_set(id);
    // std::cout << "Cluster index " << id << ", number of voxels: " << input_cluster.as_vector().size() << std::endl;
    for (auto& voxel : input_cluster.as_vector()) {
      if (voxel.id() >= clusters.meta().size()){
        continue;
      }
      output_set.add(larcv::Voxel(voxel.id(), voxel.value()));
    }
  }


  return output_set;
}


void ParentParticleSeg::finalize() {}
}
#endif
