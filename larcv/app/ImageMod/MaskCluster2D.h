/**
 * \file MaskCluster2D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class MaskCluster2D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __MASKCluster2D_H__
#define __MASKCluster2D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class MaskCluster2D ... these comments are used to generate
     doxygen documentation!
  */
  class MaskCluster2D : public ProcessBase {

  public:
    
    /// Default constructor
    MaskCluster2D(const std::string name="MaskCluster2D");
    
    /// Default destructor
    ~MaskCluster2D(){}

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
     \class larcv::MaskCluster2DFactory
     \brief A concrete factory class for larcv::MaskCluster2D
  */
  class MaskCluster2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    MaskCluster2DProcessFactory() { ProcessFactory::get().add_factory("MaskCluster2D",this); }
    /// dtor
    ~MaskCluster2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new MaskCluster2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

