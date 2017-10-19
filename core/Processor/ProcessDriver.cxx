#ifndef PROCESSDRIVER_CXX
#define PROCESSDRIVER_CXX
#include <sstream>
#include <iomanip>
#include "ProcessDriver.h"
#include "ProcessFactory.h"
#include "Base/LArCVBaseUtilFunc.h"
namespace larcv {

  ProcessDriver::ProcessDriver(std::string name)
    : larcv_base(name)
    , _enable_filter(false)
    , _random_access(false)
    , _proc_v()
    , _processing(false)
    , _fout(nullptr)
    , _fout_name("")
    , _batch_start_entry(0)
    , _batch_num_entry(0)
  {}

  void ProcessDriver::reset()
  {
    LARCV_DEBUG() << "Called" << std::endl;
    _io.reset();
    _enable_filter = _random_access = false;
    for(size_t i=0; i<_proc_v.size(); ++i) { delete _proc_v[i]; _proc_v[i]=nullptr; }
    _proc_v.clear();
    _processing = false;
    _fout = nullptr;
    _fout_name = "";
  }

  void ProcessDriver::override_input_file(const std::vector<std::string>& flist)
  {
    LARCV_DEBUG() << "Called" << std::endl;
    _io.clear_in_file();
    for(auto const& f : flist) _io.add_in_file(f);
  }

  void ProcessDriver::override_output_file(const std::string fname)
  {
    LARCV_DEBUG() << "Called" << std::endl;
    _io.set_out_file(fname);
  }

  void ProcessDriver::override_ana_file(const std::string fname)
  {
    LARCV_DEBUG() << "Called" << std::endl;
    if(_processing) {
      LARCV_CRITICAL() << "Cannot change ana file name during processing!" << std::endl;
      throw larbys();
    }
    _fout_name = fname;
  }
  
  ProcessID_t ProcessDriver::process_id(std::string name) const
  {
    LARCV_DEBUG() << "Called" << std::endl;
    auto iter = _proc_m.find(name);
    if(iter == _proc_m.end()) {
      LARCV_CRITICAL() << "Process w/ name " << name << " not found..." << std::endl;
      throw larbys();
    }
    return (*iter).second;
  }

  std::vector<std::string> ProcessDriver::process_names() const
  {
    LARCV_DEBUG() << "Called" << std::endl;
    std::vector<std::string> res;
    res.reserve(_proc_m.size());
    for(auto const& name_id : _proc_m) res.push_back(name_id.first);
    return res;
  }

  const std::map<std::string,larcv::ProcessID_t>& ProcessDriver::process_map() const
  { LARCV_DEBUG() << "Called" << std::endl; return _proc_m; }

  const ProcessBase* ProcessDriver::process_ptr(size_t id) const
  {
    LARCV_DEBUG() << "Called" << std::endl;
    if(id >= _proc_v.size()) {
      LARCV_CRITICAL() << "Invalid ID requested: " << id << std::endl;
      throw larbys();
    }
    return _proc_v[id];
  }

  void ProcessDriver::configure(const std::string config_file)
  {
    LARCV_DEBUG() << "Called" << std::endl;
    // check state
    if(_processing) {
      LARCV_CRITICAL() << "Must call finalize() before calling initialize() after starting to process..." << std::endl;
      throw larbys();
    }
    // check cfg file
    if(config_file.empty()) {
      LARCV_CRITICAL() << "Config file not set!" << std::endl;
      throw larbys();
    }

    // check cfg content top level
    auto main_cfg = CreatePSetFromFile(config_file);
    if(!main_cfg.contains_pset(name())) {
      LARCV_CRITICAL() << "ProcessDriver configuration (" << name() << ") not found in the config file (dump below)" << std::endl
		       << main_cfg.dump()
		       << std::endl;
      throw larbys();
    }
    auto const cfg = main_cfg.get<larcv::PSet>(name());
    configure(cfg);
  }

