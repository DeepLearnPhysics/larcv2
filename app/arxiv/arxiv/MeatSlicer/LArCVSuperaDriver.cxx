#ifndef LARCVSUPERADRIVER_CXX
#define LARCVSUPERADRIVER_CXX

#include "LArCVSuperaDriver.h"

namespace larcv {

  LArCVSuperaDriver::LArCVSuperaDriver()
    : _driver("ProcessDriver")
    , _supera_chstatus_ptr(nullptr)
  {}

  void LArCVSuperaDriver::configure(const std::string cfg_file)
  {
    _driver.configure(cfg_file);
  }
  
  void LArCVSuperaDriver::configure(const larcv::PSet& cfg)
  {
    _driver.configure(cfg);
  }

  void LArCVSuperaDriver::override_input_file(const std::vector<std::string>& flist)
  {
    _driver.override_input_file(flist);
  }
  
  void LArCVSuperaDriver::override_output_file(const std::string fname)
  {
    _driver.override_output_file(fname);
  }

  SuperaChStatus* LArCVSuperaDriver::SuperaChStatusPointer()
  { return _supera_chstatus_ptr; }
  
  const std::set<std::string>& LArCVSuperaDriver::DataLabels(supera::LArDataType_t type) const
  {
    static std::set<std::string> empty_string;
    auto iter = _data_request_m.find(type);
    if(iter == _data_request_m.end()) return empty_string;
    return (*iter).second;
  }

  void LArCVSuperaDriver::initialize() {

    _driver.initialize();
    _supera_idx_v.clear();

    for(size_t idx=0; idx<_driver.process_names().size(); ++idx) {

      auto proc_ptr = _driver.process_ptr(idx);

      if(proc_ptr->is("SuperaMetaMaker") && idx) {
	LARCV_CRITICAL() << "SuperaMetaMaker must be the first module!" << std::endl;
	throw larbys();
      }
      
      if(proc_ptr->is("Supera") || proc_ptr->is("SuperaMetaMaker")) {
	_supera_idx_v.push_back(idx);
	for(size_t data_type=0; data_type<(size_t)(supera::LArDataType_t::kLArDataTypeMax); ++data_type) {
	  auto const& label = ((SuperaBase*)proc_ptr)->LArDataLabel((supera::LArDataType_t)data_type);
	  if(label.empty()) continue;
	  _data_request_m[(supera::LArDataType_t)(data_type)].insert(label);
	}
      }
      
      if(_driver.process_ptr(idx)->is("SuperaChStatus")) {
	_supera_chstatus_ptr = (SuperaChStatus*)(_driver.process_ptr(idx));
	for(size_t data_type=0; data_type<(size_t)(supera::LArDataType_t::kLArDataTypeMax); ++data_type) {
	  auto const& label = _supera_chstatus_ptr->LArDataLabel((supera::LArDataType_t)data_type);
	  if(label.empty()) continue;
	  _data_request_m[(supera::LArDataType_t)(data_type)].insert(label);
	}
      }
      
    }
  }

  bool LArCVSuperaDriver::process(size_t run, size_t subrun, size_t event) 
  {
    _driver.set_id(run,subrun,event);
    return _driver.process_entry();
  }

  void LArCVSuperaDriver::finalize() { _driver.finalize(); }

}
#endif
