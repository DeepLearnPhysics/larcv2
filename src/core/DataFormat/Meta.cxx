#ifndef LARCV_META_CXX
#define LARCV_META_CXX

#include "Meta.h"
#include <iostream>

namespace larcv {

  //**********************************************************  
  void Meta::clear_data()
  //**********************************************************
  {
    _d_map.clear();
    _i_map.clear();
    _s_map.clear();
    _darray_map.clear();
    _iarray_map.clear();
    _sarray_map.clear();
  }
  
  //**********************************************************
  void Meta::dump() const
  //**********************************************************
  {
    
    std::cout<<"\n  Start contents dump..."<<std::endl;
    
    if(_d_map.size()) {
      std::cout << "  Contents of double variables shown below.\n"
		<< "    Key ... Value\n";
      for(std::map<std::string,double>::const_iterator iter(_d_map.begin());
	  iter!=_d_map.end();
	  ++iter)
	std::cout << "      " << (*iter).first.c_str() << " ... " <<  (*iter).second;
      std::cout<<std::endl<<std::endl;
    }
    
    if(_i_map.size()){
      std::cout << "  Contents of int variables shown below.\n"
		<< "    Key ... Value\n";
      for(std::map<std::string,int>::const_iterator iter(_i_map.begin());
	  iter!=_i_map.end();
	  ++iter)
	std::cout << "      " << (*iter).first.c_str() << " ... " << (*iter).second;
      std::cout<<std::endl<<std::endl;
    }
    
    if(_s_map.size()){
      std::cout << "  Contents of std::string variables shown below.\n"
		<< "    Key ... Value\n";
      for(std::map<std::string,std::string>::const_iterator iter(_s_map.begin());
	  iter!=_s_map.end();
	  ++iter)
	std::cout << "      "  << (*iter).first.c_str() << " ... " << (*iter).second.c_str();
      std::cout<<std::endl<<std::endl;
    }
    
    if(_darray_map.size()){
      std::cout << "  Contents of double array shown below.";
      for(std::map<std::string,std::vector<double> >::const_iterator iter(_darray_map.begin());
	  iter!=_darray_map.end();
	  ++iter){
	std::cout << "\n    Key=" << (*iter).first.c_str() << std::endl;
	int ctr=1;
	std::cout << "      ";
	for(std::vector<double>::const_iterator cont_iter((*iter).second.begin());
	    cont_iter!=(*iter).second.end();
	    ++cont_iter){
	  std::cout << (*cont_iter) << " ";
	  ctr++;
	  if(ctr%8==0)
	    std::cout << "\n      ";
	}
      }
      std::cout<<std::endl<<std::endl;
    }
    
    if(_iarray_map.size()){
      std::cout << "  Contents of int array shown below.";
      for(std::map<std::string,std::vector<int> >::const_iterator iter(_iarray_map.begin());
	  iter!=_iarray_map.end();
	  ++iter){
	std::cout << "\n    Key=" << (*iter).first.c_str() << std::endl;
	int ctr=1;
	std::cout << "      ";
	for(std::vector<int>::const_iterator cont_iter((*iter).second.begin());
	    cont_iter!=(*iter).second.end();
	    ++cont_iter){
	  std::cout << (*cont_iter) << " ";
	  ctr++;
	  if(ctr%8==0)
	    std::cout << "\n      ";
	}
      }
      std::cout<<std::endl<<std::endl;
    }
    
    if(_sarray_map.size()){
      std::cout << "  Contents of std::string array shown below.";
      for(std::map<std::string,std::vector<std::string> >::const_iterator iter(_sarray_map.begin());
	  iter!=_sarray_map.end();
	  ++iter){
	std::cout << "\n    Key=" << (*iter).first.c_str() << std::endl;
	int ctr=1;
	std::cout << "      ";
	for(std::vector<std::string>::const_iterator cont_iter((*iter).second.begin());
	    cont_iter!=(*iter).second.end();
	    ++cont_iter){
	  std::cout<<(*cont_iter).c_str()<<" ";
	  ctr++;
	  if(ctr%8==0)
	    std::cout << "\n      ";
	}
      }
      std::cout<<std::endl<<std::endl;
    }
    
    std::cout<<"  End of dump..."<<std::endl;
  }

