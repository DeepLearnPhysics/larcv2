/**
 * \file BatchFillerVoxel3D.h
 *
 * \ingroup ThreadIO
 * 
 * \brief Class def header for a class BatchFillerVoxel3D
 *
 * @author kazuhiro
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __BATCHFILLERVOXEL3D_H__
#define __BATCHFILLERVOXEL3D_H__

#include "Processor/ProcessFactory.h"
#include "BatchFillerTemplate.h"
#include "RandomCropper.h"
#include "DataFormat/EventVoxel3D.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class BatchFillerVoxel3D ... these comments are used to generate
     doxygen documentation!
  */
  class BatchFillerVoxel3D : public BatchFillerTemplate<float> {

  public:
    
    /// Default constructor
    BatchFillerVoxel3D(const std::string name="BatchFillerVoxel3D");
    
    /// Default destructor
    ~BatchFillerVoxel3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  protected:

    void _batch_begin_();
    void _batch_end_();

  private:

    size_t set_data_size(const EventVoxel3D* image_data);
    void assert_dimension(const EventVoxel3D* image_data) const;

    std::string _voxel_producer;
    size_t _nx;
    size_t _ny;
    size_t _nz;
    std::vector<float>  _entry_data;

  };

  /**
     \class larcv::BatchFillerVoxel3DFactory
     \brief A concrete factory class for larcv::BatchFillerVoxel3D
  */
  class BatchFillerVoxel3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BatchFillerVoxel3DProcessFactory() { ProcessFactory::get().add_factory("BatchFillerVoxel3D",this); }
    /// dtor
    ~BatchFillerVoxel3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) {
      return new BatchFillerVoxel3D(instance_name);
    }
  };

}

#endif
/** @} */ // end of doxygen group 

