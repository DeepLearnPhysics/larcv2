#ifndef LARLITE_LARLITESUPERADRIVER_CXX
#define LARLITE_LARLITESUPERADRIVER_CXX

#include "LArLiteSuperaDriver.h"
#include "DataFormat/hit.h"
#include "DataFormat/wire.h"
#include "DataFormat/opdetwaveform.h"
#include "DataFormat/mctruth.h"
#include "DataFormat/mctrack.h"
#include "DataFormat/mcshower.h"
#include "DataFormat/simch.h"
#include "DataFormat/chstatus.h"
namespace larlite {

  void LArLiteSuperaDriver::larcv_configure(std::string fname)
  { _supera.configure(fname); }
  
  void LArLiteSuperaDriver::larcv_output_file(std::string fout)
  { _supera.override_output_file(fout); }

  bool LArLiteSuperaDriver::initialize() {

    _supera.initialize();
    return true;
  }
  
  bool LArLiteSuperaDriver::analyze(storage_manager* storage) {

    //
    // set data pointers
    //

    // hit
    for(auto const& label : _supera.DataLabels(::supera::LArDataType_t::kLArHit_t)) {
      if(label.empty()) continue;
      print(msg::kINFO,Form("Retrieving hit data by %s",label.c_str()));
      auto ev_data = storage->get_data<event_hit>(label);
      if(!ev_data) {
	std::cerr << "Could not find data by producer " << label << std::endl;
	throw DataFormatException();
      }
      _supera.SetDataPointer(*ev_data,label);
    }

    // wire
    for(auto const& label : _supera.DataLabels(::supera::LArDataType_t::kLArWire_t)) {
      if(label.empty()) continue;
      print(msg::kINFO,Form("Retrieving wire data by %s",label.c_str()));
      auto ev_data = storage->get_data<event_wire>(label);
      if(!ev_data) {
	std::cerr << "Could not find data by producer " << label << std::endl;
	throw DataFormatException();
      }
      _supera.SetDataPointer(*ev_data,label);
    }

    // opdigit
    for(auto const& label : _supera.DataLabels(::supera::LArDataType_t::kLArOpDigit_t)) {
      if(label.empty()) continue;
      print(msg::kINFO,Form("Retrieving opdigit data by %s",label.c_str()));
      auto ev_data = storage->get_data<event_opdetwaveform>(label);
      if(!ev_data) {
	std::cerr << "Could not find data by producer " << label << std::endl;
	throw DataFormatException();
      }
      _supera.SetDataPointer(*ev_data,label);
    }

    // mctruth
    for(auto const& label : _supera.DataLabels(::supera::LArDataType_t::kLArMCTruth_t)) {
      if(label.empty()) continue;
      print(msg::kINFO,Form("Retrieving mctruth data by %s",label.c_str()));
      auto ev_data = storage->get_data<event_mctruth>(label);
      if(!ev_data) {
	std::cerr << "Could not find data by producer " << label << std::endl;
	throw DataFormatException();
      }
      _supera.SetDataPointer(*ev_data,label);
    }

    // mctrack
    for(auto const& label : _supera.DataLabels(::supera::LArDataType_t::kLArMCTrack_t)) {
      if(label.empty()) continue;
      print(msg::kINFO,Form("Retrieving mctrack data by %s",label.c_str()));
      auto ev_data = storage->get_data<event_mctrack>(label);
      if(!ev_data) {
	std::cerr << "Could not find data by producer " << label << std::endl;
	throw DataFormatException();
      }
      _supera.SetDataPointer(*ev_data,label);
    }

    // mcshower
    for(auto const& label : _supera.DataLabels(::supera::LArDataType_t::kLArMCShower_t)) {
      if(label.empty()) continue;
      print(msg::kINFO,Form("Retrieving mcshower data by %s",label.c_str()));
      auto ev_data = storage->get_data<event_mcshower>(label);
      if(!ev_data) {
	std::cerr << "Could not find data by producer " << label << std::endl;
	throw DataFormatException();
      }
      _supera.SetDataPointer(*ev_data,label);
    }

    // simch
    for(auto const& label : _supera.DataLabels(::supera::LArDataType_t::kLArSimCh_t)) {
      if(label.empty()) continue;
      print(msg::kINFO,Form("Retrieving simch data by %s",label.c_str()));
      auto ev_data = storage->get_data<event_simch>(label);
      if(!ev_data) {
	std::cerr << "Could not find data by producer " << label << std::endl;
	throw DataFormatException();
      }
      _supera.SetDataPointer(*ev_data,label);
    }

    // chstatus
    auto supera_chstatus = _supera.SuperaChStatusPointer();
    if(supera_chstatus) {
      auto ev_data = storage->get_data<event_chstatus>(supera_chstatus->InputLabel());
      if(!ev_data) {
	std::cerr << "Could not find data by producer " << supera_chstatus->InputLabel() << std::endl;
	throw DataFormatException();
      }

      for(auto const& chs : *ev_data) {
	auto const& pid = chs.plane();
	auto const& status_v = chs.status();
	for(size_t wire=0; wire<status_v.size(); ++wire)
	  supera_chstatus->set_chstatus(pid.Plane,wire,status_v[wire]);
      }
    }

    //
    // execute supera
    //
    _supera.process(storage->run_id(), storage->subrun_id(), storage->event_id());
    
    return true;
  }

  bool LArLiteSuperaDriver::finalize() {

    _supera.finalize();
    return true;
  }

}
#endif
