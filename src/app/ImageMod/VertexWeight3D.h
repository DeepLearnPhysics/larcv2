/**
 * \file VertexWeight3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class VertexWeight3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __VERTEXWEIGHT3D_H__
#define __VERTEXWEIGHT3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/DataFormat/Voxel3DMeta.h"
#include "larcv/core/DataFormat/Voxel.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class VertexWeight3D ... these comments are used to generate
     doxygen documentation!
  */
  class VertexWeight3D : public ProcessBase {

  public:
    
    /// Default constructor
    VertexWeight3D(const std::string name="VertexWeight3D");
    
    /// Default destructor
    ~VertexWeight3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    void find_neighbors(const Voxel3DMeta& meta, 
                        const Voxel& vox,
                        std::vector<VoxelID_t>& result) const;

    std::string _tensor3d_producer, _secondary_producer, _primary_producer, _output_producer;
    float _primary_scale_factor, _secondary_scale_factor, _min_threshold;
    bool _weight_neighbors;
  };

  /**
     \class larcv::VertexWeight3DFactory
     \brief A concrete factory class for larcv::VertexWeight3D
  */
  class VertexWeight3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    VertexWeight3DProcessFactory() { ProcessFactory::get().add_factory("VertexWeight3D",this); }
    /// dtor
    ~VertexWeight3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new VertexWeight3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

