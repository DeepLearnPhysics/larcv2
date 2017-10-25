/**
 * \file WireMask.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class WireMask
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __WIREMASK_H__
#define __WIREMASK_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class WireMask
     This module is used to mask column pixels that correspond to a set of specified wires.\n
  */
  class WireMask : public ProcessBase {

  public:
    
    /// Default constructor
    WireMask(const std::string name="WireMask");
    
    /// Default destructor
    ~WireMask(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _image_producer;  ///< Image to mask
    int         _plane_id;        ///< Plane ID (i.e. EventImage2D index number) to mask wires for. <0 means ALL planes
    std::vector<size_t> _wire_v;  ///< A list of wire numbers to be masked
    float       _mask_val;        ///< Value to be used for masking (default 0)
    std::string _chstatus_producer; ///< ChStatus producer name (if using ChStatus to mask)
    chstatus::ChannelStatus_t _threshold;     ///< Threshold status for ChStatus
  };

  /**
     \class larcv::WireMaskFactory
     \brief A concrete factory class for larcv::WireMask
  */
  class WireMaskProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    WireMaskProcessFactory() { ProcessFactory::get().add_factory("WireMask",this); }
    /// dtor
    ~WireMaskProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new WireMask(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

