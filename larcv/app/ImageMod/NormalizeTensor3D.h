/**
 * \file NormalizeTensor3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class NormalizeTensor3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __NORMALIZETENSOR3D_H__
#define __NORMALIZETENSOR3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class NormalizeTensor3D ... these comments are used to generate
     doxygen documentation!
  */
  class NormalizeTensor3D : public ProcessBase {

  public:
    
    /// Default constructor
    NormalizeTensor3D(const std::string name="NormalizeTensor3D");
    
    /// Default destructor
    ~NormalizeTensor3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    enum NormType_t { kNormalizeByArea, kNormalizeByAmplitude };

  private:

    void configure_labels(const PSet& cfg);

    std::vector<std::string> _tensor3d_producer_v;
    std::vector<std::string> _output_producer_v;

    std::vector<float> _norm_v;
    std::vector<unsigned short> _norm_type_v;

  };

  /**
     \class larcv::NormalizeTensor3DFactory
     \brief A concrete factory class for larcv::NormalizeTensor3D
  */
  class NormalizeTensor3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    NormalizeTensor3DProcessFactory() { ProcessFactory::get().add_factory("NormalizeTensor3D",this); }
    /// dtor
    ~NormalizeTensor3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new NormalizeTensor3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

