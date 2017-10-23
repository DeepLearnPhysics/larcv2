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
    ChStatus(ProjectionID_t id, std::vector<short>&& data)
      : _status_v(std::move(data))
      , _id(id)
    {}
#endif    
    /// Default destructor
    ~ChStatus(){}

    /// Initialize instance (nullify status + set dimension)
    void initialize(size_t nwires, short status = chstatus::kUNKNOWN);
    /// Set all channels' status to what's specified
    void reset(short status = chstatus::kUNKNOWN);
    /// Set Projection id
    void id(ProjectionID_t p) { _id = p; }
    /// Set wire status
    void status(size_t wire, short status);
    /// ProjectionID_t getter
    ProjectionID_t id() const { return _id; }
    /// Status getter
    short status(size_t wire) const;
    /// Status vector const reference getter
    const std::vector<short>& as_vector() const { return _status_v; }
    /// Status vector dumper
    std::string dump() const;

  private:
    std::vector<short> _status_v;
    larcv::ProjectionID_t _id;
  };
}

#endif
/** @} */ // end of doxygen group 

