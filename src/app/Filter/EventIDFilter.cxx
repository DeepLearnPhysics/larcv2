#ifndef __EventIDFILTER_CXX__
#define __EventIDFILTER_CXX__

#include "EventIDFilter.h"
#include "larcv/core/CPPUtil/CSVReader.h"

namespace larcv {

  static EventIDFilterProcessFactory __global_EventIDFilterProcessFactory__;

  EventIDFilter::EventIDFilter(const std::string name)
    : ProcessBase(name), _tree(nullptr)
  {}
    
  void EventIDFilter::configure(const PSet& cfg)
  {
    _ref_producer=cfg.get<std::string>("RefProducer");
    _ref_type=cfg.get<std::string>("RefType");
    _remove_duplicate=cfg.get<int>("RemoveDuplicate",0);
    
    auto file_path=cfg.get<std::string>("CSVFilePath");
    auto format=cfg.get<std::string>("Format","II");
    auto data = larcv::read_csv(file_path,format);
    auto const& run_v = data.get<int>("run");
    auto const& subrun_v = data.get<int>("subrun");
    auto const& event_v = data.get<int>("event");
    _id_m.clear();
    EventBase id;
    for(size_t i=0; i<run_v.size(); ++i) {
      id.set_id( run_v[i], subrun_v[i], event_v[i] );
      _id_m[id] = false;
    }
    LARCV_INFO() << "Registered: " << _id_m.size() << " unique events to be kept..." << std::endl;
  }

  void EventIDFilter::initialize()
  {
    if(has_ana_file()) {
      _tree = new TTree("EventIDFilter","");
      _tree->Branch("fname"  , &_fname);
      _tree->Branch("run"    , &_run    , "run/I");
      _tree->Branch("subrun" , &_subrun , "subrun/I");
      _tree->Branch("event"  , &_event  , "event/I");
    }
  }

  bool EventIDFilter::process(IOManager& mgr)
  {
    auto ptr = mgr.get_data(_ref_type,_ref_producer);

    EventBase ref_id;
    ref_id.set_id(ptr->run(),ptr->subrun(),ptr->event());
    auto itr = _id_m.find(ref_id);

    bool keepit = (itr != _id_m.end());
    LARCV_INFO() << "Event key: " << ptr->event_key() << " ... keep it? " << keepit << std::endl;
    bool duplicate = false;
    if(keepit) {
      duplicate = (*itr).second;
      if(duplicate) LARCV_WARNING() << "Run " << ref_id.run() << " SubRun " << ref_id.subrun() << " Event " << ref_id.event() << " is duplicated!!!" << std::endl;
      (*itr).second = true;
    }
    if(duplicate && _remove_duplicate) return false;

    if(has_ana_file()) {
      _fname  = (std::string) mgr.file_list().front();
      _run    = (int) ptr->run();
      _subrun = (int) ptr->subrun();
      _event  = (int) ptr->event();
      _tree->Fill();
    }
    return keepit;
  }

  void EventIDFilter::finalize()
  {
    for(auto const& id_used : _id_m) {
      if(id_used.second) continue;
      LARCV_WARNING() << "Event ID not found in data file (unused): Run " << id_used.first.run()
		      << " SubRun " << id_used.first.subrun() 
		      << " Event " << id_used.first.event() << std::endl;
    }
    if(has_ana_file())
      _tree->Write();
  }

}
#endif
