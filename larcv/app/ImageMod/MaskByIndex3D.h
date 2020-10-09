/**
 * \file MaskByIndex3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class MaskByIndex3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __MASKBYINDEX3D_H__
#define __MASKBYINDEX3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class MaskByIndex3D ... these comments are used to generate
     doxygen documentation!
  */
  class MaskByIndex3D : public ProcessBase {

  public:
    
    /// Default constructor
    MaskByIndex3D(const std::string name="MaskByIndex3D");
    
    /// Default destructor
    ~MaskByIndex3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    void configure_labels(const PSet& cfg);
    std::vector<std::string> _target_tensor3d_v;
    std::vector<std::string> _output_tensor3d_v;
    std::vector<std::string> _target_cluster3d_v;
    std::vector<std::string> _output_cluster3d_v;
    std::vector<size_t> _xmask_v;
    std::vector<size_t> _ymask_v;
    std::vector<size_t> _zmask_v;
    std::vector<float> _tensor3d_min_v;
    std::vector<float> _cluster3d_min_v;
  };

  /**
     \class larcv::MaskByIndex3DFactory
     \brief A concrete factory class for larcv::MaskByIndex3D
  */
  class MaskByIndex3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    MaskByIndex3DProcessFactory() { ProcessFactory::get().add_factory("MaskByIndex3D",this); }
    /// dtor
    ~MaskByIndex3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new MaskByIndex3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

