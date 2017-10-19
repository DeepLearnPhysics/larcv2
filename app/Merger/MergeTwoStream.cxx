#ifndef MERGETWOSTREAM_CXX
#define MERGETWOSTREAM_CXX

#include "MergeTwoStream.h"
#include "Base/LArCVBaseUtilFunc.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventROI.h"
#include "DataFormat/EventChStatus.h"

namespace larcv {

  MergeTwoStream::MergeTwoStream() : larcv_base      ( "MergeTwoStream"   )
				   , _in1_driver     ( "InputStream1"     )
				   , _in1_proc       ( nullptr            )
				   , _in1_proc_name  ( ""                 )
				   , _in2_driver     ( "InputStream2"     )
				   , _in2_proc       ( nullptr            )
				   , _in2_proc_name  ( ""                 )
				   , _merge_driver   ( "OutStream"        )
				   , _prepared       (false)
				   , _num_in1        (0)
				   , _num_in2        (0)
				   , _num_processed  (0)
				   , _num_input_max  (0)
				   , _num_output_max (0)
				   , _num_frac       (0)
  {}

  void MergeTwoStream::override_input_file(const std::vector<std::string>& in1_flist,
					   const std::vector<std::string>& in2_flist)
  {
    if(_prepared) {
      LARCV_CRITICAL() << "Cannot re-configure after initialized..." << std::endl;
      throw larbys();
    }
    if(in1_flist.size())
      _in1_driver.override_input_file(in1_flist);
    if(in2_flist.size())
      _in2_driver.override_input_file(in2_flist);
  }

  void MergeTwoStream::override_ana_file(std::string out_ana_fname)
  {
    if(_prepared) {
      LARCV_CRITICAL() << "Cannot re-configure after initialized..." << std::endl;
      throw larbys();
    }
    _merge_driver.override_ana_file(out_ana_fname);
  }

  void MergeTwoStream::override_output_file(std::string out_fname)
  {
    if(_prepared) {
      LARCV_CRITICAL() << "Cannot re-configure after initialized..." << std::endl;
      throw larbys();
    }
    _merge_driver.override_output_file(out_fname);
  }

  void MergeTwoStream::configure(std::string cfg_file)
  {
    if(_prepared) {
      LARCV_CRITICAL() << "Cannot re-configure after initialized..." << std::endl;
      throw larbys();
    }
    auto main_cfg = CreatePSetFromFile(cfg_file);
    auto const& cfg = main_cfg.get_pset(name());
    if(!cfg.contains_pset(_in2_driver.name())) {
      LARCV_CRITICAL() << "InputStream2 parameter set not found..." << std::endl;
      throw larbys();
    }
    if(!cfg.contains_pset(_in1_driver.name())) {
      LARCV_CRITICAL() << "InputStream1 parameter set not found..." << std::endl;
      throw larbys();
    }
    if(!cfg.contains_pset(_merge_driver.name())) {
      LARCV_CRITICAL() << "OutStream parameter set not found..." << std::endl;
      throw larbys();
    }
    _in2_proc_name = cfg.get<std::string>("Input2ImageHolder");
    if(_in2_proc_name.empty()) {
      LARCV_CRITICAL() << "Input2ImageHolder name is empty" << std::endl;
      throw larbys();
    }
    _in1_proc_name = cfg.get<std::string>("Input1ImageHolder");
    if(_in1_proc_name.empty()) {
      LARCV_CRITICAL() << "Input1ImageHolder name is empty" << std::endl;
      throw larbys();
    }
    _merge_proc_name = cfg.get<std::string>("ImageMerger");
    if(_merge_proc_name.empty()) {
      LARCV_CRITICAL() << "ImageMerger name is empty" << std::endl;
      throw larbys();
    }

    LARCV_WARNING() << "Note CosmicDataStream should contain image to-be-overlayed with ROI (Stream2 is the base image)"<<std::endl;
    LARCV_NORMAL() << "Registered Input1: " << _in2_proc_name << std::endl;
    LARCV_NORMAL() << "Registered Input2: " << _in1_proc_name << std::endl;
    LARCV_NORMAL() << "Registered Output: " << _merge_proc_name << std::endl; 

    set_verbosity((msg::Level_t)(cfg.get<unsigned short>("Verbosity",logger().level())));
    _num_output_max = cfg.get<size_t>("MaxOutputEntries");
    _merge_driver.configure(cfg.get_pset(_merge_driver.name()));
    _in2_driver.configure(cfg.get_pset(_in2_driver.name()));
    _in1_driver.configure(cfg.get_pset(_in1_driver.name()));
  }
  
