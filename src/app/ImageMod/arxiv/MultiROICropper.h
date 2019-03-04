/**
 * \file MultiROICropper.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class MultiROICropper
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __MULTIROICROPPER_H__
#define __MULTIROICROPPER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include "DataFormat/Image2D.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class MultiROICropper ... these comments are used to generate
     doxygen documentation!
  */
  class MultiROICropper : public ProcessBase {

  public:
    
    /// Default constructor
    MultiROICropper(const std::string name="MultiROICropper");
    
    /// Default destructor
    ~MultiROICropper(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    const larcv::Image2D& get_image() const { return _image; }

    const std::vector<larcv::Image2D>& get_cropped_image() const { return _cropped_v; }

    size_t target_rows() const { return _target_rows; }
    size_t target_cols() const { return _target_cols; }

  private:

    std::string _image_producer;
    std::string _roi_producer;
    size_t _target_rows;
    size_t _target_cols;
    size_t _target_ch;
    larcv::Image2D _image;
    std::vector<larcv::Image2D> _cropped_v;

  };

  /**
     \class larcv::MultiROICropperFactory
     \brief A concrete factory class for larcv::MultiROICropper
  */
  class MultiROICropperProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    MultiROICropperProcessFactory() { ProcessFactory::get().add_factory("MultiROICropper",this); }
    /// dtor
    ~MultiROICropperProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new MultiROICropper(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

