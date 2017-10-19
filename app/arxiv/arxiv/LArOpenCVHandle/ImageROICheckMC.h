/**
 * \file ImageROICheckMC.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ImageROICheckMC
 *
 * @author jarrett
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __IMAGEROICHECKMC_H__
#define __IMAGEROICHECKMC_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include <vector>
#include "DataFormat/EventROI.h"


namespace larcv {

  /**
     \class ProcessBase
     User defined class ImageROICheckMC ... these comments are used to generate
     doxygen documentation!
  */
  class ImageROICheckMC : public ProcessBase {

  public:
    
    /// Default constructor
    ImageROICheckMC(const std::string name="ImageROICheckMC");
    
    /// Default destructor
    ~ImageROICheckMC(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);
    
    bool CheckNumNuROIs(IOManager& mgr);

    bool VertexInFid(IOManager& mgr);

    void finalize();

  private:

    std::string _producer_roi;
    bool _check_num_bb;
    bool _check_vtxfid;

    float xmin =  0.0;
    float ymin = -116.5;
    float zmin =  0.0;

    float xmax =  256.25;
    float ymax =  116.5;
    float zmax = 1036.8;

    float edge_x = 10.0;
    float edge_y = 10.0;
    float edge_z = 10.0;
    
  };

  /**
     \class larcv::ImageROICheckMCFactory
     \brief A concrete factory class for larcv::ImageROICheckMC
  */
  class ImageROICheckMCProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ImageROICheckMCProcessFactory() { ProcessFactory::get().add_factory("ImageROICheckMC",this); }
    /// dtor
    ~ImageROICheckMCProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ImageROICheckMC(instance_name); }
  };
  
}

#endif
/** @} */ // end of doxygen group 

