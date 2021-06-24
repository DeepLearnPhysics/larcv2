
/**
 * \file MergeStreams.h
 *
 * \ingroup Merger
 * 
 * \brief Class def header for a class MergeStreams
 *
 * @author Temigo
 */

/** \addtogroup Merger
    @{*/
#ifndef MERGESTREAMS_H
#define MERGESTREAMS_H

// #include "ImageHolder.h"
// #include "ImageMerger.h"
#include "larcv/core/Processor/ProcessDriver.h"
#include "larcv/core/DataFormat/IOManager.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {
  /**
     \class MergeStreams
     User defined class MergeStreams ... these comments are used to generate
     doxygen documentation!
  */
  class MergeStreams : public larcv_base {
  public:
    MergeStreams(std::string name = "MergeStreams");

    ~MergeStreams();

    void initialize();

    void configure(std::string cfg_file);

    bool process();

    std::vector<larcv::Particle> merge_event_particle(std::vector<larcv::EventParticle>& event_particle_v);
    VoxelSetArray merge_event_cluster3d(std::vector<larcv::EventClusterVoxel3D>& event_cluster3d_v);
    VoxelSet merge_event_sparse3d(std::vector<larcv::EventSparseTensor3D>& event_sparse3d_v);

    void finalize();

  private:
    //std::vector<ProcessDriver> _in_driver;
    //std::vector<std::string> _in_proc_name;
    //ProcessDriver _out_driver;
    std::vector<IOManager> _in_io;
    IOManager _out_io;
    std::string _out_filename;
    size_t _current_entry;
    size_t _num_input_files;
    std::vector<int> _multiplicity;
  };
}

#endif
/** @} */ // end of doxygen group
