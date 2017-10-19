#ifndef __LARCVLOGGER_CXX__
#define __LARCVLOGGER_CXX__

#include "larcv_logger.h"
#include <mutex>
std::mutex __logger_mtx;
namespace larcv {

  logger *logger::_shared_logger = nullptr;
  
  std::map<std::string,logger> *logger::_logger_m = nullptr;

  msg::Level_t logger::_level_default = msg::kNORMAL;
  
  std::ostream& logger::send(const msg::Level_t level) const
  {
    __logger_mtx.lock();
    (*_ostrm)  << msg::kStringPrefix[level].c_str()
	       << "\033[0m ";
    __logger_mtx.unlock();
    return (*_ostrm);
  }
  
  std::ostream& logger::send(const msg::Level_t level,
			     const std::string& function ) const
  {
    auto& strm(send(level));
    strm << "\033[94m<" << _name << "::" << function.c_str() << ">\033[00m ";
    return strm;
  }
  
  std::ostream& logger::send(const msg::Level_t level,
			     const std::string& function,
			     const unsigned int line_num ) const
  {
    auto& strm(send(level));
    strm << "\033[94m<" << _name << "::" << function.c_str() << "::L" << line_num << ">\033[00m ";
    return strm;
  }
  
  std::ostream& logger::send(const msg::Level_t level,
			     const std::string& function,
			     const unsigned int line_num,
			     const std::string& file_name) const
  {
    auto& strm(send(level,function));
    // FIXME temporary operation to fetch file name from full path
    strm << file_name.substr(file_name.rfind("/")+1,file_name.size()).c_str() << "::L" << line_num << " ";
    return strm;
  }
  
  logger& logger::get_shared()
  {
    __logger_mtx.lock();
    if(!_shared_logger) _shared_logger = new logger("GLOBAL");
    __logger_mtx.unlock();
    return *_shared_logger;

  }
}
#endif