  //**********************************************************
  std::vector<double>& Meta::writeable_darray(const std::string& key)
  //**********************************************************
  {
    auto iter = _darray_map.find(key);
    if(iter != _darray_map.end()) return iter->second;
    return (_darray_map.insert(std::make_pair(key,std::vector<double>())).first)->second;
  }
  
  //*******************************************************
  std::vector<int>& Meta::writeable_iarray(const std::string& key)
  //*******************************************************
  {
    auto iter = _iarray_map.find(key);
    if(iter != _iarray_map.end()) return iter->second;
    return (_iarray_map.insert(std::make_pair(key,std::vector<int>())).first)->second;
  }
  
  //***************************************************************
  std::vector<std::string>& Meta::writeable_sarray(const std::string& key)
  //***************************************************************
  {
    auto iter = _sarray_map.find(key);
    if(iter != _sarray_map.end()) return iter->second;
    return (_sarray_map.insert(std::make_pair(key,std::vector<std::string>())).first)->second;
  }
  
  //**********************************************************
  double Meta::get_double(const std::string& key) const 
  //**********************************************************
  {  
    auto item = _d_map.find(key);
    if(item==_d_map.end()){
      std::cerr << "\033[93m<<" << __FUNCTION__ << ">>\033[00m"
	        << "Key " << key.c_str() << " does not exist!" << std::endl;
      throw std::exception();
    }
    return (*item).second;
  }

  //**********************************************************
  int Meta::get_int(const std::string& key) const 
  //**********************************************************
  {
    auto item = _i_map.find(key);
    if(item==_i_map.end()){
      std::cerr << "\033[93m<<" << __FUNCTION__ << ">>\033[00m"
	        << "Key " << key.c_str() << " does not exist!" << std::endl;
      throw std::exception();
    }
    return (*item).second;
  }

  //**********************************************************  
  const std::string& Meta::get_string(const std::string& key) const 
  //**********************************************************
  {
    auto item = _s_map.find(key);
    if(item==_s_map.end()){
      std::cerr << "\033[93m<<" << __FUNCTION__ << ">>\033[00m"
	        << "Key " << key.c_str() << " does not exist!" << std::endl;
      throw std::exception();
    }
    return (*item).second;
  }
  
  //****************************************************************
  const std::vector<double>& Meta::get_darray(const std::string& key) const
  //****************************************************************
  {
    auto item = _darray_map.find(key);
    if(item==_darray_map.end()){
      std::cerr << "\033[93m<<" << __FUNCTION__ << ">>\033[00m"
	        << "Key " << key.c_str() << " does not exist!" << std::endl;
      throw std::exception();
    }
    return (*item).second;
  }

  //*************************************************************
  const std::vector<int>& Meta::get_iarray(const std::string& key) const
  //*************************************************************
  {
    auto item = _iarray_map.find(key);
    if(item==_iarray_map.end()){
      std::cerr << "\033[93m<<" << __FUNCTION__ << ">>\033[00m"
	        << "Key " << key.c_str() << " does not exist!" << std::endl;
      throw std::exception();
    }
    return (*item).second;
  }

  //*********************************************************************
  const std::vector<std::string>& Meta::get_sarray(const std::string& key) const 
  //*********************************************************************
  {
    auto item = _sarray_map.find(key);
    if(item==_sarray_map.end()){
      std::cerr << "\033[93m<<" << __FUNCTION__ << ">>\033[00m"
	        << "Key " << key.c_str() << " does not exist!" << std::endl;
      throw std::exception();
    }
    return (*item).second;
  }

}
  
#endif
