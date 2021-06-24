#ifndef MERGESTREAMS_CXX
#define MERGESTREAMS_CXX

#include "MergeStreams.h"
#include "larcv/core/Base/LArCVBaseUtilFunc.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
#include "larcv/core/DataFormat/Voxel3D.h"
#include "larcv/core/DataFormat/Voxel.h"

namespace larcv {

  MergeStreams::MergeStreams(std::string name) : larcv_base(name), _current_entry(0) {

  }
  
  MergeStreams::~MergeStreams() {

  }

  void MergeStreams::configure(std::string cfg_file) {
    // Retrieve config
    auto main_cfg = CreatePSetFromFile(cfg_file);
    auto const& cfg = main_cfg.get_pset(name());

    _num_input_files = cfg.get<size_t>("NumInputFiles");
    _out_filename = cfg.get<std::string>("OutFilename");
    std::vector<std::string> input_files  = cfg.get<std::vector<std::string> >("InputFiles");
    _multiplicity = cfg.get<std::vector<int> >("Multiplicity");
    // TODO Check for length

    // Create one IOManager per input file
    for (size_t i = 0; i < _num_input_files; ++i) {
      IOManager io(IOManager::kREAD, "InputStream" + std::to_string(i));
      io.add_in_file(input_files[i]);
      _in_io.push_back(io);
    }    

    _out_io = IOManager(IOManager::kWRITE, "OutputStream");
    _out_io.set_out_file(_out_filename);
    std::cout << "done configure" << std::endl;

  }
  void MergeStreams::initialize() {
    //_out_driver.initialize();
    /*for(auto& driver : _in_driver) {
      driver.initialize();
      std::cout << driver.io().get_n_entries() << std::endl;
     }*/
    for (auto& io : _in_io) {
      io.initialize();
      std::cout <<"nentries" <<  io.get_n_entries() << std::endl;
      //for (auto& prod : io.product_list()) 
      //  std::cout << "producer " << prod << std::endl;
    }
    _out_io.initialize();
  }

  bool MergeStreams::process() {
    /* We may want to pick N1 events from a file and N2 events from
    another file, so putting all entries in a vector for now. */
    for (size_t i = 0; i < _num_input_files; ++i) {
      if (_current_entry < _in_io[i].get_n_entries() )
        _in_io[i].read_entry(_current_entry, false);
    }
    ++_current_entry;

    // 1. Process particles first
    // First check that all files have the same producers
    std::vector<std::string> producer_list = _in_io[0].producer_list("particle");
    /*for (auto& io : _in_io) {
      if (io.producer_list("particle") !== producer_list) {
        LARCV_CRITICAL() << "Need same producers in all files" << std::endl;
        throw larbys(); 
      }
    }*/
    // Then get the data and shift all particle ids

    for (auto& producer: producer_list) { // loop over particle_mpv, particle_mpr, etc
      std::vector<larcv::EventParticle> event_particle_v;
      for (auto& io : _in_io) { // loop over input files (? or vector of events?)
        event_particle_v.push_back(io.get_data<larcv::EventParticle>(producer));
      } 
      std::vector<larcv::Particle> out_producer = merge_event_particle(event_particle_v);
      auto& out_event_particle = _out_io.get_data<larcv::EventParticle>(producer);
      out_event_particle.emplace(std::move(out_producer));
    }

    // 2. Process cluster3d
    std::vector<std::string> producer_list_cluster3d = _in_io[0].producer_list("cluster3d");
    for (auto& producer : producer_list_cluster3d) {
      std::vector<larcv::EventClusterVoxel3D> event_cluster3d_v;
      for (auto& io : _in_io)
        event_cluster3d_v.push_back(io.get_data<larcv::EventClusterVoxel3D>(producer));
      // TODO make sure particle id and cluster index still match
      VoxelSetArray vsa_output = merge_event_cluster3d(event_cluster3d_v);
      auto& out_event_cluster3d = _out_io.get_data<larcv::EventClusterVoxel3D>(producer);
      out_event_cluster3d.meta(event_cluster3d_v[0].meta());
      out_event_cluster3d.emplace(std::move(vsa_output), event_cluster3d_v[0].meta());
  
    }

    // 3. Process sparse3d
    std::vector<std::string> producer_list_sparse3d = _in_io[0].producer_list("sparse3d");
    for (auto& producer : producer_list_sparse3d) {
      std::vector<larcv::EventSparseTensor3D> event_sparse3d_v;
      for(auto& io : _in_io)
        event_sparse3d_v.push_back(io.get_data<larcv::EventSparseTensor3D>(producer));
      VoxelSet vs_output = merge_event_sparse3d(event_sparse3d_v);
      auto& out_event_sparse3d = _out_io.get_data<larcv::EventSparseTensor3D>(producer);
      std::cout << event_sparse3d_v[0].meta().dump() << std::endl;
      out_event_sparse3d.meta(event_sparse3d_v[0].meta());
      out_event_sparse3d.emplace(std::move(vs_output), event_sparse3d_v[0].meta());
      std::cout << out_event_sparse3d.meta().dump() << std::endl;
    }

    // Set run/subrun/event id in output file
    auto ev = _in_io[0].event_id();
    _out_io.set_id(ev.run(), ev.subrun(), ev.event());
    _out_io.save_entry();

    return true;
  }

