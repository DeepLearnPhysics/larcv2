/**
 * \file CropNeutrino3D.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class CropNeutrino3D
 *
 * @author cadams
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __CROPNEUTRINO3D_H__
#define __CROPNEUTRINO3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/DataFormat/Image2D.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class CropNeutrino3D ... these comments are used to generate
     doxygen documentation!
  */
  class CropNeutrino3D : public ProcessBase {

  public:

    /// Default constructor
    CropNeutrino3D(const std::string name = "CropNeutrino3D");

    /// Default destructor
    ~CropNeutrino3D() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string  _vertex_cluster3d_producer;
    int _output_n_x;
    int _output_n_y;
    int _output_n_z;
    std::vector<std::string>  _producer_names_v;
    std::vector<std::string>  _product_types_v;
    std::vector<std::string>  _output_producers_v;;

  };

  /**
     \class larcv::CropNeutrino3DFactory
     \brief A concrete factory class for larcv::CropNeutrino3D
  */
  class CropNeutrino3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    CropNeutrino3DProcessFactory() { ProcessFactory::get().add_factory("CropNeutrino3D", this); }
    /// dtor
    ~CropNeutrino3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new CropNeutrino3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

