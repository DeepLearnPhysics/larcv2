#ifndef __PARTICLEANA_H__
#define __PARTICLEANA_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include "LArbysImageMaker.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "TTree.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventROI.h"
#include "DataFormat/EventPGraph.h"
#include "DataFormat/EventPixel2D.h"

namespace larcv {

  class ParticleAna : public ProcessBase {

  public:
    
    /// Default constructor
    ParticleAna(const std::string name="ParticleAna");
    
    /// Default destructor
    ~ParticleAna(){}

    void configure(const PSet&);
    void initialize();
    bool process(IOManager& mgr);
    void finalize();

  private:
    
    EventImage2D* _ev_img_v;
    EventImage2D* _ev_trk_img_v;
    EventImage2D* _ev_shr_img_v;
    EventROI* _ev_roi_v;
    EventPGraph* _ev_pgraph_v;
    EventPixel2D* _ev_pcluster_v;
    EventPixel2D* _ev_ctor_v;

    int _run;
    int _subrun;
    int _event;
    int _entry;
    int _vtxid;
    
    std::string _img_prod;       
    std::string _reco_roi_prod;
    std::string _pgraph_prod;
    std::string _pcluster_img_prod;
    std::string _pcluster_ctor_prod; 
    std::string _trk_img_prod;
    std::string _shr_img_prod;
    
    LArbysImageMaker _LArbysImageMaker;

    //
    // Signal Related Functionality -- vic
    //
    bool _analyze_signal;

    TTree* _signal_tree;

    void AnalyzeSignal();
    
    int _vtx_id;
    float _vtx_x;
    float _vtx_y;
    float _vtx_z;
    
    int _pathexists;
    int _pathexists2;
    std::vector<int> _pathexists_v;
    int _nparticles;
    std::vector<float> _track_frac_v;
    std::vector<float> _shower_frac_v;
    int _infiducial;
    int _charge_neighbor2;
    int _charge_neighbor3;
    std::vector<int> _charge_neighbor_v;
    
    //
    // Particle Related Functionality -- vic
    //
    bool _analyze_particle;

    TTree* _particle_tree;

    void AnalyzeParticle();

    float _length;
    float _width;
    float _perimeter;
    float _area;
    uint  _npixel;
    float _track_frac;
    float _shower_frac;
    double _mean_pixel_dist;
    double _sigma_pixel_dist;
    double _angular_sum;
    
    //
    // Angle & dQdX Related Functionality -- rui
    //
    bool _analyze_angle;
    bool _analyze_dqdx;
    
    TTree* _angle_tree;
    TTree* _dqdx_tree;
    
    void AnalyzeAngle();
    void AnalyzedQdX();
    
    double Getx2vtxmean( ::larocv::GEO2D_Contour_t ctor, float x2d, float y2d, double& pct);
    cv::Point PointShift(::cv::Point pt, geo2d::Line<float> pca);

    double Mean(const std::vector<float>& v);
    double STD(const std::vector<float>& v);
    
    float _meanl;
    float _meanr;
    float _stdl;
    float _stdr;
    float _dqdxdelta;
    float _dqdxratio;

    uint _plane;
    double _pradius;
    double _maskradius;
    uint _bins;
    float _open_angle_cut;
    float _adc_threshold;
    
    std::vector<double> _angle0_c;
    std::vector<double> _angle1_c;
    std::vector<double> _angle0_p;
    std::vector<double> _angle1_p;
    uint _straight_lines;
    double _angle_diff_c;
    double _angle_diff_p;
    std::vector<double> _angle_diff_c_v;
    std::vector<double> _angle_diff_p_v;
    
    std::vector<double> _mean0c; //mean value of x in a ctor w.r.t the vertex
    std::vector<double> _mean1c; //to determine the direction of PCA w.r.t to the vertex
    std::vector<double> _mean0p; //mean value of x in a ctor w.r.t the vertex
    std::vector<double> _mean1p; //to determine the direction of PCA w.r.t to the vertex
    
    std::vector<double> _dir0_c;//particle direction from contour
    std::vector<double> _dir1_c;
    std::vector<double> _dir0_p;//particle direction from pixels close to vertex
    std::vector<double> _dir1_p;

    //For plot 
    std::vector<std::vector<int>> _p0pxc; // per plane
    std::vector<std::vector<int>> _p0pyc;
    std::vector<std::vector<int>> _p1pxc;
    std::vector<std::vector<int>> _p1pyc;

    std::vector<std::vector<int>> _p0pxp; // per plane
    std::vector<std::vector<int>> _p0pyp;
    std::vector<std::vector<int>> _p1pxp;
    std::vector<std::vector<int>> _p1pyp;
    
    std::vector<double> _x2d_v;
    std::vector<double> _y2d_v;

    std::vector<double> _pct_0_c_v; 
    std::vector<double> _pct_1_c_v;
    std::vector<double> _pct_0_p_v; 
    std::vector<double> _pct_1_p_v;    
  };

  /**
     \class larcv::ParticleAnaFactory
     \brief A concrete factory class for larcv::ParticleAna
  */
  class ParticleAnaProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ParticleAnaProcessFactory() { ProcessFactory::get().add_factory("ParticleAna",this); }
    /// dtor
    ~ParticleAnaProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ParticleAna(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

