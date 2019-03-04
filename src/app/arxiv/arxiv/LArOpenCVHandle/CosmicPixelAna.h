#ifndef __COSMICPIXELANA_H__
#define __COSMICPIXELANA_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include "LArbysImageMaker.h"
#include "LArUtil/SpaceChargeMicroBooNE.h"

namespace larcv {

  class CosmicPixelAna : public ProcessBase {

  public:

   
    /// Default constructor
    CosmicPixelAna(const std::string name="CosmicPixelAna");
    
    /// Default destructor
    ~CosmicPixelAna(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();
    
  private:

    LArbysImageMaker _LArbysImageMaker;

    TTree* _tree;
    
    // Indices
    int _entry;
    int _run;
    int _subrun;
    int _event;

    std::string _ev_img2d_prod;
    std::string _seg_img2d_prod;
    std::string _thrumu_img_prod;
    std::string _stopmu_img_prod;
    std::string _true_roi_prod;
    std::string _roi_prod;
    size_t _crop_radius;
    ProductType_t _tags_datatype;
    
    //
    // Neutrino
    //
    int _nupixel0;
    int _nupixel1;
    int _nupixel2;

    int _cosmicpixel0;
    int _cosmicpixel1;
    int _cosmicpixel2;
    
    float _ratiopixel0;
    float _ratiopixel1;
    float _ratiopixel2;

    float _nupixelsum;
    float _nupixelavg;

    float _cosmicpixelsum;
    float _cosmicpixelavg;
    
    float _ratiopixelsum;
    float _ratiopixelavg;


    //
    // Vertex
    //
    larutil::SpaceChargeMicroBooNE _sce;
    
    int _vertex_nupixel0;
    int _vertex_nupixel1;
    int _vertex_nupixel2;

    int _vertex_cosmicpixel0;
    int _vertex_cosmicpixel1;
    int _vertex_cosmicpixel2;
    
    float _vertex_ratiopixel0;
    float _vertex_ratiopixel1;
    float _vertex_ratiopixel2;

    float _vertex_nupixelsum;
    float _vertex_nupixelavg;

    float _vertex_cosmicpixelsum;
    float _vertex_cosmicpixelavg;
    
    float _vertex_ratiopixelsum;
    float _vertex_ratiopixelavg;


    //
    //
    //
    int _nearest_wire_error;
    int _not_inside;
  };

  /**
     \class larcv::CosmicPixelAnaFactory
     \brief A concrete factory class for larcv::CosmicPixelAna
  */
  class CosmicPixelAnaProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    CosmicPixelAnaProcessFactory() { ProcessFactory::get().add_factory("CosmicPixelAna",this); }
    /// dtor
    ~CosmicPixelAnaProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new CosmicPixelAna(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

