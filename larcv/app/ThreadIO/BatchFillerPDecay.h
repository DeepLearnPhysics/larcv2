/**
 * \file BatchFillerPDecay.h
 *
 * \ingroup ThreadIO
 * 
 * \brief Class def header for a class BatchFillerPDecay
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
     User defined class BatchFillerPDecay ... these comments are used to generate
     doxygen documentation!
  */
  class BatchFillerPDecay : public BatchFillerTemplate<float> {

  public:
    
    /// Default constructor
    BatchFillerPDecay(const std::string name="BatchFillerPDecay");
    
    /// Default destructor
    ~BatchFillerPDecay(){}

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
     \class larcv::BatchFillerPDecayFactory
     \brief A concrete factory class for larcv::BatchFillerPDecay
  */
  class BatchFillerPDecayProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BatchFillerPDecayProcessFactory() { ProcessFactory::get().add_factory("BatchFillerPDecay",this); }
    /// dtor
    ~BatchFillerPDecayProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new BatchFillerPDecay(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

