
#include "Base/larcv_logger.h"
static larcv::logger& logger()
{ 
  static larcv::logger __main_logger__("main");
  return __main_logger__;
}

static void set_verbosity(larcv::msg::Level_t l)
{ logger().set(l); }

