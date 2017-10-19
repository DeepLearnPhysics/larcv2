/**
 * \file ChStatus.h
 *
 * \ingroup core_DataFormat
 * 
 * \brief Class def header for a class larcv::ChStatus
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef __LARCV_CHSTATUS_H__
#define __LARCV_CHSTATUS_H__

#include <iostream>
#include "DataFormatTypes.h"
namespace larcv {
  /**
     \class ChStatus
     \brief Data rep. to hold channel-wise "status" (such as dead wire, etc.).
  */
  class ChStatus {
    
  public:
    
    /// Default constructor
    ChStatus(){}

#ifndef __CINT__
    /// Status vector move ctor
    ChStatus(PlaneID_t plane, std::vector<short>&& data)
      : _status_v(std::move(data))
      , _plane(plane)
    {}
#endif    
    /// Default destructor
    ~ChStatus(){}

    /// Initialize instance (nullify status + set dimension)
    void  Initialize(size_t nwires, short status = chstatus::kUNKNOWN);
    /// Set all channels' status to what's specified
    void  Reset(short status = chstatus::kUNKNOWN);
    /// Set plane id
    void Plane(PlaneID_t p) { _plane = p; }
    /// Set wire status
    void Status(size_t wire, short status);
    /// Plane getter
    PlaneID_t Plane() const { return _plane; }
    /// Status getter
    short Status(size_t wire) const;
    /// Status vector const reference getter
    const std::vector<short>& as_vector() const { return _status_v; }
    /// Status vector dumper
    std::string dump() const;

  private:
    std::vector<short> _status_v;
    larcv::PlaneID_t _plane;
  };
}

#endif
/** @} */ // end of doxygen group 

