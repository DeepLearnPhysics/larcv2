#ifndef __VERTEXANA_CXX__
#define __VERTEXANA_CXX__

#include "VertexAna.h"
#include "DataFormat/EventROI.h"
#include "DataFormat/EventPGraph.h"
#include "DataFormat/EventPixel2D.h"
#include "DataFormat/EventImage2D.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/LArProperties.h"
#include "LArbysUtils.h"

namespace larcv {

  static VertexAnaProcessFactory __global_VertexAnaProcessFactory__;

  VertexAna::VertexAna(const std::string name)
    : ProcessBase(name)
  {}

  void VertexAna::configure(const PSet& cfg)
  {
    _img2d_prod         = cfg.get<std::string>("Image2DProducer");
    _pgraph_prod        = cfg.get<std::string>("PGraphProducer");
    _pcluster_ctor_prod = cfg.get<std::string>("PxContourProducer");
    _pcluster_img_prod  = cfg.get<std::string>("PxImageProducer");
    _truth_roi_prod     = cfg.get<std::string>("TrueROIProducer");
    _reco_roi_prod      = cfg.get<std::string>("RecoROIProducer");
    _first_roi          = cfg.get<bool>("FirstROI",false);
    _use_scedr          = cfg.get<bool>("UseSCEDR",true);


    LARCV_DEBUG() << "Image2DProducer:  " << _img2d_prod << std::endl;
    LARCV_DEBUG() << "PGraphProducer:   " << _pgraph_prod << std::endl;
    LARCV_DEBUG() << "PxContouProducer: " << _pcluster_ctor_prod << std::endl;
    LARCV_DEBUG() << "PxImageProducer:  " << _pcluster_img_prod << std::endl;
    LARCV_DEBUG() << "TrueROIProducer:  " << _truth_roi_prod << std::endl;
    LARCV_DEBUG() << "RecoROIProducer:  " << _reco_roi_prod << std::endl;

  }

  void VertexAna::initialize()
  {

    _event_tree = new TTree("EventVertexTree","");
    _event_tree->Branch("run",&_run,"run/I");
    _event_tree->Branch("subrun",&_subrun,"subrun/I");
    _event_tree->Branch("event",&_event,"event/I");
    _event_tree->Branch("entry",&_entry,"entry/I");

    _event_tree->Branch("tx",&_tx,"tx/D");
    _event_tree->Branch("ty",&_ty,"ty/D");
    _event_tree->Branch("tz",&_tz,"tz/D");
    _event_tree->Branch("te",&_te,"te/D");
    _event_tree->Branch("tt",&_tt,"tt/D");

    _event_tree->Branch("scex",&_scex,"scex/D");
    _event_tree->Branch("scey",&_scey,"scey/D");
    _event_tree->Branch("scez",&_scez,"scez/D");

    _event_tree->Branch("nprotons",&_nprotons, "nprotons/I");
    _event_tree->Branch("nothers", &_nothers,  "nothers/I");
    
    _event_tree->Branch("good_croi0",&_good_croi0,"good_croi0/I");
    _event_tree->Branch("good_croi1",&_good_croi1,"good_croi1/I");
    _event_tree->Branch("good_croi2",&_good_croi2,"good_croi2/I");
    _event_tree->Branch("good_croi_ctr",&_good_croi_ctr,"good_croi_ctr/I");

    _event_tree->Branch("num_croi",&_num_croi,"num_croi/I");
    _event_tree->Branch("num_vertex",&_num_vertex,"num_vertex/I");
    _event_tree->Branch("num_croi_with_vertex",&_num_croi_with_vertex,"num_croi_with_vertex/I");
    _event_tree->Branch("min_vtx_dist",&_min_vtx_dist,"min_vtx_dist/D");
    _event_tree->Branch("nearest_wire_err",&_nearest_wire_err,"nearest_wire_err/I");

    _tree = new TTree("VertexTree","");

    _tree->Branch("run",&_run,"run/I");
    _tree->Branch("subrun",&_subrun,"subrun/I");
    _tree->Branch("event",&_event,"event/I");
    _tree->Branch("entry",&_entry,"entry/I");
    _tree->Branch("roid",&_roid,"roid/I");
    _tree->Branch("vtxid",&_vtxid,"vtxid/I");

    _tree->Branch("tx",&_tx,"tx/D");
    _tree->Branch("ty",&_ty,"ty/D");
    _tree->Branch("tz",&_tz,"tz/D");
    _tree->Branch("te",&_te,"te/D");
    _tree->Branch("tt",&_tt,"tt/D");

    _tree->Branch("scex",&_scex,"scex/D");
    _tree->Branch("scey",&_scey,"scey/D");
    _tree->Branch("scez",&_scez,"scez/D");

    _tree->Branch("x",&_x,"x/D");
    _tree->Branch("y",&_y,"y/D");
    _tree->Branch("z",&_z,"z/D");
    _tree->Branch("dr",&_dr,"dr/D");
    _tree->Branch("scedr",&_scedr,"scedr/D");
    _tree->Branch("npar",&_npar,"npar/I");
    
    _tree->Branch("nearest_wire_err",&_nearest_wire_err,"nearest_wire_err/I");
    _tree->Branch("in_fiducial",&_in_fiducial,"in_fiducial/I");
	
    _tree->Branch("dx",       &_dx);
    _tree->Branch("dy",       &_dy);    
    _tree->Branch("dz",       &_dz);
    _tree->Branch("scedx",    &_scedx);
    _tree->Branch("scedy",    &_scedy);    
    _tree->Branch("scedz",    &_scedz);
    
  }

