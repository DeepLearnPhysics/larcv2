/**
 * \file PyImageStitcher.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class PyImageStitcher
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __PYIMAGESTITCHER_H__
#define __PYIMAGESTITCHER_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "PyUtils.h"
#include "larcv/core/DataFormat/ImageMeta.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class PyImageStitcher ... these comments are used to generate
     doxygen documentation!
  */
  class PyImageStitcher : public ProcessBase {

  public:
    
    /// Default constructor
    PyImageStitcher(const std::string name="PyImageStitcher");
    
    /// Default destructor
    ~PyImageStitcher(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();
    
    void append_ndarray(PyObject* img, size_t ch);

    void append_ndarray_meta(PyObject* img, const ImageMeta& meta, size_t ch);

    void set_id(size_t run, size_t subrun, size_t event)
    { _run = run; _subrun = subrun; _event = event; }

    void set_producer_name(std::string name)
    { _producer_name = name; }

    const std::string& producer_name() const { return _producer_name; }

  private:

    std::vector< std::vector<larcv::Image2D> > _image_vv;

    std::string _producer_name;

    size_t _run, _subrun, _event;
  };

  /**
     \class larcv::PyImageStitcherFactory
     \brief A concrete factory class for larcv::PyImageStitcher
  */
  class PyImageStitcherProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    PyImageStitcherProcessFactory() { ProcessFactory::get().add_factory("PyImageStitcher",this); }
    /// dtor
    ~PyImageStitcherProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new PyImageStitcher(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

