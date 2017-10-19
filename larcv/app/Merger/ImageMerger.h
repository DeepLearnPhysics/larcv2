/**
 * \file ImageMerger.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ImageMerger
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __IMAGEMERGER_H__
#define __IMAGEMERGER_H__

#include "ImageHolder.h"
#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ImageMerger ... these comments are used to generate
     doxygen documentation!
  */
  class ImageMerger : public ProcessBase {

  public:
    
    /// Default constructor
    ImageMerger(const std::string name="ImageMerger");
    
    /// Default destructor
    virtual ~ImageMerger(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    void InputImageHolder1(ImageHolder* ptr);

    void InputImageHolder2(ImageHolder* ptr);

  protected:

    ImageHolder* _in1_proc;
    ImageHolder* _in2_proc;
    float        _pmt_pedestal;
    std::string  _out_tpc_producer;
    std::string  _out_pmt_producer;
    std::string  _out_status_producer;
    std::string  _out_roi_producer;
    std::string  _out_segment_producer;

  };

  /**
     \class larcv::ImageMergerFactory
     \brief A concrete factory class for larcv::ImageMerger
  */
  class ImageMergerProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ImageMergerProcessFactory() { ProcessFactory::get().add_factory("ImageMerger",this); }
    /// dtor
    ~ImageMergerProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ImageMerger(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

