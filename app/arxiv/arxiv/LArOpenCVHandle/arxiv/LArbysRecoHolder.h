#ifndef LARBYSRECOHOLDER_H
#define LARBYSRECOHOLDER_H

#include "TTree.h"
#include "Base/larcv_base.h"
#include "Base/PSet.h"
#include "LArOpenCV/Core/ImageManager.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterManager.h"
#include "LArOpenCV/ImageCluster/AlgoClass/VertexAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"
#include "DataFormat/Image2D.h"
#include "DataFormat/ImageMeta.h"

namespace larcv {

  class LArOCVSerial{
  public:
    LArOCVSerial(){}
    ~LArOCVSerial(){}
    void Clear();
    std::vector<larocv::data::Vertex3D> _vertex_v;
    std::vector<int> _matched_v;
    std::vector<std::vector<std::vector<larocv::data::ParticleCluster> > > _particle_cluster_vvv;
    std::vector<std::vector<std::vector<larocv::data::TrackClusterCompound> > > _track_comp_vvv;
    std::vector<std::vector<std::vector<std::pair<size_t,size_t> > > > _match_pvvv;
    std::vector<std::vector<larcv::ImageMeta> > _meta_vv;
  };  

  
  class LArbysRecoHolder : public larcv_base {

  public:

    LArbysRecoHolder() :
      _vtx_ana(),
      _out_tree(nullptr),
      _larocvserial(nullptr)
    {  Reset(); }

    ~LArbysRecoHolder(){}

    void Configure(const PSet& pset);    

    void Initialize();

    void SetMeta(const std::vector<Image2D>& adc_img_v);

    void FilterMatches();

    void ShapeData(const larocv::ImageClusterManager& mgr);
    
    void Filter();

    void SetMatches(std::vector<int>&& vtxid_match_v);
    
    std::vector<std::vector<std::pair<size_t,size_t> > >
    Match(size_t vtx_id,
	  const std::vector<cv::Mat>& adc_cvimg_v,
	  bool stort=true);

    void ResetOutput();
    void StoreEvent(size_t run, size_t subrun, size_t event, size_t entry);
    bool WriteOut(TFile* fout);
    void Reset();
    void Write();
    
    //
    // Vertex getters
    //

    const larocv::VertexAnalysis& ana()
    { return _vtx_ana; }
    
    const std::vector<const larocv::data::Vertex3D*>
    Verticies()
    { return _vertex_ptr_v; }

    const larocv::data::Vertex3D*
    Vertex(size_t vertexid)
    { return _vertex_ptr_v[vertexid]; }

    //
    // ParticleCluster getters
    //
    
    const std::vector<std::vector<std::vector<const larocv::data::ParticleCluster*> > >&
    VertexPlaneParticles()
    { return _particle_cluster_ptr_vvv; }
    
    const std::vector<std::vector<const larocv::data::ParticleCluster*> >&
    PlaneParticles(size_t vertexid)
    { return _particle_cluster_ptr_vvv[vertexid]; }
    
    const std::vector<const larocv::data::ParticleCluster*>&
    Particles(size_t vertexid,size_t planeid)
    { return _particle_cluster_ptr_vvv[vertexid][planeid]; }

    const larocv::data::ParticleCluster*
    Particle(size_t vertexid,size_t planeid,size_t particleid)
    { return _particle_cluster_ptr_vvv[vertexid][planeid][particleid]; }

    //
    // TrackClusterCompound getters
    //
    const std::vector<std::vector<std::vector<const larocv::data::TrackClusterCompound*> > >&
    VertexPlaneTracks()
    { return _track_comp_ptr_vvv; }
    
    const std::vector<std::vector<const larocv::data::TrackClusterCompound*> >&
    PlaneTracks(size_t vertexid)
    { return _track_comp_ptr_vvv[vertexid]; }
    
    const std::vector<const larocv::data::TrackClusterCompound*>&
    Tracks(size_t vertexid,size_t planeid)
    { return _track_comp_ptr_vvv[vertexid][planeid]; }
    
    const larocv::data::TrackClusterCompound*
    Track(size_t vertexid,size_t planeid,size_t trackid)
    { return _track_comp_ptr_vvv[vertexid][planeid][trackid]; }

  private:
    
    std::string _output_module_name;
    size_t _output_module_offset;
    bool _require_two_multiplicity;
    bool _require_fiducial;
    float _match_coverage;
    float _match_particles_per_plane;
    float _match_min_number;
    bool _match_check_type;
    bool _match_weight_by_size;
    
    larocv::VertexAnalysis _vtx_ana;
    
    std::vector<const larocv::data::Vertex3D*> _vertex_ptr_v;
    std::vector<std::vector<std::vector<const larocv::data::ParticleCluster*> > > _particle_cluster_ptr_vvv;
    std::vector<std::vector<std::vector<const larocv::data::TrackClusterCompound*> > > _track_comp_ptr_vvv;
    std::vector<int> _matched_v;
      
    TTree* _out_tree;
    uint _run;
    uint _subrun;
    uint _event;
    uint _entry;
    LArOCVSerial* _larocvserial;
    
  };
}

#endif

