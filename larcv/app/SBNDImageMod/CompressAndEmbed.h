/**
 * \file CompressAndEmbed.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class CompressAndEmbed
 *
 * @author cadams
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __COMPRESSANDEMBED_H__
#define __COMPRESSANDEMBED_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/DataFormat/Image2D.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class CompressAndEmbed ... these comments are used to generate
     doxygen documentation!
  */
  class CompressAndEmbed : public ProcessBase {

  public:

    /// Default constructor
    CompressAndEmbed(const std::string name = "CompressAndEmbed");

    /// Default destructor
    ~CompressAndEmbed() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::vector<std::string> _producer_v;
    std::vector<std::string> _data_type_v;
    std::vector<size_t     > _row_compression_v;
    std::vector<size_t     > _col_compression_v;
    std::vector<size_t     > _output_rows_v;
    std::vector<size_t     > _output_cols_v;
    std::vector<size_t     > _mode_v;

  };

  /**
     \class larcv::CompressAndEmbedFactory
     \brief A concrete factory class for larcv::CompressAndEmbed
  */
  class CompressAndEmbedProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    CompressAndEmbedProcessFactory() { ProcessFactory::get().add_factory("CompressAndEmbed", this); }
    /// dtor
    ~CompressAndEmbedProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new CompressAndEmbed(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

