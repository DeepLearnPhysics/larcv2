/**
 * \file EventBase.h
 *
 * \ingroup DataFormat
 * 
 * \brief Class def header for a class EventBase
 *
 * @author kazuhiro
 */

/** \addtogroup DataFormat

    @{*/
#ifndef EVENTBASE_H
#define EVENTBASE_H

#include <iostream>
#include "DataFormatTypes.h"

namespace larcv {
  class IOManager;
  class DataProductFactory;
  /**
    \class EventBase
    Base class for an event data product (what is stored in output file), holding run/subrun/event ID + producer name.
  */
  class EventBase{
    friend class IOManager;
    friend class DataProductFactory;
  public:
    
    /// Default constructor
    EventBase() : 
      _run      (kINVALID_SIZE)
      , _subrun (kINVALID_SIZE)
      , _event  (kINVALID_SIZE)
    {}
    /// Copy ctor
    EventBase(const EventBase& rhs) : _producer(rhs._producer)
				    , _run(rhs._run)
				    , _subrun(rhs._subrun)
				    , _event(rhs._event)
    {}
				      
    /// Default destructor
    virtual ~EventBase(){}
    /// Set all run/subrun/event to kINVALID_SIZE
    virtual void clear();
    /// Producer name getter
    const std::string & producer() const { return _producer; }
    /// Run number getter
    size_t run()    const { return _run;    }
    /// SubRun number getter
    size_t subrun() const { return _subrun; }
    /// Event number getter
    size_t event()  const { return _event;  }
    /// Make sure run/subrun/event ID is set
    bool valid() const
    { return !(_run == kINVALID_SIZE || _subrun == kINVALID_SIZE || _event == kINVALID_SIZE); }
    /// Comparison opearator for run/subrun/event id
    inline bool operator==(const EventBase& rhs) const
    { return (_run == rhs.run() && _subrun == rhs.subrun() && _event == rhs.event()); }
    /// Comparison opearator for run/subrun/event id
    inline bool operator!=(const EventBase& rhs) const
    { return !((*this) == rhs); }
    /// Comparison opearator for run/subrun/event id (for sorted container like std::set and/or std::map)
    inline bool operator<(const EventBase& rhs) const
    {
      if(_run < rhs.run()) return true;
      if(_run > rhs.run()) return false;
      if(_subrun < rhs.subrun()) return true;
      if(_subrun > rhs.subrun()) return false;
      if(_event < rhs.event()) return true;
      if(_event > rhs.event()) return false;
      return false;
    }

    /// Formatted string key getter (a string key consists of 0-padded run, subrun, and event id)
    std::string event_key() const;

    /// Setting the id
    inline void set_id(const size_t run, const size_t subrun, const size_t event)
    { _run = run; _subrun = subrun; _event = event; }
    
  private:
    std::string _producer; ///< Producer name string
    size_t _run;    ///< LArSoft run number
    size_t _subrun; ///< LArSoft sub-run number
    size_t _event;  ///< LArSoft event number
  };
}

#endif
/** @} */ // end of doxygen group 

