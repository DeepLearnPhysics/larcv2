/**
 * \file Segment2DFromPDG.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class Segment2DFromPDG
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __SEGMENT2DFROMPDG_H__
#define __SEGMENT2DFROMPDG_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/DataFormat/Image2D.h"
#include "larcv/core/DataFormat/Voxel2D.h"
#include "larcv/core/DataFormat/Particle.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class Segment2DFromPDG ... these comments are used to generate
     doxygen documentation!
  */
  class Segment2DFromPDG : public ProcessBase {

  public:
    
    /// Default constructor
    Segment2DFromPDG(const std::string name="Segment2DFromPDG");
    
    /// Default destructor
    ~Segment2DFromPDG(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    Image2D
    CreateWeight2D(const larcv::ClusterPixel2D& cluster_v,
                   const larcv::Image2D& input_image);
    Image2D
    CreateSegment2D(const  larcv::ClusterPixel2D& cluster_v,
                    const  larcv::ParticleSet& particle_v);
    enum LabelType_t {kOverwrite, kPixelValue};
    LabelType_t _label_type;
    std::string _output_producer  ;
    std::string _label_producer   ;
    std::string _weight_producer  ;
    std::string _input_producer   ;
    std::string _cluster_producer ;
    std::string _particle_producer;

    size_t _dist_surrounding;
    std::vector<size_t> _projection_v;
    std::vector<float> _boundary_data;
    std::vector<std::vector<int> > _pdg_list;
  };

  /**
     \class larcv::Segment2DFromPDGFactory
     \brief A concrete factory class for larcv::Segment2DFromPDG
  */
  class Segment2DFromPDGProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    Segment2DFromPDGProcessFactory() { ProcessFactory::get().add_factory("Segment2DFromPDG",this); }
    /// dtor
    ~Segment2DFromPDGProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Segment2DFromPDG(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

