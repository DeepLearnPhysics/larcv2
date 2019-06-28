/**
 * \file MaskCluster3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class MaskCluster3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __MASKCluster3D_H__
#define __MASKCluster3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class MaskCluster3D ... these comments are used to generate
     doxygen documentation!
  */
  class MaskCluster3D : public ProcessBase {

  public:
    
    /// Default constructor
    MaskCluster3D(const std::string name="MaskCluster3D");
    
    /// Default destructor
    ~MaskCluster3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    void configure_labels(const PSet& cfg);
    std::vector<float> _value_min_v;
    std::vector<std::string> _reference_producer_v;
    std::vector<std::string> _target_producer_v;
    std::vector<std::string> _output_producer_v;
  };

  /**
     \class larcv::MaskCluster3DFactory
     \brief A concrete factory class for larcv::MaskCluster3D
  */
  class MaskCluster3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    MaskCluster3DProcessFactory() { ProcessFactory::get().add_factory("MaskCluster3D",this); }
    /// dtor
    ~MaskCluster3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new MaskCluster3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

