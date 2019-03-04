#ifndef __LARBYSIMAGEREANA_CXX__
#define __LARBYSIMAGEREANA_CXX__

#include "LArbysImageReAna.h"
#include "DataFormat/EventPGraph.h"
#include "DataFormat/EventPixel2D.h"
#include "LArOpenCV/ImageCluster/AlgoModule/ReAnaVertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include <array>
#include <cassert>

namespace larcv {
  
  static LArbysImageReAnaProcessFactory __global_LArbysImageReAnaProcessFactory__;
  
  LArbysImageReAna::LArbysImageReAna(const std::string name) :
    LArbysImage(),
    _mgr(nullptr),
    _vertexid(kINVALID_SIZE)
  {}
  
  void LArbysImageReAna::SetIOManager(IOManager* mgr) {
    _mgr = mgr;
    LARCV_INFO() << "Set IOManager pointer as " << _mgr << std::endl;
  }
  
  void LArbysImageReAna::Process() {
    //
    // Get the vertex & particles from pgraph @ pixel2d
    //
    std::string reana_alg_name = "reanavertex";
    LARCV_DEBUG() << "Set cheater algo name: " << reana_alg_name << std::endl;

    auto reana_id = _alg_mgr.GetClusterAlgID(reana_alg_name);
    auto reana_alg = (larocv::ReAnaVertex*)_alg_mgr.GetClusterAlgRW(reana_id);

    if (!reana_alg) throw larbys("Could not find RW reana algo");
    if (!_mgr) throw larbys("No IOManager pointer specified");
    
    const auto pgraph_producer  = _pgraph_prod;
    const auto pixel2d_producer = _pixel2d_prod;
    LARCV_DEBUG() << "Set PGraph producer name:  " << pgraph_producer << std::endl;
    LARCV_DEBUG() << "Set Pixel2D producer name: " << pixel2d_producer << std::endl;
    
    auto ev_pgraph  = (EventPGraph*)  _mgr->get_data(kProductPGraph ,pgraph_producer);
    auto ev_pixel2d = (EventPixel2D*) _mgr->get_data(kProductPixel2D,pixel2d_producer);
    
    if(!ev_pgraph) throw larbys("Could not read given PGraph producer");
    if(!ev_pixel2d) throw larbys("Could not read given Pixel2D producer");

    if(ev_pgraph->PGraphArray().empty()) {
      LARCV_DEBUG() << "No vertex reconstructed" << std::endl;
      _alg_mgr.Process();
      return;
    }
    
    bool this_roi = true;
    LARCV_DEBUG() << "...see " << ev_pgraph->PGraphArray().size() << " event vertex" << std::endl;

    for(size_t pidx=0; pidx < ev_pgraph->PGraphArray().size(); ++pidx) {
      if (_vertexid != kINVALID_SIZE && pidx != _vertexid) continue;
      LARCV_DEBUG() << "...accepted @pidx=" << pidx << std::endl;

      const auto& pgraph = ev_pgraph->PGraphArray().at(pidx);
      const auto& meta_v = pgraph.ParticleArray().front().BB();

      this_roi = true;
      for(size_t plane=0; plane<3; ++plane) {
	const auto& pmeta = meta_v[plane];
	const auto& tmeta = _current_roi.BB(plane);
	if (pmeta != tmeta)  {
	  this_roi = false;
	  break;
	}
      }
      if(!this_roi) continue;
      LARCV_DEBUG() << "...same meta" << std::endl;
      auto x = pgraph.ParticleArray().front().X();
      auto y = pgraph.ParticleArray().front().Y();
      auto z = pgraph.ParticleArray().front().Z();
      LARCV_DEBUG() << "inserting @id=" << pidx 
		    << "("<<x<<","<<y<<","<<z<<")" << std::endl;
      
      larocv::data::Vertex3D vertex;
      vertex.x = x;
      vertex.y = y;
      vertex.z = z;
      
      reana_alg->_vertex_v.emplace_back(std::move(vertex));
      std::array<larocv::GEO2D_ContourArray_t, 3> ctor_vv;
      
      LARCV_DEBUG() << "see " << pgraph.ClusterIndexArray().size() << " particle clusters" << std::endl;
      for (auto parid : pgraph.ClusterIndexArray()) {
	LARCV_DEBUG() << "@ctor="<<parid << "..." << std::endl;
	for(size_t plane=0; plane<3; ++plane) {
	  auto& ctor_v = ctor_vv[plane];
	  const auto& pix2d_v = ev_pixel2d->Pixel2DClusterArray(plane).at(parid);
	  larocv::GEO2D_Contour_t ctor;
	  ctor.resize(pix2d_v.size());
	  for(size_t pxid=0; pxid<pix2d_v.size(); ++pxid) {
	    auto& px = ctor[pxid];
	    px.x = (int) (_adc_img_mgr.img_at(plane).cols - pix2d_v[pxid].Y() - 1);
	    px.y = (int) pix2d_v[pxid].X();
	    assert(px.x > 0 && px.x < _adc_img_mgr.img_at(plane).cols);
	    assert(px.y > 0 && px.y < _adc_img_mgr.img_at(plane).rows);
	  } // end ctor point
	  LARCV_DEBUG() << "...insert sz="<<ctor.size()<<std::endl;
	  ctor_v.emplace_back(std::move(ctor));
	} // end plane
      } // end particle
      reana_alg->_ctor_vvv.emplace_back(std::move(ctor_vv));
    } // end vertex

    _alg_mgr.Process();
  }

  void LArbysImageReAna::SetPGraphProducer(const std::string& pgraph_prod) {
    _pgraph_prod = pgraph_prod;
    LARCV_DEBUG() << "Setting pgraph producer " << _pgraph_prod << std::endl;
  }

  void LArbysImageReAna::SetPixel2DProducer(const std::string& pixel2d_prod) {
    _pixel2d_prod = pixel2d_prod;
    LARCV_DEBUG() << "Setting pixel2d producer " << _pixel2d_prod << std::endl;
  }

  void LArbysImageReAna::SetVertexID(int vertexid) {
    _vertexid = (size_t)vertexid;
    LARCV_DEBUG() << "Setting vertexid producer " << _vertexid << std::endl;
  }
  
}

#endif
