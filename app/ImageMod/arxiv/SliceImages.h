/**
 * \file SliceImages.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class SliceImages
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SLICEIMAGES_H__
#define __SLICEIMAGES_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class SliceImages ... these comments are used to generate
     doxygen documentation!
  */
  class SliceImages : public ProcessBase {

  public:
    
    /// Default constructor
    SliceImages(const std::string name="SliceImages");
    
    /// Default destructor
    ~SliceImages(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::vector<size_t> _channels;
    std::string _image_producer;
    std::string _output_producer;
  };

  /**
     \class larcv::SliceImagesFactory
     \brief A concrete factory class for larcv::SliceImages
  */
  class SliceImagesProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SliceImagesProcessFactory() { ProcessFactory::get().add_factory("SliceImages",this); }
    /// dtor
    ~SliceImagesProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SliceImages(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

