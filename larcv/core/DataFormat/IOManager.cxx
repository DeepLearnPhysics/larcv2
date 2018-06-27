#ifndef __LARCV_IOMANAGER_CXX__
#define __LARCV_IOMANAGER_CXX__

#include "IOManager.h"
#include "DataProductFactory.h"
#include "larcv/core/Base/LArCVBaseUtilFunc.h"
#include <algorithm>

#include <mutex>
std::mutex __ioman_mtx;
namespace larcv {

  IOManager::IOManager(IOMode_t mode, std::string name)
    : larcv_base(name)
    , _io_mode          ( mode          )
    , _prepared         ( false         )
    , _out_file         ( nullptr       )
    , _in_tree_index    ( 0             )
    , _out_tree_index   ( 0             )
    , _in_tree_entries  ( 0             )
    , _out_tree_entries ( 0             )
    , _out_file_name    ( ""            )
    , _in_file_v        ()
    , _in_dir_v         ()
    , _key_list         ()
    , _out_tree_v       ()
    , _in_tree_v        ()
    , _in_tree_index_v  ()
    , _product_ctr      (0)
    , _product_ptr_v    ()
    , _product_type_v   ()
  { reset(); }

  IOManager::IOManager(const PSet& cfg)
    : IOManager(kREAD, cfg.get<std::string>("Name"))
  {
    reset();
    configure(cfg);
  }

  IOManager::IOManager(std::string config_file, std::string name)
    : IOManager(kREAD, name)
  {
    // check cfg file
    if (config_file.empty()) {
      LARCV_CRITICAL() << "Config file not set!" << std::endl;
      throw larbys();
    }

    // check cfg content top level
    auto main_cfg = CreatePSetFromFile(config_file);
    if (!main_cfg.contains_pset(name)) {
      LARCV_CRITICAL() << "IOManager configuration (" << name << ") not found in the config file (dump below)" << std::endl
                       << main_cfg.dump()
                       << std::endl;
      throw larbys();
    }
    auto const cfg = main_cfg.get<larcv::PSet>(name);
    reset();
    configure(cfg);
  }

  void IOManager::add_in_file(const std::string filename, const std::string dirname)
  { _in_file_v.push_back(filename); _in_dir_v.push_back(dirname); }

  void IOManager::clear_in_file()
  { _in_file_v.clear(); }

  void IOManager::set_out_file(const std::string name)
  { _out_file_name = name; }

  /*
  std::string IOManager::product_type(const size_t id) const
  {
    if( id > _product_type_v.size() ) {
      LARCV_CRITICAL() << "Product ID " << id << " does not exist... " << std::endl;
      throw larbys();
    }
    return _product_type_v[id];
  }
  */
  void IOManager::configure(const PSet& cfg)
  {
    if (_prepared) throw larbys("Cannot call configure() after initialize()!");

    set_verbosity((msg::Level_t)(cfg.get<unsigned short>("Verbosity", logger().level())));
    _io_mode = (IOMode_t)(cfg.get<unsigned short>("IOMode"));
    _out_file_name = cfg.get<std::string>("OutFileName", "");

    // Figure out input files
    _in_file_v.clear();
    _in_file_v = cfg.get<std::vector<std::string> >("InputFiles", _in_file_v);
    _in_dir_v.clear();
    _in_dir_v = cfg.get<std::vector<std::string> >("InputDirs", _in_dir_v);
    if (_in_dir_v.empty()) _in_dir_v.resize(_in_file_v.size(), "");
    if (_in_dir_v.size() != _in_file_v.size()) {
      LARCV_CRITICAL() << "# of input file (" << _in_file_v.size()
                       << ") != # of input dir (" << _in_dir_v.size() << ")!" << std::endl;
      throw larbys();
    }

    std::vector<std::string> store_only_name;
    std::vector<std::string> store_only_type;
    store_only_name = cfg.get<std::vector<std::string> >("StoreOnlyName", store_only_name);
    store_only_type = cfg.get<std::vector<std::string> >("StoreOnlyType", store_only_type);
    if (store_only_name.size() != store_only_type.size()) {
      LARCV_CRITICAL() << "StoreOnlyName and StoreOnlyType has different lengths!" << std::endl;
      throw larbys();
    }
    for (size_t i = 0; i < store_only_name.size(); ++i) {
      auto& val = _store_only[store_only_type[i]];
      val.insert(store_only_name[i]);
    }

    std::vector<std::string> read_only_name;
    std::vector<std::string> read_only_type;
    read_only_name = cfg.get<std::vector<std::string> >("ReadOnlyName", read_only_name);
    read_only_type = cfg.get<std::vector<std::string> >("ReadOnlyType", read_only_type);
    if (read_only_name.size() != read_only_type.size()) {
      LARCV_CRITICAL() << "ReadOnlyName and ReadOnlyType has different lengths!" << std::endl;
      throw larbys();
    }
    for (size_t i = 0; i < read_only_name.size(); ++i) {
      auto& val = _read_only[read_only_type[i]];
      val.insert(read_only_name[i]);
    }
  }

