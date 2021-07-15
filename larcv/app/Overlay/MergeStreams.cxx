#ifndef MERGESTREAMS_CXX
#define MERGESTREAMS_CXX

#include "MergeStreams.h"
#include "larcv/core/Base/LArCVBaseUtilFunc.h"
#include "larcv/core/DataFormat/EventNeutrino.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
#include "larcv/core/DataFormat/Voxel3D.h"
#include "larcv/core/DataFormat/Voxel.h"
#include "larcv/core/DataFormat/Neutrino.h"

namespace larcv {

  MergeStreams::MergeStreams(std::string name) : larcv_base(name) {

  }
  
  MergeStreams::~MergeStreams() {

  }

  void MergeStreams::configure(std::string cfg_file) {
    // Retrieve config
    auto main_cfg = CreatePSetFromFile(cfg_file);
    auto const& cfg = main_cfg.get_pset(name());

    //_num_input_files = cfg.get<size_t>("NumInputFiles");
    _out_filename = cfg.get<std::string>("OutFilename");
    std::vector<std::string> input_files  = cfg.get<std::vector<std::string> >("InputFiles");
    std::vector<int> multiplicity = cfg.get<std::vector<int> >("Multiplicity");

    _cluster3d_aligned = cfg.get<std::vector<std::string> >("Cluster3dAligned");
    _particle_aligned = cfg.get<std::vector<std::string> >("ParticleAligned");

    _num_input_files = input_files.size();
    if (_num_input_files != multiplicity.size()) {
      LARCV_CRITICAL() << "Input files list and multiplicity list must have the same size." << std::endl;
      throw larbys();
    }

    // Create one IOManager per input file
    for (size_t i = 0; i < _num_input_files; ++i) {
      for (size_t j = 0; j < multiplicity[i]; ++j) {
        IOManager io(IOManager::kREAD, "InputStream" + std::to_string(i));
        io.add_in_file(input_files[i]);
        _in_io.push_back(io);
        _current_entry.push_back(0);
        _multiplicity.push_back(multiplicity[i]);
        _offset.push_back(j);
      }
    }    

    _out_io = IOManager(IOManager::kWRITE, "OutputStream");
    _out_io.set_out_file(_out_filename);
    //std::cout << "done configure" << std::endl;

  }
  void MergeStreams::initialize() {
    //_out_driver.initialize();
    /*for(auto& driver : _in_driver) {
      driver.initialize();
      std::cout << driver.io().get_n_entries() << std::endl;
     }*/
    for (auto& io : _in_io) {
      io.initialize();
      //std::cout <<"nentries" <<  io.get_n_entries() << std::endl;
      //for (auto& prod : io.product_list()) 
      //  std::cout << "producer " << prod << std::endl;
    }
    _out_io.initialize();
  }

  bool MergeStreams::process() {
    /*
	So we want to pick N1 from file 1 and N2 from file 2.
	We pick N1 first events of file 1, overlay with N2 
	first events of file 2. That makes 1 output event.
	Then we pick the next N1 events of file 1 with the next
	N2 events of file 2, etc. In other words: sequential
	batches in each file. 

	TODO : add randomness option?
	
	Since we have multiple entries in _in_io for the 
	same file (depending on configured multiplicity),
	we need to adapt the entry index that we are reading
	for each of these instances.

    */
    for (size_t i = 0; i < _in_io.size(); ++i) {
      //for (size_t j = 0; j < _multiplicity[i]; ++j) {
        if (_current_entry[i] + _offset[i] < _in_io[i].get_n_entries() ) {
          _in_io[i].read_entry(_current_entry[i] + _offset[i], false);
        }
      //}
      _current_entry[i] = _current_entry[i] + _multiplicity[i];
    }

    // 2. Process cluster3d
    std::vector<std::string> producer_list_cluster3d = _in_io[0].producer_list("cluster3d");
    std::map<std::string, larcv::EventClusterVoxel3D> store_out_event_cluster3d;
    //std::map<std::string, std::vector<int> > offset_cluster3d;
    for (auto& producer : producer_list_cluster3d) {
      std::vector<larcv::EventClusterVoxel3D> event_cluster3d_v;
      //std::vector<int> offsets;
      for (auto& io : _in_io) {
        auto& event_cluster3d = io.get_data<larcv::EventClusterVoxel3D>(producer);
        event_cluster3d_v.push_back(event_cluster3d);
        //offsets.push_back(event_cluster3d.as_vector().size());
      }
      //offset_cluster3d.push_back(offsets);
      // TODO make sure particle id and cluster index still match
      VoxelSetArray vsa_output = merge_event_cluster3d(event_cluster3d_v);
      auto& out_event_cluster3d = _out_io.get_data<larcv::EventClusterVoxel3D>(producer);
      out_event_cluster3d.meta(event_cluster3d_v[0].meta());
      out_event_cluster3d.emplace(std::move(vsa_output), event_cluster3d_v[0].meta());
      store_out_event_cluster3d.insert(std::pair<std::string, larcv::EventClusterVoxel3D>(producer, out_event_cluster3d));
    }

    // 1. Process particles first
    // TODO First check that all files have the same producers
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

      // Check voxel count for aligned data products
      auto idx = std::find(_particle_aligned.begin(), _particle_aligned.end(), producer); 
      if (idx != _particle_aligned.end()) {
        std::string cluster3d_producer = _cluster3d_aligned[idx - _particle_aligned.begin()];
        auto cluster3d_idx = std::find(producer_list_cluster3d.begin(), producer_list_cluster3d.end(), cluster3d_producer);
        if (cluster3d_idx == producer_list_cluster3d.end()) {
          LARCV_CRITICAL() << "No cluster3d product found with name " << cluster3d_producer << std::endl;
          throw larbys();
        }
        else {
          // Time to check
          //auto& out_event_cluster3d = _out_io.get_data<larcv::EventClusterVoxel3D>(producer);
          // Same length for cluster3d list and particles list
          assert(store_out_event_cluster3d[cluster3d_producer].as_vector().size() == out_event_particle.as_vector().size());
          for (size_t pidx = 0; pidx <  out_event_particle.as_vector().size(); ++ pidx) {
            //std::cout << store_out_event_cluster3d[cluster3d_producer].as_vector().size() << " " << pidx << std::endl;
            assert(store_out_event_cluster3d[cluster3d_producer].as_vector().size() > pidx);
            //std::cout << producer << " " << store_out_event_cluster3d[cluster3d_producer].as_vector()[pidx].as_vector().size() << " " << out_event_particle.as_vector()[pidx].num_voxels() << " " << store_out_event_cluster3d[cluster3d_producer].as_vector()[pidx].sum() << " " << out_event_particle.as_vector()[pidx].energy_deposit() << std::endl;
            // Same voxel count?
            //assert(store_out_event_cluster3d[cluster3d_producer].as_vector()[pidx].as_vector().size() == out_event_particle.as_vector()[pidx].num_voxels());
          }
        }
      }
    }

