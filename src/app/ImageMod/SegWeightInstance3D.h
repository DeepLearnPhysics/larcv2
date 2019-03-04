/**
 * \file SegWeightInstance3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class SegWeightInstance3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __SEGWEIGHTINSTANCE3D_H__
#define __SEGWEIGHTINSTANCE3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/DataFormat/Voxel3DMeta.h"
 #include "larcv/core/DataFormat/Voxel.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class SegWeightInstance3D ... these comments are used to generate
     doxygen documentation!
  */
  class SegWeightInstance3D : public ProcessBase {

  public:
    
    /// Default constructor
    SegWeightInstance3D(const std::string name="SegWeightInstance3D");
    
    /// Default destructor
    ~SegWeightInstance3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private: 

    void find_neighbors(const Voxel3DMeta& meta, 
                        const Voxel& vox,
                        std::vector<VoxelID_t>& result) const;

    std::string _instance_producer;
    std::string _keypoint_producer;
    std::string _weight_producer;
    float _weight_max;
    float _weight_vertex;
    bool _weight_surrounding;
  };

  /**
     \class larcv::SegWeightInstance3DFactory
     \brief A concrete factory class for larcv::SegWeightInstance3D
  */
  class SegWeightInstance3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SegWeightInstance3DProcessFactory() { ProcessFactory::get().add_factory("SegWeightInstance3D",this); }
    /// dtor
    ~SegWeightInstance3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SegWeightInstance3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