  bool IOManager::initialize()
  {
    LARCV_DEBUG() << "start" << std::endl;

    if (_io_mode != kREAD) {

      if (_out_file_name.empty()) throw larbys("Must set output file name!");
      LARCV_INFO() << "Opening an output file: " << _out_file_name << std::endl;
      _out_file = TFile::Open(_out_file_name.c_str(), "RECREATE");

    }

    if (_io_mode != kWRITE) {
      prepare_input();
      if (!_in_tree_entries) {
        LARCV_ERROR() << "Found 0 entries from input files..." << std::endl;
        return false;
      }
      LARCV_NORMAL() << "Prepared input with " << _in_tree_entries << " entries..." << std::endl;
      _read_id_bool.clear();
      _read_id_bool.resize(_product_ctr, true);
    }


    // Now handle "store-only" configuration
    _store_id_bool.clear();
    if (_io_mode != kREAD && _store_only.size()) {
      std::vector<size_t> store_only_id;
      for (auto const& type_name_s: _store_only) {
        auto const& type = type_name_s.first;
        auto const& name_s = type_name_s.second;
        for (auto const& name : name_s)
          store_only_id.push_back(register_producer(ProducerName_t(type, name)));
      }
      _store_id_bool.resize(_product_ctr, false);
      if ( _product_ctr > _read_id_bool.size() ) // append to read-in counters
        _read_id_bool.resize(_product_ctr, false);
      for (auto const& id : store_only_id) _store_id_bool.at(id) = true;
    }

    _in_tree_index = 0;
    _out_tree_index = 0;
    _prepared = true;

    return true;
  }

  size_t IOManager::register_producer(const ProducerName_t& name)
  {
    LARCV_DEBUG() << "start" << std::endl;

    std::string tree_name = name.first + "_" + name.second + "_tree";
    std::string tree_desc = name.second + " tree";
    std::string br_name = name.first + "_" + name.second + "_branch";

    LARCV_INFO() << "Requested to register a producer: " << name.second << " (TTree " << tree_name << ")" << std::endl;

    auto in_iter = _key_list.find(name);

    if (in_iter != _key_list.end()) {
      LARCV_INFO() << "... already registered. Returning a registered key " << (*in_iter).second << std::endl;
      return (*in_iter).second;
    }

    _product_ptr_v[_product_ctr]  = (EventBase*)(DataProductFactory::get().create(name));
    _product_type_v[_product_ctr] = name.first;

    const ProducerID_t id = _product_ctr;
    _key_list.insert(std::make_pair(name, id));

    _product_ctr += 1;

    LARCV_INFO() << "It is a new producer registration (key=" << id << ")" << std::endl;

    // Set event ID
    if (_event_id.valid()) {
      _product_ptr_v[id]->_run    = _event_id.run();
      _product_ptr_v[id]->_subrun = _event_id.subrun();
      _product_ptr_v[id]->_event  = _event_id.event();
    } else if (_set_event_id.valid()) {
      _product_ptr_v[id]->_run    = _set_event_id.run();
      _product_ptr_v[id]->_subrun = _set_event_id.subrun();
      _product_ptr_v[id]->_event  = _set_event_id.event();
    }

    if (_io_mode != kWRITE) {
      LARCV_INFO() << "kREAD/kBOTH mode: creating an input TChain" << std::endl;
      LARCV_DEBUG() << "Branch name: " << br_name << " data pointer: " << _product_ptr_v[id] << std::endl;
      auto in_tree_ptr = new TChain(tree_name.c_str(), tree_desc.c_str());
      in_tree_ptr->SetBranchAddress(br_name.c_str(), &(_product_ptr_v[id]));
      _in_tree_v[id] = in_tree_ptr;
      _in_tree_index_v.push_back(kINVALID_SIZE);
      _in_tree_entries_v.push_back(0);
    }

    if (_io_mode != kREAD) {
      LARCV_INFO() << "kWRITE/kBOTH mode: creating an output TTree" << std::endl;
      LARCV_DEBUG() << "Branch name: " << br_name << " data pointer: " << _product_ptr_v[id] << "(" << id << "/" << _product_ptr_v.size() << ")" << std::endl;
      _out_file->cd();
      _out_tree_v[id] = new TTree(tree_name.c_str(), tree_desc.c_str());
      auto out_br_ptr = _out_tree_v[id]->Branch(br_name.c_str(), &(_product_ptr_v[id]));
      LARCV_DEBUG() << "Created TTree @ " << _out_tree_v[id] << " ... TBranch @ " << out_br_ptr << std::endl;
    }

    return id;
  }

