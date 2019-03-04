/**
 * \file PyImageMaker.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class PyImageMaker
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __PYIMAGEMAKER_H__
#define __PYIMAGEMAKER_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "PyUtils.h"
#include "larcv/core/DataFormat/ImageMeta.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class PyImageMaker ... these comments are used to generate
     doxygen documentation!
  */
  class PyImageMaker : public ProcessBase {

  public:
    
    /// Default constructor
    PyImageMaker(const std::string name="PyImageMaker");
    
    /// Default destructor
    ~PyImageMaker(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    void append_ndarray(PyObject* img);

    void append_ndarray_meta(PyObject* img, const ImageMeta& meta);

    void set_id(size_t run, size_t subrun, size_t event)
    { _run = run; _subrun = subrun; _event = event; }

  private:

    std::vector<larcv::Image2D> _image_v;

    std::string _producer_name;

    size_t _run, _subrun, _event;
  };

  /**
     \class larcv::PyImageMakerFactory
     \brief A concrete factory class for larcv::PyImageMaker
  */
  class PyImageMakerProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    PyImageMakerProcessFactory() { ProcessFactory::get().add_factory("PyImageMaker",this); }
    /// dtor
    ~PyImageMakerProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new PyImageMaker(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

