
#include "Base/larbys.h"
#include "DataFormat/IOManager.h"
#include "DataFormat/EventROI.h"
#include <fstream>
#include <map>
static larcv::logger& logger()
{ 
  static larcv::logger __main_logger__("main");
  return __main_logger__;
}

static void set_verbosity(larcv::msg::Level_t l)
{ logger().set(l); }

int main(int argc, char** argv){

  set_verbosity(larcv::msg::kNORMAL);

  if(argc < 3)  {
    LARCV_CRITICAL() << "Invalid argument count (needs at least 3)" << std::endl
		     << "  Usage: " << argv[0] << " PRODUCER_NAME FILE1 [FILE2 FILE3 ...]" << std::endl;
    return 1;
  }

  std::string producer(argv[1]);

  larcv::IOManager input_strm(larcv::IOManager::kREAD);
  for(int i=2; i<argc; ++i) input_strm.add_in_file(argv[i]);
  input_strm.initialize();

  std::ofstream fout("event_list.txt");
  size_t entry = 0;
  while(entry < input_strm.get_n_entries()) {

    input_strm.read_entry(entry);

    auto const ev_roi = (larcv::EventROI*)(input_strm.get_data(larcv::kProductROI,producer));

    fout << entry << " " 
	 << ev_roi->run() << " "
	 << ev_roi->subrun() << " "
	 << ev_roi->event() << std::endl;
    ++entry;
  }

  fout.close();

  input_strm.finalize();

  return 0;
}
