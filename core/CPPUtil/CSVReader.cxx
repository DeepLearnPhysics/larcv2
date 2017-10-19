#ifndef __LARCV_CSVREADER_CXX__
#define __LARCV_CSVREADER_CXX__
#include "CSVReader.h"

namespace larcv {

  void CSVData::clear()
  { _data_str.clear(); _data_bool.clear(); _data_int.clear(); _data_float.clear(); _data_double.clear(); }
  
  void CSVData::read_file(const std::string fname, const std::string format)
  {
    std::vector<larcv::CSVData::ValueType_t> value_types;
    for(size_t idx=0; idx<format.length(); ++idx) {
      auto v = format.substr(idx,1);
      CSVData::ValueType_t vt(CSVData::ValueType_t::kSTRING);
      if      (v == "S") vt = CSVData::ValueType_t::kSTRING;
      else if (v == "I") vt = CSVData::ValueType_t::kINT;
      else if (v == "B") vt = CSVData::ValueType_t::kBOOL;
      else if (v == "F") vt = CSVData::ValueType_t::kFLOAT;
      else if (v == "D") vt = CSVData::ValueType_t::kDOUBLE;
      else {
	LARCV_SCRITICAL() << "CSV value type \"" << v << "\" not supported!" << std::endl;
	throw larbys();
      }
      value_types.push_back(vt);
    }
    read_file(fname,value_types);
  }

  void CSVData::read_file(const std::string fname, const std::vector<larcv::CSVData::ValueType_t>& value_types)
  {
    clear();
    std::ifstream fstrm(fname.c_str());
    std::string   line_data;
    std::getline(fstrm, line_data);

    size_t last_pos=0;
    size_t next_pos=0;
    std::vector<std::string> column_names;
    std::vector<void*> column_ptrs;
    for(size_t idx=0; idx<value_types.size(); ++idx) {
      auto const& value_type = value_types[idx];
      void* column_ptr;
      if(idx<1)
	next_pos=line_data.find(",");
      else
	next_pos=line_data.find(",",last_pos);
      auto column_name = line_data.substr(last_pos, next_pos - last_pos);

      switch(value_type) {
      case CSVData::ValueType_t::kSTRING:
	if(_data_str.find(column_name) != _data_str.end()) {
	  LARCV_SCRITICAL() << "CSV column type string w/ name " << column_name << " is duplicate..." << std::endl;
	  throw larbys();
	}else
	  column_ptr = (void*)(&(_data_str[column_name]));
	break;
      case CSVData::ValueType_t::kINT:
	if(_data_int.find(column_name) != _data_int.end()) {
	  LARCV_SCRITICAL() << "CSV column type int w/ name " << column_name << " is duplicate..." << std::endl;
	  throw larbys();
	}else
	  column_ptr = (void*)(&(_data_int[column_name]));
	break;
      case CSVData::ValueType_t::kBOOL:
	if(_data_bool.find(column_name) != _data_bool.end()) {
	  LARCV_SCRITICAL() << "CSV column type bool w/ name " << column_name << " is duplicate..." << std::endl;
	  throw larbys();
	}else
	  column_ptr = (void*)(&(_data_bool[column_name]));
	break;	
      case CSVData::ValueType_t::kFLOAT:
	if(_data_float.find(column_name) != _data_float.end()) {
	  LARCV_SCRITICAL() << "CSV column type float w/ name " << column_name << " is duplicate..." << std::endl;
	  throw larbys();
	}else
	  column_ptr = (void*)(&(_data_float[column_name]));
	break;
      case CSVData::ValueType_t::kDOUBLE:
	if(_data_double.find(column_name) != _data_double.end()) {
	  LARCV_SCRITICAL() << "CSV column type double w/ name " << column_name << " is duplicate..." << std::endl;
	  throw larbys();
	}else
	  column_ptr = (void*)(&(_data_double[column_name]));
	break;	
      }
      column_names.push_back(column_name);
      column_ptrs.push_back(column_ptr);
      last_pos = next_pos+1;
    }

    while(std::getline(fstrm, line_data)) {

      if(line_data.empty()) continue;

      this->read_line(line_data,value_types,column_ptrs);
    }
    
  }
    
  void CSVData::read_line(const std::string& line_data,
			  const std::vector<larcv::CSVData::ValueType_t>& value_types,
			  std::vector<void*>& column_ptrs)
  {

    size_t last_pos=0;
    size_t next_pos=0;
    for(size_t idx=0; idx<value_types.size(); ++idx) {
      auto const& value_type = value_types[idx];
      auto& column_ptr = column_ptrs[idx];
      if(idx<1)
	next_pos=line_data.find(",");
      else
	next_pos=line_data.find(",",last_pos);
      auto value = line_data.substr(last_pos, next_pos - last_pos);
      
      switch(value_type) {
      case CSVData::ValueType_t::kSTRING:
	((std::vector<std::string>*)column_ptr)->push_back(value);
	break;
      case CSVData::ValueType_t::kINT:
	((std::vector<int>*)column_ptr)->push_back(std::atoi(value.c_str()));
	break;
      case CSVData::ValueType_t::kBOOL:
	if(value=="1" || value=="true") ((std::vector<bool>*)column_ptr)->push_back(true);
	else if(value=="0" || value=="false") ((std::vector<bool>*)column_ptr)->push_back(false);
	else {
	  LARCV_SCRITICAL() << "Boolean value neither of [0,1,false,true] ..." << std::endl;
	  throw larbys();
	}
	break;
      case CSVData::ValueType_t::kFLOAT:
	((std::vector<float>*)column_ptr)->push_back(std::atof(value.c_str()));
	break;
      case CSVData::ValueType_t::kDOUBLE:
	((std::vector<double>*)column_ptr)->push_back(std::atof(value.c_str()));
	break;	
      }
      last_pos = next_pos+1;
    }
  }
}

#endif
