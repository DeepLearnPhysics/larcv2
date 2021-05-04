/**
 * \file SegWeightVertex3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class SegWeightVertex3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __SegWeightVertex3D_H__
#define __SegWeightVertex3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/DataFormat/Voxel3DMeta.h"
#include "larcv/core/DataFormat/Voxel.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class SegWeightVertex3D ... these comments are used to generate
     doxygen documentation!
  */
  class SegWeightVertex3D : public ProcessBase {

  public:
    
    /// Default constructor
    SegWeightVertex3D(const std::string name="SegWeightVertex3D");
    
    /// Default destructor
    ~SegWeightVertex3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    void find_neighbors(const Voxel3DMeta& meta, 
                        const Voxel& vox,
                        std::vector<VoxelID_t>& result) const;

    std::string _tensor3d_producer, _particle_producer, _output_producer;
    float _prob_nonzero, _prob_neighbors;
    int _distance_neighbors;
  };

  /**
     \class larcv::VertexLabel3DFactory
     \brief A concrete factory class for larcv::VertexLabel3D
  */
  class SegWeightVertex3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SegWeightVertex3DProcessFactory() { ProcessFactory::get().add_factory("SegWeightVertex3D",this); }
    /// dtor
    ~SegWeightVertex3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SegWeightVertex3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

