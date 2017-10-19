/**
 * \file GenDeadChannelMap.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class GenDeadChannelMap
 *
 * @author vgenty
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __GENDEADCHANNELMAP_H__
#define __GENDEADCHANNELMAP_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include "DataFormat/EventImage2D.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class GenDeadChannelMap ... these comments are used to generate
     doxygen documentation!
  */
  class GenDeadChannelMap : public ProcessBase {

  public:
    
    /// Default constructor
    GenDeadChannelMap(const std::string name="GenDeadChannelMap");
    
    /// Default destructor
    ~GenDeadChannelMap(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);
    
    void finalize();
    
  private:

    std::string _img_producer;
    std::vector<std::vector<float> > _img_v;
    std::vector<ImageMeta> _meta_v;
  };

  /**
     \class larcv::GenDeadChannelMapFactory
     \brief A concrete factory class for larcv::GenDeadChannelMap
  */
  class GenDeadChannelMapProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    GenDeadChannelMapProcessFactory() { ProcessFactory::get().add_factory("GenDeadChannelMap",this); }
    /// dtor
    ~GenDeadChannelMapProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new GenDeadChannelMap(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

