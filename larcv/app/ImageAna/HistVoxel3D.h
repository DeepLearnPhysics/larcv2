/**
 * \file HistVoxel3D.h
 *
 * \ingroup ImageAna
 * 
 * \brief Class def header for a class HistVoxel3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageAna

    @{*/
#ifndef __HISTVOXEL3D_H__
#define __HISTVOXEL3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include <TTree.h>

namespace larcv {

  /**
     \class ProcessBase
     User defined class HistVoxel3D ... these comments are used to generate
     doxygen documentation!
  */
  class HistVoxel3D : public ProcessBase {

  public:
    
    /// Default constructor
    HistVoxel3D(const std::string name="HistVoxel3D");
    
    /// Default destructor
    ~HistVoxel3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    TTree* _tree;
    std::string _tensor3d_producer;
    float _x,_y,_z,_v;
  };

  /**
     \class larcv::HistVoxel3DFactory
     \brief A concrete factory class for larcv::HistVoxel3D
  */
  class HistVoxel3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    HistVoxel3DProcessFactory() { ProcessFactory::get().add_factory("HistVoxel3D",this); }
    /// dtor
    ~HistVoxel3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new HistVoxel3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

