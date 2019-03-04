/**
 * \file BatchFillerPIDLabel.h
 *
 * \ingroup ThreadIO
 * 
 * \brief Class def header for a class BatchFillerPIDLabel
 *
 * @author kazuhiro
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef __BATCHFILLERPIDLABEL_H__
#define __BATCHFILLERPIDLABEL_H__

#include "larcv/core/Processor/ProcessFactory.h"
#include "BatchFillerTemplate.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class BatchFillerPIDLabel ... these comments are used to generate
     doxygen documentation!
  */
  class BatchFillerPIDLabel : public BatchFillerTemplate<float> {

  public:
    
    /// Default constructor
    BatchFillerPIDLabel(const std::string name="BatchFillerPIDLabel");
    
    /// Default destructor
    ~BatchFillerPIDLabel(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void _batch_begin_();

    void _batch_end_();

    void finalize();

  private:
    std::string _part_producer;
    std::vector<float> _entry_data;
    size_t _num_class;
    std::vector<int> _pdg_list;
  };

  /**
     \class larcv::BatchFillerPIDLabelFactory
     \brief A concrete factory class for larcv::BatchFillerPIDLabel
  */
  class BatchFillerPIDLabelProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BatchFillerPIDLabelProcessFactory() { ProcessFactory::get().add_factory("BatchFillerPIDLabel",this); }
    /// dtor
    ~BatchFillerPIDLabelProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new BatchFillerPIDLabel(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

