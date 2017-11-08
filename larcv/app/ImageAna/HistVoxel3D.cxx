#ifndef __HISTVOXEL3D_CXX__
#define __HISTVOXEL3D_CXX__

#include "HistVoxel3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static HistVoxel3DProcessFactory __global_HistVoxel3DProcessFactory__;

  HistVoxel3D::HistVoxel3D(const std::string name)
    : ProcessBase(name)
  {}
    
  void HistVoxel3D::configure(const PSet& cfg)
  {
    _tensor3d_producer = cfg.get<std::string>("TargetProducer");
  }

  void HistVoxel3D::initialize()
  {
    _tree = new TTree(_tensor3d_producer.c_str(),"");
    _tree->Branch("x",&_x,"x/F");
    _tree->Branch("y",&_y,"y/F");
    _tree->Branch("z",&_z,"z/F");
    _tree->Branch("v",&_v,"v/F");
  }

  bool HistVoxel3D::process(IOManager& mgr)
  {
    auto const& ev_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>(_tensor3d_producer);
    auto const& meta = ev_tensor3d.meta();
    for(auto const& vox : ev_tensor3d.as_vector()) {
      auto const pos = meta.position(vox.id());
      _x = pos.x;
      _y = pos.y;
      _z = pos.z;
      _v = vox.value();
      _tree->Fill();
    }
    return true;
  }

  void HistVoxel3D::finalize()
  {
    if(has_ana_file()) {
      if(_tree)  _tree->Write();
    }
  }

}
#endif
