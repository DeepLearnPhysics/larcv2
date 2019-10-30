/**
 * \file WeightKeyPoint2D.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class WeightKeyPoint2D
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __WeightKeyPoint2D_H__
#define __WeightKeyPoint2D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class WeightKeyPoint2D ... these comments are used to generate
     doxygen documentation!
  */
  class WeightKeyPoint2D : public ProcessBase {

  public:
    
    /// Default constructor
    WeightKeyPoint2D(const std::string name="WeightKeyPoint2D");
    
    /// Default destructor
    ~WeightKeyPoint2D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    float _weight_value;
    int   _weight_range;
    std::string _output_producer;
    std::string _weight_producer;
    std::string _segment_producer;

  };

  /**
     \class larcv::WeightKeyPoint2DFactory
     \brief A concrete factory class for larcv::WeightKeyPoint2D
  */
  class WeightKeyPoint2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    WeightKeyPoint2DProcessFactory() { ProcessFactory::get().add_factory("WeightKeyPoint2D",this); }
    /// dtor
    ~WeightKeyPoint2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new WeightKeyPoint2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