  bool VertexAna::process(IOManager& mgr)
  {


    ClearEvent();
    
    bool has_mc = false;
    bool has_reco_vtx = false;

    auto const ev_img2d = (EventImage2D*)(mgr.get_data(kProductImage2D,_img2d_prod));
    if (!ev_img2d) throw larbys("Invalid image producer provided");

    auto const ev_croi_v     = (EventROI*)(mgr.get_data(kProductROI,_reco_roi_prod));
    if (!ev_croi_v) throw larbys("Invalid cROI producer provided");

    EventPGraph* ev_pgraph = nullptr;
    if (!_pgraph_prod.empty()) {
      ev_pgraph = (EventPGraph*)(mgr.get_data(kProductPGraph,_pgraph_prod));
      if (!ev_pgraph) throw larbys("Invalid pgraph producer provided!");
      has_reco_vtx = true;
    }

    EventPixel2D* ev_ctor_v = nullptr;
    if (!_pcluster_ctor_prod.empty()) {
      ev_ctor_v = (EventPixel2D*)(mgr.get_data(kProductPixel2D,_pcluster_ctor_prod));
      if (!ev_ctor_v) throw larbys("Invalid Contour Pixel2D producer provided!");
      if (!has_reco_vtx) throw larbys("Gave PGraph producer but no particle cluster?");
    }

    EventPixel2D* ev_pcluster_v = nullptr;
    if (!_pcluster_img_prod.empty()) {
      ev_pcluster_v = (EventPixel2D*)(mgr.get_data(kProductPixel2D,_pcluster_img_prod));
      if (!ev_pcluster_v) throw larbys("Invalid Particle Pixel2D producer provided!");
      if (!has_reco_vtx) throw larbys("Gave PGraph producer but no image cluster?");
    }

    EventROI* ev_roi_v = nullptr;
    if (!_truth_roi_prod.empty()) {
      ev_roi_v = (EventROI*)(mgr.get_data(kProductROI,_truth_roi_prod));
      has_mc = true;
      if (!ev_roi_v) throw larbys("Invalid truth roi producer provided");
      if (ev_roi_v->ROIArray().empty()) throw larbys("Invalid truth roi producer provided");
    }

    _run    = ev_img2d->run();
    _subrun = ev_img2d->subrun();
    _event  = ev_img2d->event();
    _entry  = mgr.current_entry();

    LARCV_DEBUG() << "Got RSEE=("<<_run<<","<<_subrun<<","<<_event<<","<<_entry<<")"<<std::endl;
    const auto& adc_img_v = ev_img2d->Image2DArray();

    _tx = _ty = _tz = _tt = _te = -1.;
    _scex = _scey = _scez = -1.;
    auto geo = larutil::Geometry::GetME();
    auto larp = larutil::LArProperties::GetME();
    double xyz[3];
    double wire_v[3];

    _nprotons = 0;
    _nothers  = 0;    

    if(has_mc) {
      for(auto const& roi : ev_roi_v->ROIArray()){
	if(std::abs(roi.PdgCode()) == 12 || std::abs(roi.PdgCode()) == 14 || _first_roi) {
	  _tx = roi.X();
	  _ty = roi.Y();
	  _tz = roi.Z();
	  _tt = roi.T();
	  _te = roi.EnergyInit();
	  auto const offset = _sce.GetPosOffsets(_tx,_ty,_tz);
	  _scex = _tx - offset[0] + 0.7;
	  _scey = _ty + offset[1];
	  _scez = _tz + offset[2];
	  if (_first_roi) break;
	}

	if ( roi.PdgCode()==2212 ) {
	  if ( (roi.EnergyInit()-938.0)>60.0 )
	    _nprotons++;
	}
	else if ( roi.PdgCode()==111 || roi.PdgCode()==211 || roi.PdgCode()==-211 || roi.PdgCode()==22 || abs(roi.PdgCode())>100 ) {
	  _nothers++;
	}
      }

      xyz[0] = _scex;
      xyz[1] = _scey;
      xyz[2] = _scez;

      try {
	wire_v[0] = geo->NearestWire(xyz,0);
	wire_v[1] = geo->NearestWire(xyz,1);
	wire_v[2] = geo->NearestWire(xyz,2);
	_nearest_wire_err = 0;
      } catch(const std::exception& e) {
	LARCV_WARNING()<<"Cannot find nearest wire for ("<<xyz[0]<<","<<xyz[1]<<","<<xyz[2]<<")"<<std::endl;
	wire_v[0] = wire_v[1] = wire_v[2] = kINVALID_DOUBLE;
	_nearest_wire_err = 1;
      }
    }


    const double tick = (_scex / larp->DriftVelocity() + 4) * 2. + 3200.;
    _num_croi    = ev_croi_v->ROIArray().size();
    _num_croi_with_vertex = 0;
    _good_croi0 = 0;
    _good_croi1 = 0;
    _good_croi2 = 0;
    _good_croi_ctr = 0;

    std::vector<std::vector<ImageMeta> > roid_v;
    roid_v.reserve(ev_croi_v->ROIArray().size());

    for(auto const& croi : ev_croi_v->ROIArray()) {
      auto const& bb_v = croi.BB();
      roid_v.push_back(crop_metas(adc_img_v,bb_v));

      if (has_mc) {
	size_t good_croi_ctr = 0;
	for(size_t plane=0; plane<bb_v.size(); ++plane) {
	  auto const& croi_meta = bb_v[plane];
	  auto const& wire = wire_v[plane];
	  if( croi_meta.min_x() <= wire && wire <= croi_meta.max_x() &&
	      croi_meta.min_y() <= tick && tick <= croi_meta.max_y() )
	    ++good_croi_ctr;
	}
	if(good_croi_ctr <= _good_croi_ctr) continue;
	if(good_croi_ctr > 1) _good_croi_ctr+=1;

	for(size_t plane=0; plane<bb_v.size(); ++plane) {
	  auto const& croi_meta = bb_v[plane];
	  auto const& wire = wire_v[plane];
	  if( croi_meta.min_x() <= wire && wire <= croi_meta.max_x() &&
	      croi_meta.min_y() <= tick && tick <= croi_meta.max_y() ) {
	    if(plane == 0) _good_croi0 = 1;
	    if(plane == 1) _good_croi1 = 1;
	    if(plane == 2) _good_croi2 = 1;
	  }
	}
      }
    } // end check of true vertex in cROI

    if (!has_reco_vtx) {
      _event_tree->Fill();
      return true;
    }

    _num_vertex  = ev_pgraph->PGraphArray().size();

    std::vector<size_t> plane_order_v = {2,0,1};
    _min_vtx_dist = 1.e9;
    _vtxid=-1;

    auto vtx_counts = ev_pgraph->PGraphArray().size();

    LARCV_DEBUG() << "Got " << vtx_counts << " vertices" << std::endl;
    for (int vtx_idx = 0; vtx_idx < vtx_counts; ++vtx_idx) {
      ClearVertex();
      _vtxid += 1;

      auto pgraph = ev_pgraph->PGraphArray().at(vtx_idx);

      auto const& roi_v = pgraph.ParticleArray();
      auto const& bb_v = roi_v.front().BB();

      auto iter = std::find(roid_v.begin(),roid_v.end(),bb_v);
      if (iter == roid_v.end()) throw larbys("Unknown image meta");

      auto roid = iter - roid_v.begin();

      if (roid != _roid) _vtxid = 0;

      _roid  = roid;

      _npar = pgraph.ClusterIndexArray().size();

      auto const& a_roi = roi_v.front();

      _x = a_roi.X();
      _y = a_roi.Y();
      _z = a_roi.Z();

      _dr    = sqrt(pow(_x - _tx  ,2) + pow(_y - _ty  ,2) + pow(_z - _tz  ,2));
      _scedr = sqrt(pow(_x - _scex,2) + pow(_y - _scey,2) + pow(_z - _scez,2));

      _dx = _x - _tx;
      _dy = _y - _ty;
      _dz = _z - _tz;

      _scedx = _x - _scex;
      _scedy = _y - _scey;
      _scedz = _z - _scez;

      if(_use_scedr){
        if(_scedr < _min_vtx_dist) _min_vtx_dist = _scedr;
      }
      else{
        if(_dr < _min_vtx_dist) _min_vtx_dist = _dr;
      }

      _in_fiducial = InFiducialRegion3D(_x,_y,_z);
      
      _tree->Fill();
    } // end loop over vertex
    
    _event_tree->Fill();

    return true;
  }

