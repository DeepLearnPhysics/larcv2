#ifndef __UNIONROI_H__
#define __UNIONROI_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include "DataFormat/EventROI.h"
#include "DataFormat/ImageMeta.h"

namespace larcv {

  class UnionROI : public ProcessBase {

  public:

    UnionROI(const std::string name="UnionROI");
    ~UnionROI(){}

    void configure(const PSet&);
    void initialize() {}
    bool process(IOManager& mgr);
    void finalize() {}

    ROI Union(const std::vector<ROI>& roi_v);

  private:
    std::string _in_roi_producer;
    std::string _out_roi_producer;

  };

  class UnionROIProcessFactory : public ProcessFactoryBase {
  public:
    UnionROIProcessFactory() { ProcessFactory::get().add_factory("UnionROI",this); }
    ~UnionROIProcessFactory() {}
    ProcessBase* create(const std::string instance_name) { return new UnionROI(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

