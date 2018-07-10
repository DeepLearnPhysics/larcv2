/**
 * \file CropNeutrino2D.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class CropNeutrino2D
 *
 * @author cadams
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __CROPNEUTRINO2D_H__
#define __CROPNEUTRINO2D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/DataFormat/Image2D.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class CropNeutrino2D ... these comments are used to generate
     doxygen documentation!
  */
  class CropNeutrino2D : public ProcessBase {

  public:

    /// Default constructor
    CropNeutrino2D(const std::string name = "CropNeutrino2D");

    /// Default destructor
    ~CropNeutrino2D() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:


    std::string  _vertex_cluster2d_producer;
    int _output_rows;
    int _output_cols;
    std::vector<std::string>  _producer_names_v;
    std::vector<std::string>  _product_types_v;
    std::vector<std::string>  _output_producers_v;

  };

  /**
     \class larcv::CropNeutrino2DFactory
     \brief A concrete factory class for larcv::CropNeutrino2D
  */
  class CropNeutrino2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    CropNeutrino2DProcessFactory() { ProcessFactory::get().add_factory("CropNeutrino2D", this); }
    /// dtor
    ~CropNeutrino2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new CropNeutrino2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

