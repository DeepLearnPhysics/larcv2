/**
 * \file DataStream.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class DataStream
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __DataSTREAM_H__
#define __DataSTREAM_H__

#include "ImageHolder.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class DataStream ... these comments are used to generate
     doxygen documentation!
  */
  class DataStream : public ImageHolder {

  public:
    
    /// Default constructor
    DataStream(const std::string name="DataStream");
    
    /// Default destructor
    ~DataStream(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _tpc_image_producer;
    std::string _pmt_image_producer;
    std::string _ch_status_producer;
    std::string _roi_producer;
    std::string _segment_producer;
    
  };

  /**
     \class larcv::DataStreamFactory
     \brief A concrete factory class for larcv::DataStream
  */
  class DataStreamProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    DataStreamProcessFactory() { ProcessFactory::get().add_factory("DataStream",this); }
    /// dtor
    ~DataStreamProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new DataStream(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

