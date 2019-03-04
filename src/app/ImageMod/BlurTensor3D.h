/**
 * \file BlurTensor3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class BlurTensor3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __BLURTENSOR3D_H__
#define __BLURTENSOR3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class BlurTensor3D ... these comments are used to generate
     doxygen documentation!
  */
  class BlurTensor3D : public ProcessBase {

  public:
    
    /// Default constructor
    BlurTensor3D(const std::string name="BlurTensor3D");
    
    /// Default destructor
    ~BlurTensor3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    void configure_labels(const PSet& cfg);
    std::vector<std::string> _output_producer_v;
    std::vector<std::string> _tensor3d_producer_v;
    std::vector<size_t> _numvox_v;
    std::vector<double> _sigma_v;
    std::vector<std::vector<std::vector<double> > > _scale_vvv;
    bool _normalize;
    float _threshold;
    float _division_threshold;
  };

  /**
     \class larcv::BlurTensor3DFactory
     \brief A concrete factory class for larcv::BlurTensor3D
  */
  class BlurTensor3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BlurTensor3DProcessFactory() { ProcessFactory::get().add_factory("BlurTensor3D",this); }
    /// dtor
    ~BlurTensor3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new BlurTensor3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