  void IOManager::prepare_input()
  {
    LARCV_DEBUG() << "start" << std::endl;
    if (_product_ctr) {
      LARCV_CRITICAL() << "Cannot call prepare_input before calling reset()!" << std::endl;
      throw larbys();
    }

    LARCV_INFO() << "Start inspecting " << _in_file_v.size() << "files" << std::endl;
    for (size_t i = 0; i < _in_file_v.size(); ++i) {

      auto const& fname = _in_file_v[i];
      auto const& dname = _in_dir_v[i];

      TFile *fin = TFile::Open(fname.c_str(), "READ");
      if (!fin) {
        LARCV_CRITICAL() << "Open attempt failed for a file: " << fname << std::endl;
        throw larbys();
      }

      LARCV_NORMAL() << "Opening a file in READ mode: " << fname << std::endl;
      TDirectoryFile* fin_dir = 0;
      if (dname.empty()) fin_dir = fin;
      else {
        TObject* obj = fin->Get(dname.c_str());
        if (!obj || std::string(obj->ClassName()) != "TDirectoryFile") {
          LARCV_CRITICAL() << "Could not locate TDirectoryFile: " << dname << std::endl;
          throw larbys();
        }
        fin_dir = (TDirectoryFile*)obj;
      }

      TList* key_list = fin_dir->GetListOfKeys();
      TIter key_iter(key_list);
      std::set<std::string> processed_object;
      while (1) {
        TObject* obj = key_iter.Next();
        if (!obj) break;
        if (processed_object.find(obj->GetName()) != processed_object.end()) continue;
        obj = fin_dir->Get(obj->GetName());
        LARCV_DEBUG() << "Found object " << obj->GetName() << " (type=" << obj->ClassName() << ")" << std::endl;
        processed_object.insert(obj->GetName());

        if (std::string(obj->ClassName()) != "TTree") {
          LARCV_DEBUG() << "Skipping " << obj->GetName() << " ... (not TTree)" << std::endl;
          continue;
        }

        std::string obj_name = obj->GetName();

        char c[2] = "_";
        if (obj_name.find_first_of(c) > obj_name.size() ||
            obj_name.find_first_of(c) == obj_name.find_last_of(c)) {
          LARCV_INFO() << "Skipping " << obj->GetName() << " ... (not LArCV TTree)" << std::endl;
          continue;
        }

        std::string type_name( obj_name.substr(0, obj_name.find_first_of(c)) );
        std::string suffix( obj_name.substr(obj_name.find_last_of(c) + 1, obj_name.size() - obj_name.find_last_of(c)) );
        std::string producer_name( obj_name.substr(obj_name.find_first_of(c) + 1, obj_name.find_last_of(c) - obj_name.find_first_of(c) - 1) );

        if (suffix != "tree") {
          LARCV_INFO() << "Skipping " << obj->GetName() << " ... (not LArCV TTree)" << std::endl;
          continue;
        }

        // If read-only is specified and not in a list, skip
        if (_read_only.size()) {
          bool skip = true;
          auto const& type_name_iter = _read_only.find(type_name);
          if(type_name_iter != _read_only.end()) {
            auto const& type_name_s = (*type_name_iter).second;
            auto const& name_iter = type_name_s.find(producer_name);
            if(name_iter != type_name_s.end()) skip = false;
          }
          if (skip) {
            LARCV_NORMAL() << "Skipping: producer=" << producer_name << " type= " << type_name << std::endl;
            continue;
          }
          LARCV_INFO() << "Not skipping: producer=" << producer_name << " type= " << type_name << std::endl;
        }

        auto id = register_producer(ProducerName_t(type_name, producer_name));
        LARCV_INFO() << "Registered: producer=" << producer_name << " Key=" << id << std::endl;
        _in_tree_v[id]->AddFile(fname.c_str());
      }
    }

    if (!_in_tree_v.front()) {
      _in_tree_entries = 0;
      return;
    }

    // Get tree entries
    _in_tree_entries = kINVALID_SIZE;
    for (size_t id = 0; id < _in_tree_v.size(); ++id) {
      auto& t = _in_tree_v[id];
      if (!t) break;
      size_t tmp_entries = t->GetEntries();
      t->GetEntry(0);
      LARCV_INFO() << "TTree " << t->GetName() << " has " << tmp_entries << " entries" << std::endl;
      if (_in_tree_entries == kINVALID_SIZE) _in_tree_entries = tmp_entries;
      else _in_tree_entries = (_in_tree_entries < tmp_entries ? _in_tree_entries : tmp_entries);
      _in_tree_entries_v[id] = tmp_entries;
    }

  }