  void MergeTwoStream::initialize()
  {
    _merge_driver.initialize();
    _in2_driver.initialize();
    _in1_driver.initialize();
    // retrieve image holder 1 & 2
    auto const id_cosmic = _in2_driver.process_id(_in2_proc_name);
    _in2_proc = (ImageHolder*)(_in2_driver.process_ptr(id_cosmic));
    auto const id_nu = _in1_driver.process_id(_in1_proc_name);
    _in1_proc = (ImageHolder*)(_in1_driver.process_ptr(id_nu));
    auto const id_merged = _merge_driver.process_id(_merge_proc_name);
    _merge_proc = (ImageMerger*)(_merge_driver.process_ptr(id_merged));
    _merge_proc->InputImageHolder1(_in1_proc);
    _merge_proc->InputImageHolder2(_in2_proc);
    _prepared=true;
    _num_in2 = 0;
    _num_in1 = 0;
    _num_input_max = std::min(_in2_driver.io().get_n_entries(),_in1_driver.io().get_n_entries());
    _num_frac = _num_input_max/10 + 1;
    if(_num_output_max > _num_input_max) {
      LARCV_NORMAL() << "Only " << _num_input_max << " entries available from input. Re-setting output max entry..." << std::endl;
      _num_output_max = _num_input_max;
    }
    LARCV_NORMAL() << "Processing max " << _num_output_max << " entries..." << std::endl;
    _num_processed = 0;
  }
  
  bool MergeTwoStream::process()
  {    
    if(!_prepared) {
      LARCV_CRITICAL() << "Must call initialize() beore process()" << std::endl;
      throw larbys();
    }

    if(_num_processed >= _num_output_max) {
      LARCV_CRITICAL() << "No more output entry to be made!" << std::endl;
      return false;
    }

    while(_num_in2 < _num_input_max) {
      ++_num_in2;
      if(_in2_driver.process_entry()) break;
    }
    while(_num_in1 < _num_input_max) {
      ++_num_in1;
      if(_in1_driver.process_entry()) break;
    }

    if(_num_in2 >= _num_input_max) return false;
    if(_num_in1 >= _num_input_max) return false;

    LARCV_INFO() << "Processing InputStream1 entry " << _num_in2
		 << " ... InputStream2 entry " << _num_in1 << std::endl;

    _merge_driver.process_entry();
    ++_num_processed;
    if(_num_output_max < 10)
      { LARCV_NORMAL() << "Processed " << _num_processed << " entries..." << std::endl; }
    else if( _num_processed && (_num_processed%_num_frac == 0) )
      { LARCV_NORMAL() << "Processed " << 10*((_num_processed/_num_frac)+1) << " % processed..." << std::endl; }

    return true;
  }

  void MergeTwoStream::batch_process()
  {
    while(_num_processed<_num_output_max) if(!process()) break;
    LARCV_NORMAL() << "Finished 100%" << std::endl;
  }
  
  void MergeTwoStream::finalize()
  {
    if(!_prepared) {
      LARCV_CRITICAL() << "Must call initialize() beore process()" << std::endl;
      throw larbys();
    }

    _in1_driver.finalize();
    _in2_driver.finalize();
    _merge_driver.finalize();
  }

}
#endif
