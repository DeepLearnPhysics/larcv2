/**
 * \file BatchFillerSparseTensor3D.h
 *
 * \ingroup ThreadIO
 *
 * \brief Class def header for a class BatchFillerSparseTensor3D
 *
 * @author kazuhiro
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __BATCHFILLERSPARSETENSOR3D_H__
#define __BATCHFILLERSPARSETENSOR3D_H__

#include "larcv/core/Processor/ProcessFactory.h"
#include "BatchFillerTemplate.h"
#include "RandomCropper.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class BatchFillerTensor3D ... these comments are used to generate
     doxygen documentation!
  */
  class BatchFillerSparseTensor3D : public BatchFillerTemplate<float> {

  public:

    /// Default constructor
    BatchFillerSparseTensor3D(const std::string name="BatchFillerSparseTensor3D");

    /// Default destructor
    ~BatchFillerSparseTensor3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  protected:

    void _batch_begin_();
    void _batch_end_();

  private:

    size_t set_data_size(const EventSparseTensor3D& image_data);
    void assert_dimension(const EventSparseTensor3D& image_data) const;

    std::string _tensor3d_producer;
    size_t _max_voxels;
    float _unfilled_voxel_value;

    std::vector<float>  _entry_data;
    bool _allow_empty;
    bool _include_values;
    bool _augment;
  };

  /**
     \class larcv::BatchFillerSparseTensor3DFactory
     \brief A concrete factory class for larcv::BatchFillerSparseTensor3D
  */
  class BatchFillerSparseTensor3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BatchFillerSparseTensor3DProcessFactory() { ProcessFactory::get().add_factory("BatchFillerSparseTensor3D",this); }
    /// dtor
    ~BatchFillerSparseTensor3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) {
      return new BatchFillerSparseTensor3D(instance_name);
    }
  };

}

#endif
/** @} */ // end of doxygen group

