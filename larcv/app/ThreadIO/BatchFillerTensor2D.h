/**
 * \file BatchFillerTensor2D.h
 *
 * \ingroup ThreadIO
 *
 * \brief Class def header for a class BatchFillerTensor2D
 *
 * @author kazuhiro
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __BATCHFILLERTENSOR2D_H__
#define __BATCHFILLERTENSOR2D_H__

#include "larcv/core/Processor/ProcessFactory.h"
#include "BatchFillerTemplate.h"
#include "RandomCropper.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class BatchFillerTensor2D ... these comments are used to generate
     doxygen documentation!
  */
  class BatchFillerTensor2D : public BatchFillerTemplate<float> {

  public:

    /// Default constructor
    BatchFillerTensor2D(const std::string name="BatchFillerTensor2D");

    /// Default destructor
    ~BatchFillerTensor2D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  protected:

    void _batch_begin_();
    void _batch_end_();

  private:

    size_t set_data_size(const EventSparseTensor2D& image_data);
    void assert_dimension(const EventSparseTensor2D& image_data) const;

    std::string _tensor2d_producer;
    size_t _rows;
    size_t _cols;
    size_t _num_channels;
    std::vector<size_t> _slice_v;
    size_t _max_ch;

    std::vector<float>  _entry_data;
    size_t _num_channel;
    float _voxel_base_value;
    bool _allow_empty;
  };

  /**
     \class larcv::BatchFillerTensor2DFactory
     \brief A concrete factory class for larcv::BatchFillerTensor2D
  */
  class BatchFillerTensor2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BatchFillerTensor2DProcessFactory() { ProcessFactory::get().add_factory("BatchFillerTensor2D",this); }
    /// dtor
    ~BatchFillerTensor2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) {
      return new BatchFillerTensor2D(instance_name);
    }
  };

}

#endif
/** @} */ // end of doxygen group

