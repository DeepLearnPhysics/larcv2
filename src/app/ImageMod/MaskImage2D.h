/**
 * \file MaskImage2D.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class MaskImage2D
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __MaskImage2D_H__
#define __MaskImage2D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class MaskImage2D ... these comments are used to generate
     doxygen documentation!
  */
  class MaskImage2D : public ProcessBase {

  public:
    
    /// Default constructor
    MaskImage2D(const std::string name="MaskImage2D");
    
    /// Default destructor
    ~MaskImage2D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    float _pi_thresh_min;
    float _mask_value;
    bool  _ignore_ref_origin;
    std::string _output_producer;
    std::string _reference_image_producer;
    std::string _target_image_producer;

  };

  /**
     \class larcv::MaskImage2DFactory
     \brief A concrete factory class for larcv::MaskImage2D
  */
  class MaskImage2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    MaskImage2DProcessFactory() { ProcessFactory::get().add_factory("MaskImage2D",this); }
    /// dtor
    ~MaskImage2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new MaskImage2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

