/**
 * \file OperateImage.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class OperateImage
 *
 * @author vgenty
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __OPERATEIMAGE_H__
#define __OPERATEIMAGE_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include <string>

namespace larcv {

  /**
     \class ProcessBase
     User defined class OperateImage ... these comments are used to generate
     doxygen documentation!
  */
  class OperateImage : public ProcessBase {

  public:
    
    /// Default constructor
    OperateImage(const std::string name="OperateImage");
    
    /// Default destructor
    ~OperateImage(){}

    enum class Operation_t : uint { kSum, kSubtract }; //, kMultiply, kDivide };
    
    void configure(const PSet&);
    void initialize() {}
    bool process(IOManager& mgr);
    void finalize() {}

    std::string _output_producer;
    std::vector<std::string> _ref_producer_v;
    std::vector<uint> _operation_v;
    
  };

  /**
     \class larcv::OperateImageFactory
     \brief A concrete factory class for larcv::OperateImage
  */
  class OperateImageProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    OperateImageProcessFactory() { ProcessFactory::get().add_factory("OperateImage",this); }
    /// dtor
    ~OperateImageProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new OperateImage(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