  void VertexAna::finalize()
  {
    if(has_ana_file()) {
      _tree->Write();
      _event_tree->Write();
    }
  }

  void VertexAna::ClearEvent() {

    // Indices
    _entry = kINVALID_INT;
    _run = kINVALID_INT;
    _subrun = kINVALID_INT;
    _event = kINVALID_INT;
    _roid = kINVALID_INT;
    _vtxid = kINVALID_INT;

    // MC vertex
    _tx = kINVALID_DOUBLE;
    _ty = kINVALID_DOUBLE;
    _tz = kINVALID_DOUBLE;
    _te = kINVALID_DOUBLE;
    _tt = kINVALID_DOUBLE;
    
    // SCE corrected MC vertex
    _scex = kINVALID_DOUBLE;
    _scey = kINVALID_DOUBLE;
    _scez = kINVALID_DOUBLE;

    // Reconstructed
    _x = kINVALID_DOUBLE;
    _y = kINVALID_DOUBLE;
    _z = kINVALID_DOUBLE;

    _dr = kINVALID_DOUBLE;
    _scedr = kINVALID_DOUBLE;

    _npar = kINVALID_INT;

    _good_croi0 = kINVALID_INT;
    _good_croi1 = kINVALID_INT;
    _good_croi2 = kINVALID_INT;
    _good_croi_ctr = kINVALID_INT;

    _num_croi = kINVALID_INT;
    _num_vertex = kINVALID_INT;
    _num_croi_with_vertex = kINVALID_INT;

    _min_vtx_dist = kINVALID_DOUBLE;
    _nearest_wire_err = kINVALID_INT;

    _in_fiducial = false;

    _vtxid = kINVALID_INT;

    _nprotons = kINVALID_INT;
    _nothers = kINVALID_INT;
    
    ClearVertex();
  }

  void VertexAna::ClearVertex() {
    _x = kINVALID_DOUBLE;
    _y = kINVALID_DOUBLE;
    _z = kINVALID_DOUBLE;

    _dx = kINVALID_DOUBLE;
    _dy = kINVALID_DOUBLE;
    _dz = kINVALID_DOUBLE;

    _dr = kINVALID_DOUBLE;

    _scedx = kINVALID_DOUBLE;
    _scedy = kINVALID_DOUBLE;
    _scedz = kINVALID_DOUBLE;

    _scedr = kINVALID_DOUBLE;

    _npar = kINVALID_INT;

    _in_fiducial = kINVALID_INT;
    
    
  }
  
}
#endif
