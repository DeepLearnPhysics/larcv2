/**
 * \file SegLabelFiveTypes2D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class SegLabelFiveTypes2D
 *
 * @author kterao
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __SEGLABELFIVETypes2D_H__
#define __SEGLABELFIVETypes2D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class SegLabelFiveTypes2D ... these comments are used to generate
     doxygen documentation!
  */
  class SegLabelFiveTypes2D : public ProcessBase {

  public:
    
    /// Default constructor
    SegLabelFiveTypes2D(const std::string name="SegLabelFiveTypes2D");
    
    /// Default destructor
    ~SegLabelFiveTypes2D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    std::string _cluster2d_producer;
    std::string _particle_producer;
    std::string _output_producer;
    std::string _sparse2d_producer;
    size_t _min_num_voxel;
  };

  /**
     \class larcv::SegLabelFiveTypes2DFactory
     \brief A concrete factory class for larcv::SegLabelFiveTypes2D
  */
  class SegLabelFiveTypes2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SegLabelFiveTypes2DProcessFactory() { ProcessFactory::get().add_factory("SegLabelFiveTypes2D",this); }
    /// dtor
    ~SegLabelFiveTypes2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SegLabelFiveTypes2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