    // 3. Process sparse3d
    std::vector<std::string> producer_list_sparse3d = _in_io[0].producer_list("sparse3d");
    for (auto& producer : producer_list_sparse3d) {
      std::vector<larcv::EventSparseTensor3D> event_sparse3d_v;
      for(auto& io : _in_io)
        event_sparse3d_v.push_back(io.get_data<larcv::EventSparseTensor3D>(producer));
      larcv::VoxelSet vs_output = merge_event_sparse3d(event_sparse3d_v);
      auto& out_event_sparse3d = _out_io.get_data<larcv::EventSparseTensor3D>(producer);
      out_event_sparse3d.meta(event_sparse3d_v[0].meta());
      out_event_sparse3d.emplace(std::move(vs_output), event_sparse3d_v[0].meta());
    }

    // 4. Process neutrino
    std::vector<std::string> producer_list_neutrino = _in_io[0].producer_list("neutrino");
    for (auto& producer : producer_list_neutrino) {
      std::vector<larcv::EventNeutrino> event_neutrino_v;
      std::vector<larcv::EventParticle> event_particle_v;

      // Check that a ParticleSet exists for that producer
      auto idx = std::find(producer_list.begin(), producer_list.end(), producer);
      if (idx == producer_list.end()) {
        LARCV_CRITICAL() << "Found neutrino product without corresponding particle product (named " << producer << ")" << std::endl;
        throw larbys();
      }

      for (auto& io : _in_io) {
        auto& event_neutrino = io.get_data<larcv::EventNeutrino>(producer);
        event_neutrino_v.push_back(event_neutrino);
        auto& event_particle = io.get_data<larcv::EventParticle>(producer);
        event_particle_v.push_back(event_particle);
      }

      std::vector<larcv::Neutrino> output = merge_event_neutrino(event_neutrino_v, event_particle_v);
      auto& out_event_neutrino = _out_io.get_data<larcv::EventNeutrino>(producer);
      //out_event_cluster3d.meta(event_cluster3d_v[0].meta());
      out_event_neutrino.emplace(std::move(output));
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

  std::vector<larcv::Neutrino> MergeStreams::merge_event_neutrino(std::vector<larcv::EventNeutrino>& event_neutrino_v, std::vector<larcv::EventParticle>& event_particle_v) {
    assert(event_neutrino_v.size() == event_particle_v.size());

    std::vector<larcv::Neutrino> out_producer;
    larcv::InstanceID_t shift_id = 0;
    for (size_t i = 0; i < event_neutrino_v.size(); ++i) {
      auto neutrino_v = event_neutrino_v[i].as_vector();
      auto particle_v = event_particle_v[i].as_vector();
      larcv::InstanceID_t max_id = 0;
      for (auto particle : particle_v)
        if (max_id < particle.id()) max_id = particle.id();
      for (auto neutrino: neutrino_v) {
        neutrino.id(neutrino.id() + shift_id);
        out_producer.push_back(neutrino);
      }
      shift_id += max_id+1;
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
