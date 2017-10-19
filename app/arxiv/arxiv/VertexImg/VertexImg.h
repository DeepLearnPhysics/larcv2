/**
 * \file VertexImg.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class VertexImg
 *
 * @author taritree
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __VERTEXIMG_H__
#define __VERTEXIMG_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include <string>

namespace larcv {

  /**
     \class ProcessBase
     User defined class VertexImg ... these comments are used to generate
     doxygen documentation!
  */
  class VertexImg : public ProcessBase {

  public:
    
    /// Default constructor
    VertexImg(const std::string name="VertexImg");
    
    /// Default destructor
    ~VertexImg(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();


  protected:
    
    std::string fROIProducerName;
    std::string fImageProducerName;
    std::string fOutputProducerName;
    bool finplace;
    int fImageTickStart;
    int fVertexRadius;
    int fMarkValue;
  };

  /**
     \class larcv::VertexImgFactory
     \brief A concrete factory class for larcv::VertexImg
  */
  class VertexImgProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    VertexImgProcessFactory() { ProcessFactory::get().add_factory("VertexImg",this); }
    /// dtor
    ~VertexImgProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new VertexImg(instance_name); }

  };

}

#endif
/** @} */ // end of doxygen group 

