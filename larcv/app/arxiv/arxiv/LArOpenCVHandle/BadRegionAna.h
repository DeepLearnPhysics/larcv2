/**
 * \file BadRegionAna.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class BadRegionAna
 *
 * @author vgenty
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __BADREGIONANA_H__
#define __BADREGIONANA_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include "Geo2D/Core/Line.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class BadRegionAna ... these comments are used to generate
     doxygen documentation!
  */
  class BadRegionAna : public ProcessBase {

  public:
    
    /// Default constructor
    BadRegionAna(const std::string name="BadRegionAna");
    
    /// Default destructor
    ~BadRegionAna(){}

    void configure(const PSet&);
    void initialize();
    bool process(IOManager& mgr);
    void finalize();


  private:

    TTree *_tree;
    
    std::string _pgraph_prod;
    std::string _reco_roi_prod;
    std::string _adc_img_prod;

    int _run;
    int _subrun;
    int _event;
    int _entry;
    int _roid;
    int _vtxid;
    int _in_region_0;
    int _in_region_1;
    int _in_region_2;
    int _in_region_3;
    int _in_region_4;
    int _in_region_5;
    int _in_region_yz;
    int _in_region_xz;
    int _in_region;
    
    
    std::vector<std::array<geo2d::Vector<float>, 4> > _yz_bound_v;
    std::vector<std::array<geo2d::Vector<float>, 4> > _xz_bound_v;
    
  };

  /**
     \class larcv::BadRegionAnaFactory
     \brief A concrete factory class for larcv::BadRegionAna
  */
  class BadRegionAnaProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BadRegionAnaProcessFactory() { ProcessFactory::get().add_factory("BadRegionAna",this); }
    /// dtor
    ~BadRegionAnaProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new BadRegionAna(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

