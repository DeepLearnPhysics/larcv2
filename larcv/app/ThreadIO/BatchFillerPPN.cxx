#ifndef __BatchFillerPPN_CXX__
#define __BatchFillerPPN_CXX__

#include "BatchFillerPPN.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
#include <random>

namespace larcv {

  static BatchFillerPPNProcessFactory __global_BatchFillerPPNProcessFactory__;

  BatchFillerPPN::BatchFillerPPN(const std::string name)
    : BatchFillerTemplate<float>(name)
  {}

  void BatchFillerPPN::configure(const PSet& cfg)
  {
    bool error = false;
    _min_voxel_count = cfg.get<int>("MinVoxelCount", 5);
		_min_energy_deposit = cfg.get<double>("MinEnergyDeposit", 0.05);
    _part_producer   = cfg.get<std::string>("ParticleProducer");
    _buffer_size     = cfg.get<size_t>("BufferSize",100);
    auto const shape_type = cfg.get<std::string>("ShapeType");
    if      (shape_type == "shower") _shape_type = kShower;
    else if (shape_type == "track" ) _shape_type = kTrack;
    else {
      LARCV_CRITICAL() << "Invalid ShapeType (" << shape_type << ") ... [shower,track]" << std::endl;
      error = true;
    }
    auto const point_type = cfg.get<std::string>("PointType");
    _point_type = kPoint3D;
    if      (point_type == "xy") _point_type = kPointXY;
    else if (point_type == "yz") _point_type = kPointYZ;
    else if (point_type == "zx") _point_type = kPointZX;
    else if (point_type == "3d") _point_type = kPoint3D;
    else {
      LARCV_CRITICAL() << "Invalid PointType (" << point_type << ") ... [xy,yz,zx,3d]" << std::endl;
      error = true;
    }

    _tensor_producer = cfg.get<std::string>("Tensor3DProducer","");
    if(_tensor_producer.empty()) {
      _tensor_producer = cfg.get<std::string>("ImageProducer");
      _image_channel   = cfg.get<size_t>("ImageChannel");
      // make sure point is not 3d
      if(_point_type == kPoint3D) {
	LARCV_CRITICAL() << "PointType == \"3d\" but Tensor3DProducer not given..." << std::endl;
	error = true;
      }
    }else if(_point_type != kPoint3D) {
      LARCV_CRITICAL() << "PointType != \"3d\" but Tensor3DProducer given..." << std::endl;
      error = true;
    }
    if(error) throw larbys();
  }

  void BatchFillerPPN::initialize()
  {}

  void BatchFillerPPN::_batch_begin_()
  {
    if (!batch_data().dim().empty() && (int)(batch_size()) != batch_data().dim().front()) {
      auto dim = batch_data().dim();
      dim[0] = batch_size();
      this->set_dim(dim);
    }
  }

  void BatchFillerPPN::_batch_end_()
  {
    if (logger().level() <= msg::kINFO) {

      int data_ctr = 0;
      for(auto const& v : batch_data().data()) {
      	if(v<0) break;
      	++data_ctr;
      }

      size_t pt_ctr = 0;
      if(_point_type == kPoint3D) pt_ctr = data_ctr / 3;
      else pt_ctr = data_ctr / 2;

      LARCV_INFO() << "Data length: " << data_ctr
		   << " ... " << pt_ctr << (_point_type == kPoint3D ? " 3D points" : " 2D points") << std::endl;

      if(logger().level() <= msg::kDEBUG) {
      	std::stringstream ss;
      	size_t dim = (_point_type == kPoint3D ? 3 : 2);
      	ss << "Dumpint coordinates..." << std::endl << "    Buffer 0 ";
      	for(size_t i=0; i<batch_data().data().size(); ++i) {
      	  ss << batch_data().data()[i] << "    ";
      	  if((i+1) % dim == 0) {
      	    ss << std::endl;
      	    if((i+1) < batch_data().data().size())
      	      ss << "    Buffer " << (i+1) / dim << " ";
      	  }
      	}
      	LARCV_DEBUG() << ss.str() << std::endl;
      }

    }
  }

  void BatchFillerPPN::finalize()
  {}

