/**
 * \file SuperaMCROI.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class SuperaMCROI
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SUPERAMCBASE_H__
#define __SUPERAMCBASE_H__
//#ifndef __CINT__
//#ifndef __CLING__
#include "SuperaBase.h"
#include "FMWKInterface.h"
#include "MCParticleTree.h"
#include "MCROIMaker.h"
#include "ImageMetaMaker.h"
#include "ParamsROI.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class SuperaMCROI ... these comments are used to generate
     doxygen documentation!
  */
  class SuperaMCROI : public SuperaBase,
		      public supera::ParamsROI,
		      public supera::ImageMetaMaker {

  public:
    
    /// Default constructor
    SuperaMCROI(const std::string name="SuperaMCROI");
    
    /// Default destructor
    ~SuperaMCROI(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    bool StoreG4SecondaryROI() const { return _store_g4_secondary_roi; }
    
    bool StoreG4PrimaryROI() const { return _store_g4_primary_roi; }

    const std::vector<std::pair<size_t,size_t> >& ROI2MCNode(int roi_index) const;

    const std::vector<std::vector<std::pair<size_t,size_t> > >&
    ROI2MCNode() const { return _roi2mcnode_vv; }
    
    const supera::MCParticleTree& ParticleTree() const { return _mcpt; }
    
  private:

    bool _store_roi;
    bool _store_g4_primary_roi;
    bool _store_g4_secondary_roi;
    std::vector<larcv::ROI> _roi_v;
    supera::MCParticleTree _mcpt;
    supera::MCROIMaker     _mcroi_maker;
    std::vector<std::vector<std::pair<size_t,size_t> > > _roi2mcnode_vv;

    unsigned short _pass_origin;
    std::vector<int> _filter_pdg;
    std::vector<double> _filter_min_einit;
    std::vector<double> _filter_min_edep;

    double _shower_min_einit;
    double _shower_min_edep;
    
    double _track_min_einit;
    double _track_min_edep;

    size_t _filter_min_cols;
    size_t _filter_min_rows;

    bool FilterNode(const supera::MCNode& node) const;
    larcv::ImageMeta FormatMeta(const larcv::ImageMeta& part_image,
				const larcv::ImageMeta& event_image) const;
    larcv::ROI MakeROI(const supera::MCNode& node,const std::vector<supera::LArSimCh_t>& sch_v) const;
    void UpdatePrimaryROI(larcv::ROI& pri_roi,
			  std::vector<larcv::ROI>& sec_roi_v) const;
  };

  /**
     \class larcv::SuperaMCROIFactory
     \brief A concrete factory class for larcv::SuperaMCROI
  */
  class SuperaMCROIProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SuperaMCROIProcessFactory() { ProcessFactory::get().add_factory("SuperaMCROI",this); }
    /// dtor
    ~SuperaMCROIProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SuperaMCROI(instance_name); }
  };

}
//#endif
//#endif
#endif
/** @} */ // end of doxygen group 

