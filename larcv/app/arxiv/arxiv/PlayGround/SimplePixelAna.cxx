#ifndef __SIMPLEPIXELANA_CXX__
#define __SIMPLEPIXELANA_CXX__

#include "SimplePixelAna.h"
#include "DataFormat/EventImage2D.h"
namespace larcv {

  static SimplePixelAnaProcessFactory __global_SimplePixelAnaProcessFactory__;

  SimplePixelAna::SimplePixelAna(const std::string name)
    : ProcessBase(name)
  {}
    
  void SimplePixelAna::configure(const PSet& cfg)
  {
    // Get Image2D key string 
    _image_name = cfg.get<std::string>("ImageName");
    // Get pixel counting (analysis) threshold value
    _pixel_count_threshold = cfg.get<float>("PixelCountThreshold");
  }

  void SimplePixelAna::initialize()
  {
    // Instantiate & prepare TTree
    _tree = new TTree("tree","Tree for simple analysis");
    // Create variable holder in TTree
    _tree->Branch( "max_pixel",   &_max_pixel,   "max_pixel/F"   );
    _tree->Branch( "pixel_sum",   &_pixel_sum,   "pixel_sum/F"   );
    _tree->Branch( "pixel_count", &_pixel_count, "pixel_count/I" );
    _tree->Branch( "image_index", &_image_index, "image_index/s" );
    // Initialize variable contents
    _max_pixel   = 0.;
    _pixel_sum   = 0.;
    _pixel_count = 0;
    _image_index = 0;
  }

  bool SimplePixelAna::process(IOManager& mgr)
  {
    // get EventImage2D (image2d collection)
    auto my_event_image2d = (EventImage2D*)(mgr.get_data(kProductImage2D,_image_name));
    //LARCV_NORMAL() << my_event_image2d << std::endl;

    // get a direct reference to std::vector<larcv::Image2D> (i.e. array of Image2D)
    auto const& img2d_v = my_event_image2d->Image2DArray();

    // Loop over each Image2D and fill analysis variable
    for(size_t index=0; index < img2d_v.size(); ++index) {

      // Initialize analysis variables
      _image_index = index;
      _pixel_sum   = _max_pixel = 0.;
      _pixel_count = 0;
      
      // Access Image2D
      auto const& img2d = img2d_v[index];

      // get 1D array reference to call C++ function to find max 
      auto const& pixel_array = img2d.as_vector(); // this returns std::vector<float>

      // Method 1: using smart way which we hate
      //float _max_pixel = *(std::max_element(pixel_array.begin(),pixel_array.end()));

      // Method 2: using our way
      for(auto const& v : pixel_array) {

	if(v > _max_pixel) _max_pixel = v;

	if(v > _pixel_count_threshold) _pixel_count++;
      }
      
      LARCV_DEBUG() << "pixel max value: " << _max_pixel << std::endl;
      _tree->Fill();
    }
    return true;
  }

  void SimplePixelAna::finalize()
  {
    // If an analysis output file is configured to exist, write TTree into an output
    if(has_ana_file()) _tree->Write();
  }

}
#endif
