/**
 * \file ResizeImage.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ResizeImage
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __RESIZEIMAGE_H__
#define __RESIZEIMAGE_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ResizeImage ... these comments are used to generate
     doxygen documentation!
  */
  class ResizeImage : public ProcessBase {

  public:
    
    /// Default constructor
    ResizeImage(const std::string name="ResizeImage");
    
    /// Default destructor
    ~ResizeImage(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _input_producer;
    std::string _output_producer;
    std::vector<double> _origin_x_v;
    std::vector<double> _origin_y_v;
    std::vector<double> _width_v;
    std::vector<double> _height_v;
    std::vector<size_t> _rows_v;
    std::vector<size_t> _cols_v;

  };

  /**
     \class larcv::ResizeImageFactory
     \brief A concrete factory class for larcv::ResizeImage
  */
  class ResizeImageProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ResizeImageProcessFactory() { ProcessFactory::get().add_factory("ResizeImage",this); }
    /// dtor
    ~ResizeImageProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ResizeImage(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

