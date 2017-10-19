#ifndef __VERTEXANA_H__
#define __VERTEXANA_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include "LArUtil/SpaceChargeMicroBooNE.h"

#include <TTree.h>
#include <math.h>
#include <numeric>

namespace larcv {

  class VertexAna : public ProcessBase {

  public:

    /// Default constructor
    VertexAna(const std::string name="VertexAna");

    /// Default destructor
    ~VertexAna(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    TTree* _tree;
    TTree* _event_tree;

    // Indices
    int _entry;
    int _run;
    int _subrun;
    int _event;
    int _roid;
    int _vtxid;

    // MC vertex
    double _tx;
    double _ty;
    double _tz;
    double _te;
    double _tt;

    // SCE corrected MC vertex
    double _scex;
    double _scey;
    double _scez;

    int _nprotons; //< number of truth protons
    int _nothers;  //< number of other particles    

    // Reconstructed
    double _x;
    double _y;
    double _z;

    double _dx;
    double _dy;
    double _dz;
   
    double _scedx;
    double _scedy;
    double _scedz;

    double _dr;
    double _scedr;

    int _npar;

    int _good_croi0;
    int _good_croi1;
    int _good_croi2;
    int _good_croi_ctr;

    int _num_croi;
    int _num_vertex;
    int _num_croi_with_vertex;

    double _min_vtx_dist;
    int _nearest_wire_err;

    int _in_fiducial;
    
    ::larutil::SpaceChargeMicroBooNE _sce;

    std::string _img2d_prod;
    std::string _pgraph_prod;
    std::string _pcluster_ctor_prod;
    std::string _pcluster_img_prod;
    std::string _truth_roi_prod;
    std::string _reco_roi_prod;
    bool _first_roi;
    bool _use_scedr;

  private:
    
    void ClearEvent();
    void ClearVertex();

  };

  /**
     \class larcv::VertexAnaFactory
     \brief A concrete factory class for larcv::VertexAna
  */
  class VertexAnaProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    VertexAnaProcessFactory() { ProcessFactory::get().add_factory("VertexAna",this); }
    /// dtor
    ~VertexAnaProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new VertexAna(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group
