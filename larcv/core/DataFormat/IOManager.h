/**
 * \file IOManager.h
 *
 * \ingroup core_DataFormat
 *
 * \brief Class def header for a class larcv::IOManager
 *
 * @author drinkingkazu
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef IOMANAGER_H
#define IOMANAGER_H

#include <iostream>
#include <map>
#include <set>
#include <TTree.h>
#include <TChain.h>
#include <TFile.h>
#include "larcv/core/Base/larcv_base.h"
#include "larcv/core/Base/larbys.h"
#include "larcv/core/Base/PSet.h"
#include "EventBase.h"
//#include "ProductMap.h"
namespace larcv {
  /**
    \class IOManager
    \brief LArCV file IO hanlder class: it can read/write LArCV file.
  */
  class IOManager : public larcv::larcv_base {

  public:
    /// Three IO modes: read, write, or read-and-write
    enum IOMode_t { kREAD, kWRITE, kBOTH };

    /// Default constructor
    IOManager(IOMode_t mode = kREAD, std::string name = "IOManager");

    /// Configuration PSet construction so you don't have to call setter functions
    IOManager(const PSet& cfg);

    /// Configuration PSet file construction so you don't have to call setter functions
    IOManager(std::string config_file, std::string name = "IOManager");

    /// Default destructor
    ~IOManager() {}
    /// IO mode accessor
    IOMode_t io_mode() const { return _io_mode;}
    void reset();
    void add_in_file(const std::string filename, const std::string dirname = "");
    void clear_in_file();
    void set_out_file(const std::string name);
    ProducerID_t producer_id(const ProducerName_t& name) const;
    void configure(const PSet& cfg);
    bool initialize();
    bool read_entry(const size_t index, bool force_reload = false);
    bool save_entry();
    void finalize();
    void clear_entry();
    void set_id(const size_t run, const size_t subrun, const size_t event);
    size_t current_entry() const { return _in_tree_index; }

    size_t get_n_entries_out() const
    { return _out_tree_entries;}

    std::string get_file_out_name() const
    { return _out_file_name;}

    size_t get_n_entries() const
    { return (_in_tree_entries ? _in_tree_entries : _out_tree_entries); }

    EventBase* get_data(const std::string& type, const std::string& producer);
    EventBase* get_data(const ProducerID_t id);

    //
    // Some template class getter for auto-cast
    //

    template <class T> T& get_data(const std::string& producer)
    { return *((T*)(this->get_data(product_unique_name<T>(), producer))); }

    template <class T> T& get_data(const ProducerID_t id)
    {
      if (id >= _product_type_v.size()) {
        LARCV_CRITICAL() << "Invalid producer id: " << id << " requested " << std::endl;
        throw larbys();
      }
      auto ptr = this->get_data(id);
      if (product_unique_name<T>() != _product_type_v[id]) {
        LARCV_CRITICAL() << "Unmatched type (in memory type = " << _product_type_v[id]
                         << " while specialization type = " << product_unique_name<T>()
                         << std::endl;
        throw larbys();
      }
      return *((T*)(ptr));
    }

    const EventBase& event_id() const { return ( _set_event_id.valid() ? _set_event_id : _event_id ); }

    const EventBase& last_event_id() const { return _last_event_id; }

    const std::vector<std::string> producer_list(const std::string type) const
    {
      std::vector<std::string> res;
      for (auto const& key_value : _key_list) {
        if (key_value.first.first != type) continue;
        res.push_back(key_value.first.second);
      }
      return res;
    }

    const std::vector<std::string> product_list() const
    {
      std::vector<std::string> res;
      for (auto const& key_value : _key_list) {
        res.push_back(key_value.first.first);
      }
      return res;
    }

    const std::vector<std::string>& file_list() const
    { return _in_file_v; }

  private:
    void   set_id();
    void   prepare_input();
    size_t register_producer(const ProducerName_t& name);

    IOMode_t    _io_mode;
    bool        _prepared;
    TFile*      _out_file;
    size_t      _in_tree_index;
    size_t      _out_tree_index;
    size_t      _in_tree_entries;
    size_t      _out_tree_entries;
    EventBase   _event_id;
    EventBase   _set_event_id;
    EventBase   _last_event_id;
    std::string _out_file_name;
    std::vector<std::string>     _in_file_v;
    std::vector<std::string>     _in_dir_v;
    std::map<larcv::ProducerName_t, larcv::ProducerID_t> _key_list;
    std::vector<TTree*>          _out_tree_v;
    std::vector<TChain*>         _in_tree_v;
    std::vector<size_t>          _in_tree_index_v;
    std::vector<size_t>          _in_tree_entries_v;
    size_t _product_ctr;
    std::vector<larcv::EventBase*> _product_ptr_v;
    std::vector<std::string>       _product_type_v;
    std::map<std::string,std::set<std::string> > _store_only;
    std::map<std::string,std::set<std::string> > _read_only;
    std::vector<bool> _store_id_bool;
    std::vector<bool> _read_id_bool;
  };

}

#endif
/** @} */ // end of doxygen group