  bool IOManager::read_entry(const size_t index, bool force_reload)
  {
    LARCV_DEBUG() << "start" << std::endl;
    if (_io_mode == kWRITE) {
      LARCV_WARNING() << "Nothing to read in kWRITE mode..." << std::endl;
      return false;
    }
    if (!_prepared) {
      LARCV_CRITICAL() << "Cannot be called before initialize()!" << std::endl;
      throw larbys();
    }
    if (index >= _in_tree_entries) {
      LARCV_ERROR() << "Input only has " << _in_tree_entries << " entries!" << std::endl;
      return false;
    }
    if (_in_tree_index != index) {
      _in_tree_index = index;
      _event_id.clear();
      _set_event_id.clear();
    } else if (force_reload) {
      _in_tree_index = index;
      for (auto& v : _in_tree_index_v) v = kINVALID_SIZE;
      _event_id.clear();
      _set_event_id.clear();
    }
    LARCV_DEBUG() << "Current input tree index: " << _in_tree_index << std::endl;
    return true;
  }

  bool IOManager::save_entry()
  {
    LARCV_DEBUG() << "start" << std::endl;

    if (!_prepared) {
      LARCV_CRITICAL() << "Cannot be called before initialize()!" << std::endl;
      throw larbys();
    }

    if (_io_mode == kREAD) {
      LARCV_ERROR() << "Cannot save in READ mode..." << std::endl;
      return false;
    }

    // in kBOTH mode make sure all TTree entries are read-in
    if (_io_mode == kBOTH) {
      for (size_t id = 0; id < _in_tree_index_v.size(); ++id) {
        if (_store_id_bool.size() && (id >= _store_id_bool.size() || !_store_id_bool[id])) continue;
        if (_in_tree_index_v[id] == _in_tree_index) continue;
        get_data(id);
      }
    }

    LARCV_INFO() << "Saving new entry " << std::endl;

    set_id();

    if (_store_id_bool.empty()) {

      for (auto& p : _product_ptr_v)  {
        if (!p) break;
        if (!p->valid()) {
          LARCV_CRITICAL() << "Product by a producer " << p->producer()
                           << " has an invalid event id: ("
                           << p->run() << "," << p->subrun() << "," << p->event() << ")" << std::endl;
          throw larbys("Must set an event ID to store!");
        }
      }

      for (size_t i = 0; i < _out_tree_v.size(); ++i) {
        auto& t = _out_tree_v[i];
        auto& p = _product_ptr_v[i];
        if (!t) break;
        LARCV_DEBUG() << "Saving " << t->GetName() << " entry " << t->GetEntries() << std::endl;
        t->Fill();
        p->clear();
      }

    } else {

      for (size_t i = 0; i < _store_id_bool.size(); ++i) {
        auto const& p = _product_ptr_v[i];
        if (!_store_id_bool[i]) {
          p->clear();
          continue;
        }
        if (!p->valid()) {
          LARCV_CRITICAL() << "Invalid event id: (" << p->run() << "," << p->subrun() << "," << p->event() << ")" << std::endl;
          throw larbys("Must set an event ID to store!");
        }
      }

      for (size_t i = 0; i < _store_id_bool.size(); ++i) {
        if (!_store_id_bool[i]) continue;
        auto& t = _out_tree_v[i];
        auto& p = _product_ptr_v[i];
        LARCV_DEBUG() << "Saving " << t->GetName() << " entry " << t->GetEntries() << std::endl;
        t->Fill();
        p->clear();
      }
    }

    clear_entry();

    _out_tree_entries += 1;
    _out_tree_index += 1;

    return true;
  }

