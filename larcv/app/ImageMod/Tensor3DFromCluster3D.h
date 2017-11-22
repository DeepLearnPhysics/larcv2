/**
 * \file Tensor3DFromCluster3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class Tensor3DFromCluster3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __Tensor3DFromCluster3D_H__
#define __Tensor3DFromCluster3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class Tensor3DFromCluster3D ... these comments are used to generate
     doxygen documentation!
  */
  class Tensor3DFromCluster3D : public ProcessBase {

  public:
    
    /// Default constructor
    Tensor3DFromCluster3D(const std::string name="Tensor3DFromCluster3D");
    
    /// Default destructor
    ~Tensor3DFromCluster3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    void configure_labels(const PSet& cfg);

    enum class PIType_t {
      kPITypeFixedPI,
      kPITypeInputVoxel,
      kPITypeClusterIndex,
      kPITypeUndefined
    };

    std::vector<std::string> _cluster3d_producer_v;
    std::vector<std::string> _output_producer_v;
    std::vector<unsigned short> _pi_type_v;
    std::vector<float> _fixed_pi_v;
  };

  /**
     \class larcv::Tensor3DFromCluster3DFactory
     \brief A concrete factory class for larcv::Tensor3DFromCluster3D
  */
  class Tensor3DFromCluster3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    Tensor3DFromCluster3DProcessFactory() { ProcessFactory::get().add_factory("Tensor3DFromCluster3D",this); }
    /// dtor
    ~Tensor3DFromCluster3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Tensor3DFromCluster3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

