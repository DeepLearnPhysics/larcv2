/**
 * \file BlurCluster2D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class BlurCluster2D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __BLURTENSOR2D_H__
#define __BLURTENSOR2D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class BlurCluster2D ... these comments are used to generate
     doxygen documentation!
  */
  class BlurCluster2D : public ProcessBase {

  public:
    
    /// Default constructor
    BlurCluster2D(const std::string name="BlurCluster2D");
    
    /// Default destructor
    ~BlurCluster2D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    void configure_labels(const PSet& cfg);
    std::vector<std::string> _output_producer_v;
    std::vector<std::string> _cluster2d_producer_v;
    std::vector<size_t> _numvox_v;
    std::vector<double> _sigma_v;
    std::vector<std::vector<double> > _scale_vv;
    bool _normalize;
    float _threshold;
    float _division_threshold;
  };

  /**
     \class larcv::BlurCluster2DFactory
     \brief A concrete factory class for larcv::BlurCluster2D
  */
  class BlurCluster2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BlurCluster2DProcessFactory() { ProcessFactory::get().add_factory("BlurCluster2D",this); }
    /// dtor
    ~BlurCluster2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new BlurCluster2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