  void IOManager::clear_entry()
  {
    for (auto& p : _product_ptr_v) {
      if (!p) break;
      p->clear();
    }
    if (_set_event_id.valid()) {
      LARCV_DEBUG() << "Set _last_event_id to externally set values:"
                    << " run = " << _set_event_id.run()
                    << " subrun = " << _set_event_id.subrun()
                    << " event = " << _set_event_id.event() << std::endl;
      _last_event_id = _set_event_id;
    }
    else {
      LARCV_DEBUG() << "Set _last_event_id to inherited values:"
                    << " run = " << _event_id.run()
                    << " subrun = " << _event_id.subrun()
                    << " event = " << _event_id.event() << std::endl;
      _last_event_id = _event_id;
    }
    _event_id.clear();
    _set_event_id.clear();
  }

  ProducerID_t IOManager::producer_id(const ProducerName_t& name) const
  {
    LARCV_DEBUG() << "start" << std::endl;

    if (name.second.empty()) {
      LARCV_CRITICAL() << "Empty producer name (invalid)" << std::endl;
      throw larbys();
    }
    if (name.first.empty()) {
      LARCV_CRITICAL() << "Empty producer type (invalid)" << std::endl;
      throw larbys();
    }

    auto iter = _key_list.find(name);
    if (iter == _key_list.end()) {
      return kINVALID_PRODUCER;
    }
    return (*iter).second;
  }

  EventBase* IOManager::get_data(const std::string& type, const std::string& producer)
  {
    LARCV_DEBUG() << "start" << std::endl;

    auto prod_name = ProducerName_t(type, producer);
    auto id = producer_id(prod_name);

    if (id == kINVALID_SIZE) {
      id = register_producer(prod_name);
      if (_io_mode == kREAD) {
        LARCV_NORMAL() << type << " created w/ producer name " << producer << " but won't be stored in file (kREAD mode)" << std::endl;
      } else {
        //for (size_t i = 0; i < _in_tree_index; ++i) _out_tree_v[id]->Fill();
        LARCV_NORMAL() << "Created TTree " << _out_tree_v[id]->GetName() << " (id=" << id << ") w/ " << _in_tree_index << " entries..." << std::endl;
      }
    }
    return get_data(id);
  }

  EventBase* IOManager::get_data(const size_t id)
  {
    __ioman_mtx.lock();
    LARCV_DEBUG() << "start" << std::endl;

    if (id >= _product_ctr) {
      LARCV_ERROR() << "Invalid producer ID requested:" << id << std::endl;
      throw larbys();
    }

    if (_io_mode != kWRITE && _in_tree_index != kINVALID_SIZE &&
        _in_tree_index_v[id] != _in_tree_index &&
        (id >= _read_id_bool.size() || _read_id_bool[id])) {

      if (_in_tree_entries_v[id]) {
        LARCV_DEBUG() << "Reading in TTree " << _in_tree_v[id]->GetName() << " index " << _in_tree_index << std::endl;
        _in_tree_v[id]->GetEntry(_in_tree_index);
        _in_tree_index_v[id] = _in_tree_index;
      }

      auto& ptr = _product_ptr_v[id];
      // retrieve event_id if not yet done
      if (!_event_id.valid()) {
        LARCV_INFO() << "Setting event id (" << ptr->run() << "," << ptr->subrun() << "," << ptr->event() << ")" << std::endl;
        _event_id = (EventBase)(*ptr);
      } else if (ptr->valid() && _event_id != *ptr) {

        if (id >= _read_id_bool.size())
          ptr->set_id(_event_id.run(), _event_id.subrun(), _event_id.event());
        else {
          LARCV_CRITICAL() << "Event alignment error (run,subrun,event) detected: "
                           << "Current (" << _event_id.run() << "," << _event_id.subrun() << "," << _event_id.event() << ") vs. "
                           << "Read-in (" << ptr->run() << "," << ptr->subrun() << "," << ptr->event() << ")" << std::endl;
          throw larbys();
        }
      }
    }
    __ioman_mtx.unlock();
    return _product_ptr_v[id];
  }

