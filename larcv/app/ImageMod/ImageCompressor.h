/**
 * \file ImageCompressor.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class ImageCompressor
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __IMAGECOMPRESSOR_H__
#define __IMAGECOMPRESSOR_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/DataFormat/Image2D.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ImageCompressor ... these comments are used to generate
     doxygen documentation!
  */
  class ImageCompressor : public ProcessBase {

  public:

    /// Default constructor
    ImageCompressor(const std::string name = "ImageCompressor");

    /// Default destructor
    ~ImageCompressor() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::vector<std::string> _image_producer_v;
    std::vector<size_t     > _row_compression_v;
    std::vector<size_t     > _col_compression_v;
    std::vector<larcv::Image2D::CompressionModes_t> _mode_v;

  };

  /**
     \class larcv::ImageCompressorFactory
     \brief A concrete factory class for larcv::ImageCompressor
  */
  class ImageCompressorProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ImageCompressorProcessFactory() { ProcessFactory::get().add_factory("ImageCompressor", this); }
    /// dtor
    ~ImageCompressorProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ImageCompressor(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

