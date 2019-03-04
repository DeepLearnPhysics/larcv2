/**
 * \file ROIAna.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ROIAna
 *
 * @author vgenty
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __ROIANA_H__
#define __ROIANA_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventROI.h"
#include "LArUtil/SpaceChargeMicroBooNE.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class ROIAna ... these comments are used to generate
     doxygen documentation!
  */
  class ROIAna : public ProcessBase {

  public:
    
    /// Default constructor
    ROIAna(const std::string name="ROIAna");
    
    /// Default destructor
    ~ROIAna(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    float area(const ImageMeta& meta) const;
    float area(const ImageMeta* meta) const;
    void clear();

    TTree* _roi_tree;
    
    std::string _roi_producer;
    std::string _img_producer;
std::string _seg_producer;

    int _run;
    int _subrun;
    int _event;
    int _entry;
    
    int _nroi;
    
    float _area_exclusive0;
    float _area_exclusive1;
    float _area_exclusive2;

    float _area_inclusive0;
    float _area_inclusive1;
    float _area_inclusive2;

    float _union_area0;
    float _union_area1;
    float _union_area2;

    float _area_image0;
    float _area_image1;
    float _area_image2;
    
    larutil::SpaceChargeMicroBooNE _sce;

    int _good_croi0;
    int _good_croi1;
    int _good_croi2;
    int _good_croi_ctr;

    int _nearest_wire_err;    
  };

  /**
     \class larcv::ROIAnaFactory
     \brief A concrete factory class for larcv::ROIAna
  */
  class ROIAnaProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ROIAnaProcessFactory() { ProcessFactory::get().add_factory("ROIAna",this); }
    /// dtor
    ~ROIAnaProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ROIAna(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

