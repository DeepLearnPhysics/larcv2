/**
 * \file ROIMerger.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ROIMerger
 *
 * @author vgenty
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __ROIMERGER_H__
#define __ROIMERGER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ROIMerger ... these comments are used to generate
     doxygen documentation!
  */
  class ROIMerger : public ProcessBase {

  public:
    
    /// Default constructor
    ROIMerger(const std::string name="ROIMerger");
    
    /// Default destructor
    ~ROIMerger(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    const std::string& output_producer() const
    { return _output_roi_producer; }


  private:
    std::string _input_roi_producer;
    std::string _output_roi_producer;
    float _iou_threshold;
    
    void
    SearchScore(size_t this_id,
		size_t that_id,
		std::vector<std::vector<std::pair<size_t,float> > >& scores_pvv,
		std::vector<std::set<size_t> >& merge_id_vv,
		std::vector<bool>& seek_v);
    
    
  };

  /**
     \class larcv::ROIMergerFactory
     \brief A concrete factory class for larcv::ROIMerger
  */
  class ROIMergerProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ROIMergerProcessFactory() { ProcessFactory::get().add_factory("ROIMerger",this); }
    /// dtor
    ~ROIMergerProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ROIMerger(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

