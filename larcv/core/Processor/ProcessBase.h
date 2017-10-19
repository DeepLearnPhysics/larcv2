/**
 * \file ProcessBase.h
 *
 * \ingroup core_Processor
 * 
 * \brief Class def header for a class larcv::ProcessBase
 *
 * @author drinkingkazu
 */

/** \addtogroup core_Processor

    @{*/
#ifndef PROCESSBASE_H
#define PROCESSBASE_H

#include "larcv/core/Base/Watch.h"
#include "larcv/core/DataFormat/IOManager.h"
#include "ProcessorTypes.h"
namespace larcv {

  class ProcessDriver;
  class ProcessFactory;
  /**
     \class ProcessBase
     @brief A base class for "process module" to be run by larcv::ProcessDriver
     @detail Inherited class must implment 4 pure virtual functions that are called by larcv::ProcessDriver instance.\n
     ProcessBase::configure(const larcv::PSet) is called first with the argument passing the configuration parameters.\n
     ProcessBase::initialize() is called after configure. This is where you may want to initialize variables.\n
     ProcessBase::process(larcv::IOManager&) is called for every event. The argument provides an access to event data.\n
     ProcessBase::finalize() is called after larcv::ProcessDriver finished looping over all events.\n
  */
  class ProcessBase : public larcv_base {
    friend class ProcessDriver;
    friend class ProcessFactory;

  public:
    
    /// Default constructor
    ProcessBase(const std::string name="ProcessBase");
    
    /// Default destructor
    virtual ~ProcessBase(){}

    //
    // Four pure virtual functions that larcv::ProcessDriver calls and need implementation.
    //
    /// Called first with the argument passing the configuration parameters.
    virtual void configure(const PSet&) = 0;
    /// Called after configure, this is where you should initialize variables to be stored in an output analysis file.
    virtual void initialize() = 0;
    /// Called per-event, this is where you should implement your per-event action/analysis.
    virtual bool process(IOManager& mgr) = 0;
    /// Called after event loop is over. This is where you can store your histograms etc. to an output analysis file.
    virtual void finalize() = 0;

    /// Returns true/false if there is an analysis ROOT file to store the output objects
    bool has_ana_file() const
    { return _fout != nullptr; }
    /// Returns analysis ROOT file if exits (else throws an exception)
    TFile& ana_file()
    { if(!_fout) throw larbys("ana file does not exist"); return *_fout; }

    //
    // Following functions are 
    //
    /// Only for experts: allows a loose grouping for a set of ProcessBase inherit classes via true/false return to a "question".
    virtual bool is(const std::string question) const;
    /// Only for experts: larcv::ProcessDriver to see if this module can create a new event or not
    bool event_creator() const
    { return _event_creator; }

  private:

    void _configure_(const PSet&);

    bool _process_(IOManager& mgr);

    bool _event_creator;    ///< special flag to mark this algorithm an event creator
#ifndef __CINT__
    larcv::Watch _watch;    ///< algorithm profile stopwatch
#endif
    double _proc_time;      ///< algorithm execution time record (cumulative)
    size_t _proc_count;     ///< algorithm execution counter (cumulative)
    
    larcv::ProcessID_t _id; ///< unique algorithm identifier
    bool _profile;          ///< measure process time if profile flag is on
    std::string _typename;  ///< process type from factory
    TFile* _fout;           ///< output analysis file
  };
}

#endif
/** @} */ // end of doxygen group 

