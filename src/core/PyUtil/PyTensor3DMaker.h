/**
 * \file PyTensor3DMaker.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class PyTensor3DMaker
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __PYTensor3DMAKER_H__
#define __PYTensor3DMAKER_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "PyUtils.h"
#include "larcv/core/DataFormat/Voxel.h"
#include "larcv/core/DataFormat/Voxel3DMeta.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class PyTensor3DMaker ... these comments are used to generate
     doxygen documentation!
  */
  class PyTensor3DMaker : public ProcessBase {

  public:
    
    /// Default constructor
    PyTensor3DMaker(const std::string name="PyTensor3DMaker");
    
    /// Default destructor
    ~PyTensor3DMaker(){}

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
    larcv::VoxelSet _tensor3d;
    Voxel3DMeta _meta;

  };

  /**
     \class larcv::PyTensor3DMakerFactory
     \brief A concrete factory class for larcv::PyTensor3DMaker
  */
  class PyTensor3DMakerProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    PyTensor3DMakerProcessFactory() { ProcessFactory::get().add_factory("PyTensor3DMaker",this); }
    /// dtor
    ~PyTensor3DMakerProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new PyTensor3DMaker(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 
