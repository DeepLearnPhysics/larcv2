// /**
//  * \file ParticleBoundaries.h
//  *
//  * \ingroup ImageMod
//  * 
//  * \brief Class def header for a class ParticleBoundaries
//  *
//  * @author kazuhiro
//  */

// /** \addtogroup ImageMod

//     @{*/
// #ifndef __PARTICLEBOUNDARIES_H__
// #define __PARTICLEBOUNDARIES_H__

// #include "larcv/core/Processor/ProcessBase.h"
// #include "larcv/core/Processor/ProcessFactory.h"
// namespace larcv {

//   /**
//      \class ProcessBase
//      User defined class ParticleBoundaries ... these comments are used to generate
//      doxygen documentation!
//   */
//   class ParticleBoundaries : public ProcessBase {

//   public:
    
//     /// Default constructor
//     ParticleBoundaries(const std::string name="ParticleBoundaries");
    
//     /// Default destructor
//     ~ParticleBoundaries(){}

//     void configure(const PSet&);

//     void initialize();

//     bool process(IOManager& mgr);

//     void finalize();

//   private:

//     std::string _cluster_producer;
//     std::string _particle_producer;
//     std::string _output_producer;
//     float _voxel_threshold;
    
//   };

//   /**
//      \class larcv::BBoxFromCluster3DFactory
//      \brief A concrete factory class for larcv::ParticleBoundaries
//   */
//   class ParticleBoundariesProcessFactory : public ProcessFactoryBase {
//   public:
//     /// ctor
//     ParticleBoundariesProcessFactory() { ProcessFactory::get().add_factory("ParticleBoundaries",this); }
//     /// dtor
//     ~ParticleBoundariesProcessFactory() {}
//     /// creation method
//     ProcessBase* create(const std::string instance_name) { return new ParticleBoundaries(instance_name); }
//   };

// }

// #endif
// /** @} */ // end of doxygen group 

