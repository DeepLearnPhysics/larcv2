/**
 * \file SegLabelFromCluster3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class SegLabelFromCluster3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __SEGLABELFROMCLUSTER3D_H__
#define __SEGLABELFROMCLUSTER3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include <set>

namespace larcv {

  /**
     \class ProcessBase
     User defined class SegLabelFromCluster3D ... these comments are used to generate
     doxygen documentation!
  */
  class SegLabelFromCluster3D : public ProcessBase {

  public:
    
    /// Default constructor
    SegLabelFromCluster3D(const std::string name="SegLabelFromCluster3D");
    
    /// Default destructor
    ~SegLabelFromCluster3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    std::string _cluster3d_producer;
    std::string _particle_producer;
    std::string _output_producer;
    size_t _undefined_label;
    std::vector<size_t> _particle_to_class;
    std::vector<size_t> _anti_particle_to_class;
    size_t _min_num_voxel;
  };

  /**
     \class larcv::SegLabelFromCluster3DFactory
     \brief A concrete factory class for larcv::SegLabelFromCluster3D
  */
  class SegLabelFromCluster3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SegLabelFromCluster3DProcessFactory() { ProcessFactory::get().add_factory("SegLabelFromCluster3D",this); }
    /// dtor
    ~SegLabelFromCluster3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SegLabelFromCluster3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

