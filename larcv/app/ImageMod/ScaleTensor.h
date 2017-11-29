/**
 * \file ScaleTensor.h
 *
 * \ingroup ImageAna
 * 
 * \brief Class def header for a class ScaleTensor
 *
 * @author kazuhiro
 */

/** \addtogroup ImageAna

    @{*/
#ifndef __ScaleTensor_H__
#define __ScaleTensor_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include <TTree.h>

namespace larcv {

  /**
     \class ProcessBase
     User defined class ScaleTensor ... these comments are used to generate
     doxygen documentation!
  */
  class ScaleTensor : public ProcessBase {

  public:
    
    /// Default constructor
    ScaleTensor(const std::string name="ScaleTensor");
    
    /// Default destructor
    ~ScaleTensor(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    void configure_labels(const PSet& cfg);
    std::vector<TTree*> _tree_v;
    std::vector<std::string> _tensor3d_producer_v;
    std::vector<std::string> _output3d_producer_v;
    std::vector<std::string> _tensor2d_producer_v;
    std::vector<std::string> _output2d_producer_v;
    std::vector<float>       _factor2d_v;
    std::vector<float>       _factor3d_v;
    std::vector<float>       _offset2d_v;
    std::vector<float>       _offset3d_v;

  };

  /**
     \class larcv::ScaleTensorFactory
     \brief A concrete factory class for larcv::ScaleTensor
  */
  class ScaleTensorProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ScaleTensorProcessFactory() { ProcessFactory::get().add_factory("ScaleTensor",this); }
    /// dtor
    ~ScaleTensorProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ScaleTensor(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

