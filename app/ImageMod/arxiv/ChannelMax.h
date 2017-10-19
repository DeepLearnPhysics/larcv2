/**
 * \file ChannelMax.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ChannelMax
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __CHANNELMAX_H__
#define __CHANNELMAX_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ChannelMax ... these comments are used to generate
     doxygen documentation!
  */
  class ChannelMax : public ProcessBase {

  public:
    
    /// Default constructor
    ChannelMax(const std::string name="ChannelMax");
    
    /// Default destructor
    ~ChannelMax(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    
    size_t _nplanes;
    std::string _in_producer;
    std::string _out_producer;
    std::vector<float> _plane_weight_v;
    std::vector<bool> _relevant_chan_v;
    std::vector<float> _channel_mask_v;
  };

  /**
     \class larcv::ChannelMaxFactory
     \brief A concrete factory class for larcv::ChannelMax
  */
  class ChannelMaxProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ChannelMaxProcessFactory() { ProcessFactory::get().add_factory("ChannelMax",this); }
    /// dtor
    ~ChannelMaxProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ChannelMax(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

