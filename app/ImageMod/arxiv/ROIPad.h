#ifndef __ROIPAD_H__
#define __ROIPAD_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include "DataFormat/ImageMeta.h"

namespace larcv {

  class ROIPad : public ProcessBase {

  public:
    
    ROIPad(const std::string name="ROIPad");
    ~ROIPad(){}
    void configure(const PSet&);
    void initialize();
    bool process(IOManager& mgr);
    void finalize();

    ImageMeta PadMeta(const ImageMeta& bb,
		      float row_pad,
		      float col_pad);
    

    
  private:
    
    std::string _img_producer;
    std::string _input_roi_producer;
    std::string _output_roi_producer;
    
    float _row_pad;
    float _col_pad;

    
  };

  class ROIPadProcessFactory : public ProcessFactoryBase {
  public:

    ROIPadProcessFactory() { ProcessFactory::get().add_factory("ROIPad",this); }
    ~ROIPadProcessFactory() {}
    ProcessBase* create(const std::string instance_name) { return new ROIPad(instance_name); }
    
  };

}

#endif
