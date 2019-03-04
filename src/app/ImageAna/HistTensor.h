/**
 * \file HistTensor.h
 *
 * \ingroup ImageAna
 * 
 * \brief Class def header for a class HistTensor
 *
 * @author kazuhiro
 */

/** \addtogroup ImageAna

    @{*/
#ifndef __HistTensor_H__
#define __HistTensor_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include <TTree.h>

namespace larcv {

  /**
     \class ProcessBase
     User defined class HistTensor ... these comments are used to generate
     doxygen documentation!
  */
  class HistTensor : public ProcessBase {

  public:
    
    /// Default constructor
    HistTensor(const std::string name="HistTensor");
    
    /// Default destructor
    ~HistTensor(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    std::vector<TTree*> _tree_v;
    std::vector<std::string> _tensor3d_producer_v;
    std::vector<std::string> _tensor2d_producer_v;
    float _x,_y,_z,_v;
    int _projection;
  };

  /**
     \class larcv::HistTensorFactory
     \brief A concrete factory class for larcv::HistTensor
  */
  class HistTensorProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    HistTensorProcessFactory() { ProcessFactory::get().add_factory("HistTensor",this); }
    /// dtor
    ~HistTensorProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new HistTensor(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

