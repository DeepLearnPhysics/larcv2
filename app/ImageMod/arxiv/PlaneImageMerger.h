/**
 * \file PlaneImageMerger.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class PlaneImageMerger
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __PLANEIMAGEMERGER_H__
#define __PLANEIMAGEMERGER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include <set>

namespace larcv {

  /**
     \class ProcessBase
     User defined class PlaneImageMerger ... these comments are used to generate
     doxygen documentation!
  */
  class PlaneImageMerger : public ProcessBase {

  public:
    
    /// Default constructor
    PlaneImageMerger(const std::string name="PlaneImageMerger");
    
    /// Default destructor
    ~PlaneImageMerger(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _producer_key;
    std::string _output_producer;
    std::set<size_t> _plane_s;
  };

  /**
     \class larcv::PlaneImageMergerFactory
     \brief A concrete factory class for larcv::PlaneImageMerger
  */
  class PlaneImageMergerProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    PlaneImageMergerProcessFactory() { ProcessFactory::get().add_factory("PlaneImageMerger",this); }
    /// dtor
    ~PlaneImageMergerProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new PlaneImageMerger(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