  void ProcessDriver::configure(const PSet& cfg)
  {
    reset();

    // check io config exists
    LARCV_INFO() << "Retrieving IO config" << std::endl;
    PSet io_config("Empty");
    if(cfg.contains_pset("IOManager"))
      io_config = cfg.get<larcv::PSet>("IOManager");
    else if(cfg.contains_pset(std::string(name() + "IOManager")))
      io_config = cfg.get<larcv::PSet>(name() + "IOManager");
    else {
      LARCV_CRITICAL() << "IOManager config not found!" << std::endl
		       << cfg.dump()
		       << std::endl;
      throw larbys();
    }
    
    // check process config exists
    LARCV_INFO() << "Retrieving ProcessList" << std::endl;
    if(!cfg.contains_pset("ProcessList")) {
      LARCV_CRITICAL() << "ProcessList config not found!" << std::endl
		       << cfg.dump()
		       << std::endl;
      throw larbys();
    }
    auto const proc_config = cfg.get<larcv::PSet>("ProcessList");
    
    // Prepare IO manager
    LARCV_INFO() << "Configuring IO" << std::endl;
    _io = IOManager(io_config);
    // Set ProcessDriver
    LARCV_INFO() << "Retrieving self (ProcessDriver) config" << std::endl;
    set_verbosity((msg::Level_t)(cfg.get<unsigned short>("Verbosity",logger().level())));
    larcv::logger::get_shared().set(logger().level());
    _enable_filter = cfg.get<bool>("EnableFilter");
    _random_access = cfg.get<bool>("RandomAccess");
    _fout_name = cfg.get<std::string>("AnaFile","");
    _batch_start_entry = cfg.get<int>("StartEntry",0);
    _batch_num_entry   = cfg.get<int>("NumEntries",0);
    // Process list
    auto process_instance_type_v = cfg.get<std::vector<std::string> >("ProcessType");
    auto process_instance_name_v = cfg.get<std::vector<std::string> >("ProcessName");

    if(process_instance_type_v.size() != process_instance_name_v.size()) {
      LARCV_CRITICAL() << "ProcessType and ProcessName config parameters have different length! "
		       << "(" << process_instance_type_v.size() << " vs. " << process_instance_name_v.size() << ")" << std::endl;
      throw larbys();
    }

    LARCV_INFO() << "Start looping process list to instantiate processes" << std::endl;
    for(auto& p : _proc_v) if(p) { delete p; }
    _proc_v.clear();
    _proc_m.clear();
    _has_event_creator=false;
    for(size_t i=0; i<process_instance_type_v.size(); ++i) {
      auto const& name = process_instance_name_v[i];
      auto const& type = process_instance_type_v[i];
      if(_proc_m.find(name) != _proc_m.end()) {
	LARCV_CRITICAL() << "Duplicate Process name found: " << name << std::endl;
	throw larbys("Duplicate algorithm name found!");
      }
      size_t id = _proc_v.size();
      LARCV_NORMAL() << "Instantiating Process ID=" << id << " Type: " << type << " w/ Name: " << name << std::endl;
      /*
      if(!proc_config.contains_pset(name)) {
	LARCV_CRITICAL() << "Could not locate configuration for " << name << std::endl
			 << proc_config.dump() << std::endl;
	throw larbys();
      }
      */
      auto ptr = ProcessFactory::get().create(type,name);
      ptr->_id = id;
      ptr->_configure_(proc_config.get_pset(name));
      _proc_m[name] = id;
      if(ptr->event_creator()) {
	     if(_has_event_creator) {
	       LARCV_CRITICAL() << "Only 1 event creator is allowed to exist!" << std::endl;
	       throw larbys();
	     }
	     if((i+1) != process_instance_type_v.size()) {
	       LARCV_CRITICAL() << "Event creator must be set to the last of ProcessList!" << std::endl;
	       throw larbys();
	     }
	     _has_event_creator=true;
      }
      _proc_v.push_back(ptr);
    }
  }

