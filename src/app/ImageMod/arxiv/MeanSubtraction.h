/**
 * \file MeanSubtraction.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class MeanSubtraction
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __MEANSUBTRACTION_H__
#define __MEANSUBTRACTION_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include "DataFormat/Image2D.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class MeanSubtraction ... these comments are used to generate
     doxygen documentation!
  */
  class MeanSubtraction : public ProcessBase {

  public:
    
    /// Default constructor
    MeanSubtraction(const std::string name="MeanSubtraction");
    
    /// Default destructor
    ~MeanSubtraction(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::vector<float> _mean_value_v;

    std::vector<larcv::Image2D> _mean_image_v;
    
    std::string _target_image_producer;

  };

  /**
     \class larcv::MeanSubtractionFactory
     \brief A concrete factory class for larcv::MeanSubtraction
  */
  class MeanSubtractionProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    MeanSubtractionProcessFactory() { ProcessFactory::get().add_factory("MeanSubtraction",this); }
    /// dtor
    ~MeanSubtractionProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new MeanSubtraction(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

