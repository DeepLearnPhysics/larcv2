/**
 * \file ImageChargeAna.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ImageChargeAna
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __IMAGECHARGEANA_H__
#define __IMAGECHARGEANA_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include <TTree.h>
namespace larcv {

  /**
     \class ProcessBase
     User defined class ImageChargeAna ... these comments are used to generate
     doxygen documentation!
  */
  class ImageChargeAna : public ProcessBase {

  public:
    
    /// Default constructor
    ImageChargeAna(const std::string name="ImageChargeAna");
    
    /// Default destructor
    ~ImageChargeAna(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    TTree* _tree;
    double _q0, _q1, _q2, _qsum;

  };

  /**
     \class larcv::ImageChargeAnaFactory
     \brief A concrete factory class for larcv::ImageChargeAna
  */
  class ImageChargeAnaProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ImageChargeAnaProcessFactory() { ProcessFactory::get().add_factory("ImageChargeAna",this); }
    /// dtor
    ~ImageChargeAnaProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ImageChargeAna(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

