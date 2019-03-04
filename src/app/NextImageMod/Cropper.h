/**
 * \file Cropper.h
 *
 * \ingroup NextImageMod
 * 
 * \brief Class def header for a class Cropper
 *
 * @author deltutto
 */

/** \addtogroup NextImageMod

    @{*/
#ifndef __CROPPER_H__
#define __CROPPER_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class Cropper ... these comments are used to generate
     doxygen documentation!
  */
  class Cropper : public ProcessBase {

  public:
    
    /// Default constructor
    Cropper(const std::string name="Cropper");
    
    /// Default destructor
    ~Cropper(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    int _output_n_x;
    int _output_n_y;
    int _output_n_z;
    std::string               _pmaps_producer;
    std::vector<std::string>  _producer_names_v;
    std::vector<std::string>  _product_types_v;
    std::vector<std::string>  _output_producers_v;
    int _scale_mc_x = 10.;
    int _scale_mc_y = 10.;
    int _scale_mc_z = 1.;

  };

  /**
     \class larcv::CropperFactory
     \brief A concrete factory class for larcv::Cropper
  */
  class CropperProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    CropperProcessFactory() { ProcessFactory::get().add_factory("Cropper",this); }
    /// dtor
    ~CropperProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Cropper(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