  VoxelSet MergeStreams::merge_event_sparse3d(std::vector<larcv::EventSparseTensor3D>& event_sparse3d_v) {
    VoxelSet vs_out;
    for (auto ev_sparse3d : event_sparse3d_v) {
      vs_out.emplace(std::move(ev_sparse3d), false);
    }
    return vs_out;
  }

  VoxelSetArray MergeStreams::merge_event_cluster3d(std::vector<larcv::EventClusterVoxel3D>& event_cluster3d_v) {
    VoxelSetArray out_producer;
    size_t total_num_clusters = 0;
    for (auto ev_cluster3d : event_cluster3d_v)
      total_num_clusters += ev_cluster3d.as_vector().size();

    out_producer.resize(total_num_clusters);
    larcv::InstanceID_t counter = 0;
    for (auto ev_cluster3d : event_cluster3d_v) {
      std::cout << "cluster3d " << ev_cluster3d.as_vector().size() << std::endl;
      //out_producer.insert(ev_cluster3d.as_vector());
      for (auto const& cluster : ev_cluster3d.as_vector()) {
        auto& out_cluster = out_producer.writeable_voxel_set(counter);
        out_cluster.emplace(cluster, false);
        ++counter;
      }
    }
    return out_producer;
  }

  std::vector<larcv::Particle> MergeStreams::merge_event_particle(std::vector<larcv::EventParticle>& event_particle_v) {
    larcv::InstanceID_t shift_id = 0;
    larcv::InstanceID_t shift_group_id = 0;
    larcv::InstanceID_t shift_interaction_id = 0;
    std::vector<larcv::Particle> out_producer;
    for (auto event_particle : event_particle_v) {
        auto particle_v = event_particle.as_vector();
        larcv::InstanceID_t max_id = 0;
        larcv::InstanceID_t max_group_id = 0;
        larcv::InstanceID_t max_interaction_id = 0;
        for (auto particle : particle_v) {
          if (max_id < particle.id()) max_id = particle.id();
          larcv::InstanceID_t new_id = particle.id() + shift_id;
          particle.id(new_id);
          particle.parent_id(particle.parent_id() + shift_id);

          std::vector<larcv::InstanceID_t> children;
          for (auto child : particle.children_id())
            children.push_back(child + shift_id);
          particle.children_id(children);

          if (max_group_id < particle.group_id()) max_group_id = particle.group_id();
          particle.group_id(particle.group_id() + shift_group_id);

          if (max_interaction_id < particle.interaction_id()) max_interaction_id = particle.interaction_id();
          particle.interaction_id(particle.interaction_id() + shift_interaction_id);
          out_producer.push_back(particle);
        }
        shift_id += max_id+1;
        shift_group_id += max_group_id+1;
        shift_interaction_id += max_interaction_id+1;
    }
    return out_producer;
  }

  void MergeStreams::finalize() {
    /*_out_driver.finalize();
    for (auto& driver : _in_driver) {
      driver.finalize();
    }*/
    for (auto& io : _in_io)
      io.finalize();
    _out_io.finalize();
  }
}

#endif
