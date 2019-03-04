
#include "Base/larbys.h"
#include "DataFormat/IOManager.h"
#include "DataFormat/EventImage2D.h"
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

  larcv::IOManager output_strm(larcv::IOManager::kWRITE);
  output_strm.set_out_file("mean.root");
  output_strm.initialize();
  auto output_image = (larcv::EventImage2D*)(output_strm.get_data(larcv::kProductImage2D,"mean"));
  std::vector<larcv::Image2D> image_v;

  size_t nentries = input_strm.get_n_entries();
  size_t entries_fraction = nentries/10;
  size_t entry=0;
  double image_count=0;
  while(entry < nentries) {
    input_strm.read_entry(entry);
    ++entry;

    if(nentries<10)

      { LARCV_NORMAL() << "Processing entry " << entry << std::endl; }
    
    else if(entry%entries_fraction==0)

      { LARCV_NORMAL() << "Processed " << entry/entries_fraction * 10 << " %" << std::endl; }

    auto event_image = (larcv::EventImage2D*)(input_strm.get_data(larcv::kProductImage2D,producer));

    if(image_v.empty()) {

      event_image->Move(image_v);

    }else{

      if(image_v.size() != event_image->Image2DArray().size())

	throw ::larcv::larbys("Incompatible length of Image2DArray found!");

      auto const& in_image_v = event_image->Image2DArray();

      for(size_t i=0; i<in_image_v.size(); ++i) {

	image_v[i] += in_image_v[i].as_vector();

      }
    }
    ++image_count;
  }

  for(auto& img : image_v)

    img /= image_count;

  output_image->Emplace(std::move(image_v));

  output_strm.set_id(0,0,0);
  output_strm.save_entry();
  output_strm.finalize();
  input_strm.finalize();

  return 0;
}
