/**
 * \file VertexLabel3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class VertexLabel3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __VertexLabel3D_H__
#define __VertexLabel3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/DataFormat/Voxel3DMeta.h"
#include "larcv/core/DataFormat/Voxel.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class VertexLabel3D ... these comments are used to generate
     doxygen documentation!
  */
  class VertexLabel3D : public ProcessBase {

  public:
    
    /// Default constructor
    VertexLabel3D(const std::string name="VertexLabel3D");
    
    /// Default destructor
    ~VertexLabel3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    void find_neighbors(const Voxel3DMeta& meta, 
                        const Voxel& vox,
                        std::vector<VoxelID_t>& result) const;

    std::string _tensor3d_producer, _primary_producer, _output_producer;
    float _prob_nonzero, _prob_neighbors;
  };

  /**
     \class larcv::VertexLabel3DFactory
     \brief A concrete factory class for larcv::VertexLabel3D
  */
  class VertexLabel3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    VertexLabel3DProcessFactory() { ProcessFactory::get().add_factory("VertexLabel3D",this); }
    /// dtor
    ~VertexLabel3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new VertexLabel3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