  void ProcessDriver::initialize()
  {
    LARCV_DEBUG() << "Called" << std::endl;
    // check state
    if(_processing) {
      LARCV_CRITICAL() << "Must call finalize() before calling initialize() after starting to process..." << std::endl;
      throw larbys();
    }

    // Initialize IO
    LARCV_INFO() << "Initializing IO " << std::endl;
    _io.initialize();

    // Handle invalid cases
    auto const nentries = _io.get_n_entries();
    auto const io_mode  = _io.io_mode();
    
    // Random access + write mode cannot be combined
    if(_random_access && io_mode == IOManager::kWRITE) {
      LARCV_CRITICAL() << "Random access mode not supported for kWRITE IO mode!" << std::endl;
      throw larbys();
    }
    // No entries means nothing to do unless write mode
    if(!nentries && io_mode != IOManager::kWRITE) {
      LARCV_CRITICAL() << "No entries found from IO (kREAD/kBOTH mode cannot be run)!" << std::endl;
      throw larbys();
    }

    // Prepare analysis output file if needed
    if(!_fout_name.empty()) {
      LARCV_NORMAL() << "Opening analysis output file " << _fout_name << std::endl;
      _fout = TFile::Open(_fout_name.c_str(),"RECREATE");
    }

    // Initialize process
    for(auto& p : _proc_v) {
      LARCV_INFO() << "Initializing: " << p->name() << std::endl;
      p->initialize();
    }

    // Change state from to-be-initialized to to-process
    _processing = true;

    // Prepare ttree entry index array to follow in execution (randomize if specified
    LARCV_INFO() << "Preparing access index vector" << std::endl;
    if(nentries) {
      _access_entry_v.resize(nentries);
      for(size_t i=0; i<_access_entry_v.size(); ++i) _access_entry_v[i] = i;
      if(_random_access) std::random_shuffle(_access_entry_v.begin(),_access_entry_v.end());
    }

    _current_entry = 0;
  }

  bool ProcessDriver::_process_entry_()
  {
    // Private method to execute processes and change entry number record
    // This method does not perform any sanity check, hence private and 
    // should be used by wrapper method which performs necessary checks.

    // Execute
    bool good_status=true;
    //bool cleared=false;
    for(auto& p : _proc_v) {
      good_status = good_status && p->_process_(_io);
      if(!good_status && _enable_filter) break;
    }
    // No event-write to be done if _has_event_creator is set. Otherwise go ahead
    if(!_has_event_creator) {
      // If not read mode save entry
      if(_io.io_mode() != IOManager::kREAD && (!_enable_filter || good_status)) {
	//cleared = true;
	_io.save_entry();    
      }
      /*
      if(!cleared)
	_io.clear_entry();
      cleared=true;
      */
    }
    /*
    if(!cleared && _io.io_mode() == IOManager::kREAD) 
      _io.clear_entry();
    */
    // Bump up entry record
    ++_current_entry;

    return good_status;
  }

  bool ProcessDriver::process_entry()
  {
    LARCV_DEBUG() << "Called" << std::endl;
    // Public method to process "next" entry

    // Check state
    if(!_processing) {
      LARCV_CRITICAL() << "Must call initialize() before start processing!" << std::endl;
      throw larbys();
    }

    // Check if input entry exists in case of read/both io mode
    if(_io.io_mode() != IOManager::kWRITE) {

      if(_access_entry_v.size() <= _current_entry) {
	LARCV_NORMAL() << "Entry " << _current_entry << " exceeds available events in a file!" << std::endl;
	return false;
      }
      // if exist then move read pointer
      //_io.clear_entry();
      _io.read_entry(_access_entry_v[_current_entry]);
    }
    // Execute processes
    return _process_entry_();
  }
  
  bool ProcessDriver::process_entry(size_t entry, bool force_reload)
  {
    LARCV_DEBUG() << "Called" << std::endl;
    // Public method to process "specified" entry

    // Check state
    if(!_processing) {
      LARCV_CRITICAL() << "Must call initialize() before start processing!" << std::endl;
      throw larbys();
    }

    // Check if input entry exists in case of read/both io mode
    if(_io.io_mode() != IOManager::kWRITE) {

      if(_access_entry_v.size() <= entry) {
	LARCV_ERROR() << "Entry " << entry << " exceeds available events in a file!" << std::endl;
	return false;
      }
      // if exist then move read pointer
      //_io.clear_entry();
      _io.read_entry(_access_entry_v[entry],force_reload);
      _current_entry = entry;
    }
    // Execute processes
    return _process_entry_();
  }
  