  void IOManager::set_id(const size_t run, const size_t subrun, const size_t event) {

    if (_io_mode == kREAD) {
      LARCV_CRITICAL() << "Cannot change event id in kREAD mode" << std::endl;
      throw larbys();
    }

    EventBase tmp;
    tmp._run    = run;
    tmp._subrun = subrun;
    tmp._event  = event;

    LARCV_INFO() << "Request to set event id: " << tmp.event_key() << std::endl;

    if (_set_event_id.valid() && _set_event_id != tmp)
      LARCV_INFO() << "Force setting (run,subrun,event) ID as (" << run << "," << subrun << "," << event << ")" << std::endl;

    _set_event_id = tmp;

  }

  void IOManager::set_id() {
    LARCV_DEBUG() << "start" << std::endl;

    if (_io_mode == kREAD) return;

    if (_set_event_id.valid())

      _event_id = _set_event_id;

    LARCV_INFO() << "Setting event id for output trees: " << _event_id.event_key() << std::endl;

    for (size_t i = 0; i < _product_ptr_v.size(); ++i) {

      auto& p = _product_ptr_v[i];
      if (!p) break;
      LARCV_DEBUG() << "Updating event id for product " << _product_type_v[i] << " by " << p->producer() << std::endl;
      if ( (*p) != _event_id ) {
        if (p->valid()) {
          LARCV_WARNING() << "Override event id for product " << _product_type_v[i]
                          << " by " << p->producer()
                          << " from (" << p->run() << "," << p->subrun() << "," << p->event() << ")"
                          << " to (" << _event_id.run() << "," << _event_id.subrun() << "," << _event_id.event() << ")" << std::endl;
        }
        p->_run = _event_id.run();
        p->_subrun = _event_id.subrun();
        p->_event = _event_id.event();
      }

    }
  }

  void IOManager::finalize()
  {
    LARCV_DEBUG() << "start" << std::endl;

    if (_io_mode != kWRITE) {
      LARCV_INFO() << "Deleting input TChains" << std::endl;
      for (auto& t : _in_tree_v) {if (!t) break; delete t;};
    }

    if (_io_mode != kREAD) {
      _out_file->cd();
      if (_store_id_bool.empty()) {
        for (auto& t : _out_tree_v) {
          if (!t) break;
          LARCV_NORMAL() << "Writing " << t->GetName() << " with " << t->GetEntries() << " entries" << std::endl;
          t->Write();
        }
      } else {
        for (size_t i = 0; i < _store_id_bool.size(); ++i) {
          if (!_store_id_bool[i]) continue;
          auto& t = _out_tree_v[i];
          LARCV_NORMAL() << "Writing " << t->GetName() << " with " << t->GetEntries() << " entries" << std::endl;
          t->Write();
        }
      }
      LARCV_NORMAL() << "Closing output file" << std::endl;
      _out_file->Close();
      _out_file = nullptr;
    }

    LARCV_INFO() << "Deleting data pointers" << std::endl;
    for (auto& p : _product_ptr_v) { delete p; }

    reset();
  }

  void IOManager::reset()
  {
    LARCV_DEBUG() << "start" << std::endl;
    _event_id.clear();
    _set_event_id.clear();
    _in_tree_v.clear();
    _in_tree_v.resize(1000, nullptr);
    _in_tree_entries_v.clear();
    _in_tree_entries_v.resize(1000, 0);
    _in_tree_index_v.clear();
    _out_tree_v.clear();
    _out_tree_v.resize(1000, nullptr);
    _product_ptr_v.clear();
    _product_ptr_v.resize(1000, nullptr);
    _product_type_v.clear();
    _product_type_v.resize(1000, "");
    _product_ctr = 0;
    _in_tree_index = 0;
    _out_tree_index = 0;
    _in_tree_entries = 0;
    _prepared = false;
    _out_file_name = "";
    _in_file_v.clear();
    _in_dir_v.clear();
    _key_list.clear();
    _read_only.clear();
    _store_only.clear();
    _read_id_bool.clear();
    _store_id_bool.clear();
  }

}
#endif
