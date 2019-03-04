/**
 * \file PyCluster3DMaker.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class PyCluster3DMaker
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __PYCluster3DMAKER_H__
#define __PYCluster3DMAKER_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "PyUtils.h"
#include "larcv/core/DataFormat/Voxel.h"
#include "larcv/core/DataFormat/Voxel3DMeta.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class PyCluster3DMaker ... these comments are used to generate
     doxygen documentation!
  */
  class PyCluster3DMaker : public ProcessBase {

  public:
    
    /// Default constructor
    PyCluster3DMaker(const std::string name="PyCluster3DMaker");
    
    /// Default destructor
    ~PyCluster3DMaker(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    void append_ndarray(PyObject* img);

    inline void set_meta(const Voxel3DMeta& meta) { _meta = meta; }

    void set_id(size_t run, size_t subrun, size_t event)
    { _run = run; _subrun = subrun; _event = event; }

  private:

    std::string _producer_name;
    size_t _run, _subrun, _event;
    larcv::VoxelSetArray _cluster3d_v;
    Voxel3DMeta _meta;

  };

  /**
     \class larcv::PyCluster3DMakerFactory
     \brief A concrete factory class for larcv::PyCluster3DMaker
  */
  class PyCluster3DMakerProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    PyCluster3DMakerProcessFactory() { ProcessFactory::get().add_factory("PyCluster3DMaker",this); }
    /// dtor
    ~PyCluster3DMakerProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new PyCluster3DMaker(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 