  void ProcessDriver::batch_process(size_t start_entry,size_t num_entries){
    LARCV_DEBUG() << "Called" << std::endl;
    // Public method to execute num_entries starting from start_entry
    if(!start_entry) start_entry = _batch_start_entry;
    if(!num_entries) num_entries = _batch_num_entry;

    // Check state
    if(!_processing) {
      LARCV_CRITICAL() << "Must call initialize() before start processing!" << std::endl;
      throw larbys();
    }

    // Define the max entry index for processing (1 above last entry to be processed)
    size_t max_entry = start_entry + num_entries;

    // Check if start_entry is 0 for write mode (no entry should be specified for write mode)
    if(_io.io_mode() == IOManager::kWRITE){
      if(start_entry) {
	LARCV_CRITICAL() << "Cannot specify start entry (1st arg) in kWRITE IO mode!" << std::endl;
	throw larbys();
      }
      max_entry = _current_entry + num_entries;

    }else{
      _current_entry = start_entry;
      if(!num_entries) max_entry = start_entry + _io.get_n_entries();
    }

    // Make sure max entry does not exceed the physical max from input. If so, truncate.
    if(_io.io_mode() != IOManager::kWRITE && max_entry > _access_entry_v.size()){
      LARCV_WARNING() << "Requested to process entries from " << start_entry << " to " << max_entry-1 
		      << " ... but there are only " << _access_entry_v.size() << " entries in input!" << std::endl
		      << "Truncating the end entry to " << _access_entry_v.size()-1 << std::endl;
      max_entry = _access_entry_v.size();
    }

    // Batch-execute in while loop
    size_t num_processed=0;
    size_t num_fraction=(max_entry - _current_entry)/10;
    while(_current_entry < max_entry) {
      
      if(_io.io_mode() != IOManager::kWRITE) {
	//_io.clear_entry();
	_io.read_entry(_access_entry_v[_current_entry]);
      }
      _process_entry_();

      ++num_processed;
      if(!num_fraction) { 
	LARCV_NORMAL() << "Processed " << num_processed << " entries..." << std::endl;
      }else if(num_processed%num_fraction==0) {
	LARCV_NORMAL() << "Processed " << 10*int(num_processed/num_fraction) << " %..." << std::endl;
      }
    }

  }
  
  void ProcessDriver::finalize()
  {
    LARCV_DEBUG() << "called" << std::endl;

    for(auto& p : _proc_v) {
      LARCV_INFO() << "Finalizing: " << p->name() << std::endl;
      if(_fout) _fout->cd();
      p->_fout = _fout;
      p->finalize();
    }

    // Profile repor
    LARCV_INFO() << "Compiling time profile..." << std::endl;
    std::stringstream ss;
    for(auto& p : _proc_v) {
      if(!p->_profile) continue;
      ss << "  \033[93m" << std::setw(20) << std::setfill(' ') << p->name() << "\033[00m"
	 << " ... # call " << std::setw(5) << std::setfill(' ') << p->_proc_count
	 << " ... total time " << p->_proc_time << " [s]"
	 << " ... average time " << p->_proc_time / p->_proc_count << " [s/process]"
	 << std::endl;
    }

    std::string msg(ss.str());
    if(!msg.empty()) 
      LARCV_NORMAL() << "Simple time profiling requested and run..." << std::endl
		     << "  ================== " << name() << " Profile Report ==================" << std::endl
		     << msg
		     << std::endl;

    if(_fout) {
      LARCV_NORMAL() << "Closing analysis output file..." << std::endl;
      _fout->Close();
    }

    LARCV_INFO() << "Finalizing IO..." << std::endl;
    _io.finalize();
    LARCV_INFO() << "Resetting..." << std::endl;
    reset();
  }

  size_t ProcessDriver::get_tree_index( size_t entry ) const {
    if (entry<_access_entry_v.size() )
      return _access_entry_v.at(entry);
    else
      throw larbys();
    return 0;
  }
  
}

#endif
