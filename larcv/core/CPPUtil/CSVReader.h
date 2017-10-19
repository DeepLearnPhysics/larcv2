#ifndef __LARCV_CSVREADER_H__
#define __LARCV_CSVREADER_H__
#include "larcv/core/Base/larcv_logger.h"
#include "larcv/core/Base/larbys.h"
#include <fstream>
#include <map>
#include <array>
#include <iostream>

namespace larcv {

  // Data holder
  class CSVData {
  public:
    /// internal organization purpose
    enum class ValueType_t { kSTRING, kBOOL, kINT, kFLOAT, kDOUBLE };
  public:
    /// default ctor
    CSVData() { clear(); }
    /// default dtor
    ~CSVData() {}
    /// alternative ctor
    CSVData(const std::string fname, const std::string format)
    { read_file(fname,format); }
    /// alternative ctor
    CSVData(const std::string& fname, const std::vector<larcv::CSVData::ValueType_t>& format)
    { read_file(fname,format); }

    /// Clear
    void clear();
    /// Read in CSV file
    void read_file(const std::string, const std::string);
    /// Read in CSV file
    void read_file(const std::string, const std::vector<larcv::CSVData::ValueType_t>&);

    /// A value-type-wise container getter
    template <class T>
    const std::map<std::string,std::vector<T> >& get_container() const;

    /// A column of a specific value type getter 
    template <class T>
    const std::vector<T>& get(const std::string key) const;

    /// For python
    inline const std::vector<std::string>& get_string(const std::string key) const
    { return this->get<std::string>(key); }
    /// For python
    inline const std::vector<int>& get_int(const std::string key) const
    { return this->get<int>(key); }
    /// For python
    inline const std::vector<bool>& get_bool(const std::string key) const
    { return this->get<bool>(key); }
    /// For python
    inline const std::vector<float>& get_float(const std::string key) const
    { return this->get<float>(key); }
    /// For python
    inline const std::vector<double>& get_double(const std::string key) const
    { return this->get<double>(key); }

  private:
    void read_line(const std::string& line_data,
		   const std::vector<larcv::CSVData::ValueType_t>& value_types,
		   std::vector<void*>& column_ptrs);
		  
    std::map< std::string, std::vector<std::string> > _data_str;
    std::map< std::string, std::vector<bool>        > _data_bool;
    std::map< std::string, std::vector<int>         > _data_int;
    std::map< std::string, std::vector<float>       > _data_float;
    std::map< std::string, std::vector<double>      > _data_double;
  };

  template<> inline const std::map<std::string,std::vector<std::string> >&
  CSVData::get_container<std::string>() const { return _data_str;   }
  
  template<> inline const std::map<std::string,std::vector<bool>        >&
  CSVData::get_container<bool>() const { return _data_bool;  }
  
  template<> inline const std::map<std::string,std::vector<int>         >&
  CSVData::get_container<int>() const { return _data_int;   }
  
  template<> inline const std::map<std::string,std::vector<float>       >&
  CSVData::get_container<float>() const { return _data_float; }
  
  template<> inline const std::map<std::string,std::vector<double>      >&
  CSVData::get_container<double>() const { return _data_double; }

  template <class T>
  inline const std::vector<T>& CSVData::get(const std::string key) const
  {
    auto const& data_m = get_container<T>();
    auto iter = data_m.find(key);
    if(iter == data_m.end()) {
      LARCV_SCRITICAL() << "Key " << key << " does not exist!" << std::endl;
      throw larbys();
    }
    return (*iter).second;
  }

  inline CSVData read_csv(const std::string fname, const std::string format)
  { return CSVData(fname,format); }
  
}
#endif
