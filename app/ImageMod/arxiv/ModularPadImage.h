/**
 * \file ModularPadImage.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ModularPadImage
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __MODULARPADIMAGE_H__
#define __MODULARPADIMAGE_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ModularPadImage ... these comments are used to generate
     doxygen documentation!
  */
  class ModularPadImage : public ProcessBase {

  public:
    
    /// Default constructor
    ModularPadImage(const std::string name="ModularPadImage");
    
    /// Default destructor
    ~ModularPadImage(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    size_t _modular_col;
    size_t _modular_row;
    size_t _max_col;
    size_t _max_row;
    std::string _input_producer;
    std::string _output_producer;
    std::vector<size_t> _ch_v;

  };

  /**
     \class larcv::ModularPadImageFactory
     \brief A concrete factory class for larcv::ModularPadImage
  */
  class ModularPadImageProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ModularPadImageProcessFactory() { ProcessFactory::get().add_factory("ModularPadImage",this); }
    /// dtor
    ~ModularPadImageProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ModularPadImage(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

