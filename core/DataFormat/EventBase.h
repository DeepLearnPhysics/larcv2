/**
 * \file EventBase.h
 *
 * \ingroup core_DataFormat
 * 
 * \brief Class def header for a class larcv::EventBase
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef LARCV_EVENTBASE_H
#define LARCV_EVENTBASE_H

#include <iostream>
#include "DataFormatTypes.h"

namespace larcv {

  class IOManager;
  class DataProductFactory;
  /**
    \class EventBase
    \brief Base class for an event data product (what is stored in output file), holding run/event ID + producer name.
  */
  class EventBase{
    friend class IOManager;
    friend class DataProductFactory;
  public:
    
    /// Default constructor
    EventBase()
      : _run    (kINVALID_SIZE)
      , _event  (kINVALID_SIZE)
    {}
    /// Copy ctor
    EventBase(const EventBase& rhs)
      : _producer(rhs._producer)
      , _run(rhs._run)
      , _event(rhs._event)
    {}
				      
    /// Default destructor
    virtual ~EventBase(){}
    /// Set all run/event to kINVALID_SIZE
    virtual void clear();
    /// Producer name getter
    const std::string & producer() const { return _producer; }
    /// Run number getter
    size_t run()    const { return _run;    }
    /// Event number getter
    size_t event()  const { return _event;  }
    /// Setter
    void set(size_t run, size_t event) { _run = run; _event=event;}
    /// Make sure run/event ID is set
    bool valid() const
    { return !(_run == kINVALID_SIZE || _event == kINVALID_SIZE); }
    /// Comparison opearator for run/event id
    inline bool operator==(const EventBase& rhs) const
    { return (_run == rhs.run() && _event == rhs.event()); }
    /// Comparison opearator for run/event id
    inline bool operator!=(const EventBase& rhs) const
    { return !((*this) == rhs); }
    /// Comparison opearator for run/event id (for sorted container like std::set and/or std::map)
    inline bool operator<(const EventBase& rhs) const
    {
      if(_run < rhs.run()) return true;
      if(_run > rhs.run()) return false;
      if(_event < rhs.event()) return true;
      if(_event > rhs.event()) return false;
      return false;
    }

    /// Formatted string key getter (a string key consists of 0-padded run and event id)
    std::string event_key() const;

  private:
    std::string _producer; ///< Producer name string
    size_t _run;    ///< LArSoft run number
    size_t _event;  ///< LArSoft event number
  };
}

#endif
/** @} */ // end of doxygen group 

