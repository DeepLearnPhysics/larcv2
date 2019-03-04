/**
 * \file BatchFillerPPN.h
 *
 * \ingroup ThreadIO
 *
 * \brief Class def header for a class BatchFillerPPN
 *
 * @author kazuhiro
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __BATCHFILLERPIDLABEL_H__
#define __BATCHFILLERPIDLABEL_H__

#include "larcv/core/DataFormat/DataFormatTypes.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "BatchFillerTemplate.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class BatchFillerPPN ... these comments are used to generate
     doxygen documentation!
  */
  class BatchFillerPPN : public BatchFillerTemplate<float> {

  public:

    /// Default constructor
    BatchFillerPPN(const std::string name="BatchFillerPPN");

    /// Default destructor
    ~BatchFillerPPN(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void _batch_begin_();

    void _batch_end_();

    void finalize();

  private:

    enum ShapeType_t {kShower, kTrack};
    typedef larcv::PointType_t PointType_t;

    int _min_voxel_count;
		double _min_energy_deposit;
    std::string _tensor_producer;
    std::string _part_producer;
    ShapeType_t _shape_type;
    PointType_t _point_type;
    size_t _buffer_size;
    size_t _image_channel;
    std::vector<float> _entry_data;

  };

  /**
     \class larcv::BatchFillerPPNFactory
     \brief A concrete factory class for larcv::BatchFillerPPN
  */
  class BatchFillerPPNProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BatchFillerPPNProcessFactory() { ProcessFactory::get().add_factory("BatchFillerPPN",this); }
    /// dtor
    ~BatchFillerPPNProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new BatchFillerPPN(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group
