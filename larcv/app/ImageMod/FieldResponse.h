/**
 * \file FieldResponse.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class FieldResponse
 *
 * @author drinkingkazu
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __FIELDRESPONSE_H__
#define __FIELDRESPONSE_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class FieldResponse ... these comments are used to generate
     doxygen documentation!
  */
  class FieldResponse : public ProcessBase {

  public:

    /// Default constructor
    FieldResponse(const std::string name="FieldResponse");

    /// Default destructor
    ~FieldResponse(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    std::string _tensor3d_producer;
    float _voxel_value;
  };

  /**
     \class larcv::FieldResponseFactory
     \brief A concrete factory class for larcv::FieldResponse
  */
  class FieldResponseProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    FieldResponseProcessFactory() { ProcessFactory::get().add_factory("FieldResponse",this); }
    /// dtor
    ~FieldResponseProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new FieldResponse(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group
