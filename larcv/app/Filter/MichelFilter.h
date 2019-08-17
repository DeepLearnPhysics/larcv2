/**
 * \file MichelFilter.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class MichelFilter
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __EVENTIDFILTER_H__
#define __EVENTIDFILTER_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class MichelFilter ... these comments are used to generate
     doxygen documentation!
  */
  class MichelFilter : public ProcessBase {

  public:
    
    /// Default constructor
    MichelFilter(const std::string name="MichelFilter");
    
    /// Default destructor
    ~MichelFilter(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _cluster_label;
    std::string _particle_label;
    double _min_energy_init;
    double _min_energy_dep;
    double _min_voxel_sum;
    size_t _min_voxel_count;
    size_t _min_dist_x;
    size_t _min_dist_y;
    size_t _min_dist_z;

  };

  /**
     \class larcv::MichelFilterFactory
     \brief A concrete factory class for larcv::MichelFilter
  */
  class MichelFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    MichelFilterProcessFactory() { ProcessFactory::get().add_factory("MichelFilter",this); }
    /// dtor
    ~MichelFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new MichelFilter(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

