#ifndef __LARBYSIMAGEFILTER_H__
#define __LARBYSIMAGEFILTER_H__
#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include "LArbysImageAna.h"
#include "LArbysImageExtract.h"
#include "LArOpenCV/ImageCluster/AlgoClass/VertexAnalysis.h"

namespace larcv {
  
  class LArbysImageFilter : public ProcessBase {

  public:
    LArbysImageFilter(const std::string name="LArbysImageFilter");
    ~LArbysImageFilter(){}
    void configure(const PSet&);
    void initialize();
    bool process(IOManager& mgr);
    void finalize();
    void SetLArbysImageAna(LArbysImageAna* larbysana_ptr) 
    { _larbysana_ptr = larbysana_ptr; }
    void SetLArbysImageExtract(LArbysImageExtract* larbysext_ptr) 
    { _larbysext_ptr = larbysext_ptr; }
  private:
    void ClearVertex();
    void ClearEvent();
    void WriteOut(size_t vertexid);
    LArbysImageAna* _larbysana_ptr;
    LArbysImageExtract* _larbysext_ptr;
    larocv::VertexAnalysis _vtx_ana;
    bool _require_two_multiplicity;
    bool _require_match;
    bool _require_fiducial;
    std::string _filtereventtreename;
    std::string _filtervertextreename;
    //Event Tree
    TTree* _event_tree;
    uint _run;
    uint _subrun;
    uint _event;
    uint _entry;
    std::vector<larocv::data::Vertex3D> _vertex3d_v;
    std::vector<std::vector<std::vector<larocv::data::ParticleCluster> > > _particle_cluster_vvv;
    std::vector<std::vector<std::vector<larocv::data::TrackClusterCompound> > > _track_compound_vvv;
    //Vertex Tree
    TTree* _vtx3d_tree;
    uint _vtx3d_id;
    uint _vtx3d_n_planes;
    uint _vtx3d_type;
    double _vtx3d_x;
    double _vtx3d_y;
    double _vtx3d_z;
    std::vector<double> _vtx2d_x_v;
    std::vector<double> _vtx2d_y_v;
    std::vector<uint> _ntrack_par_v;
    std::vector<uint> _nshower_par_v;
    std::vector<double> _circle_x_v;
    std::vector<double> _circle_y_v;
    std::vector<uint> _par_multi;
    TTree* _mc_tree;
  };
  class LArbysImageFilterProcessFactory : public ProcessFactoryBase {
  public:
    LArbysImageFilterProcessFactory() { ProcessFactory::get().add_factory("LArbysImageFilter",this); }
    ~LArbysImageFilterProcessFactory() {}
    ProcessBase* create(const std::string instance_name) { return new LArbysImageFilter(instance_name); }
  };
}
#endif