  bool BatchFillerPPN::process(IOManager & mgr)
  {
    auto const& event_part  = mgr.get_data<larcv::EventParticle>(_part_producer);
    const size_t pt_dim = (_point_type == kPoint3D ? 3 : 2);
    if (batch_data().dim().empty()) {
      std::vector<int> dim(3);
      dim[0] = batch_size();
      dim[1] = _buffer_size;
      dim[2] = pt_dim;
      LARCV_INFO() << "Setting dimension (" << dim[0] << "," << dim[1] << "," << dim[2] << ")" << std::endl;
      set_dim(dim);
    }

    // fill
    larcv::ImageMeta meta;
    larcv::Voxel3DMeta meta3d;

    if(_point_type == kPoint3D){
      LARCV_INFO() << "Retrieving SparseTensor3D " << _tensor_producer << std::endl;
      meta3d = mgr.get_data<larcv::EventSparseTensor3D>(_tensor_producer).meta();
    }else{
      LARCV_INFO() << "Retrieving Image2D " << _tensor_producer << std::endl;
      meta = mgr.get_data<larcv::EventImage2D>(_tensor_producer).as_vector().at(_image_channel).meta();
    }
    auto const& part_v = event_part.as_vector();
    LARCV_DEBUG() << "Resizing _entry_data " << batch_data().entry_data_size() << std::endl;
    _entry_data.resize(batch_data().entry_data_size());
    LARCV_DEBUG() << "Resetting _entry_data " << std::endl;
    for(auto& v : _entry_data) v = -1.;

    size_t data_index = 0;
    LARCV_INFO() << "Processing " << part_v.size() << " particles" << std::endl;
    for(auto const& part : part_v) {

      auto pdg_code = part.pdg_code();
      auto track_id = part.track_id();
      auto energy   = part.energy_deposit();

      if(part.energy_deposit()<_min_energy_deposit || part.num_voxels() < _min_voxel_count) continue;

      if(_shape_type == kTrack  && (pdg_code == 11 || pdg_code == 22 || pdg_code == -11) ) continue;

      if(_shape_type == kShower && (pdg_code != 11 && pdg_code != 22 && pdg_code != -11) ) continue;

      if(_shape_type == kShower && ((_point_type == kPoint3D && !meta3d.contains(part.first_step().as_point3d())) || (_point_type != kPoint3D && !meta.contains(part.first_step().as_point2d(_point_type))))) continue;

      if(pdg_code > 1000000000) {
      	LARCV_INFO() << "Skipping nucleus TrackID " << track_id << " PDG " << pdg_code << " Energy " << energy << std::endl;
      	continue;
      }

      if(_shape_type == kShower && part.parent_pdg_code() == 13 && part.creation_process() == "muIoni") {
      	LARCV_INFO() << "Skipping delta ray TrackID " << track_id << " Energy " << energy << std::endl;
      	continue;
      }

      // Register start point
      double x, y, z;
      larcv::Vertex start_point;
      start_point = part.first_step();
      start_point.as_point(_point_type, &x, &y, &z);

      if (_point_type == kPoint3D) {
        x = (x - meta3d.min_x()) / meta3d.size_voxel_x();
        y = (y - meta3d.min_y()) / meta3d.size_voxel_y();
        z = (z - meta3d.min_z()) / meta3d.size_voxel_z();
        _entry_data.at(data_index) = x; ++data_index;
        _entry_data.at(data_index) = y; ++data_index;
        _entry_data.at(data_index) = z; ++data_index;
      }
      else {
        x = (x - meta.min_x()) / meta.pixel_width();
        y = (y - meta.min_y()) / meta.pixel_height();
        _entry_data.at(data_index) = x; ++data_index;
        _entry_data.at(data_index) = y; ++data_index;
      }

      if(_point_type == kPoint3D) {
	       LARCV_INFO() << "TrackID " << track_id << " PDG " << pdg_code << " Energy " << energy << " start (" << x << "," << y << "," << z << ")" << std::endl;
      }else{
	       LARCV_INFO() << "TrackID " << track_id << " PDG " << pdg_code << " Energy " << energy << " start (" << x << "," << y << ")" << std::endl;
      }

      if(_shape_type==kShower) continue;

      // for track, add end point
      larcv::Vertex end_point;
      end_point = part.last_step();
      end_point.as_point(_point_type, &x, &y, &z);

      if (_point_type == kPoint3D) {
        x = (x - meta3d.min_x()) / meta3d.size_voxel_x();
      	y = (y - meta3d.min_y()) / meta3d.size_voxel_y();
      	z = (z - meta3d.min_z()) / meta3d.size_voxel_z();
      	_entry_data.at(data_index) = x; ++data_index;
      	_entry_data.at(data_index) = y; ++data_index;
      	_entry_data.at(data_index) = z; ++data_index;
      }
      else {
        x = (x - meta.min_x()) / meta.pixel_width();
      	y = (y - meta.min_y()) / meta.pixel_height();
      	_entry_data.at(data_index) = x; ++data_index;
      	_entry_data.at(data_index) = y; ++data_index;
      }

      if(_point_type == kPoint3D) {
	       LARCV_INFO() << "TrackID " << track_id << " PDG " << pdg_code << " Energy " << energy << " end (" << x << "," << y << "," << z << ")" << std::endl;
      }else{
	       LARCV_INFO() << "TrackID " << track_id << " PDG " << pdg_code << " Energy " << energy << " end (" << x << "," << y << ")" << std::endl;
      }
    }

    set_entry_data(_entry_data);
    return true;
  }

}
#endif
