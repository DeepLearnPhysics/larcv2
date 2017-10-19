/**
 * \file SuperaKeyPointCluster.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class SuperaKeyPointCluster
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SUPERAKEYPOINTCLUSTER_H__
#define __SUPERAKEYPOINTCLUSTER_H__
#include "SuperaBase.h"
#include "FMWKInterface.h"
#include "ImageMetaMaker.h"
#include "ParamsPixel2D.h"
#include "DataFormat/EventPixel2D.h"
#include "DataFormat/Vertex.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class SuperaKeyPointCluster ... these comments are used to generate
     doxygen documentation!
  */
  class SuperaKeyPointCluster : public SuperaBase,
				public supera::ParamsPixel2D,
				public supera::ImageMetaMaker {

  public:
    
    /// Default constructor
    SuperaKeyPointCluster(const std::string name="SuperaKeyPointCluster");
    
    /// Default destructor
    ~SuperaKeyPointCluster(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    larcv::Vertex GetPoint(const supera::LArMCStep_t& step);
	
    std::vector<larcv::Pixel2DCluster>
    CreateCluster(const std::vector<larcv::ImageMeta>& meta_v,
		  const std::set<larcv::Vertex>& pt_s,
		  const unsigned short val,
		  const int time_offset);

    std::string _in_roi_label;
    bool   _apply_sce;
    size_t _row_pad;
    size_t _col_pad;
    bool _cluster_primary_start;
    bool _cluster_secondary_start;
    bool _cluster_scattering;

  };

  /**
     \class larcv::SuperaKeyPointClusterFactory
     \brief A concrete factory class for larcv::SuperaKeyPointCluster
  */
  class SuperaKeyPointClusterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SuperaKeyPointClusterProcessFactory() { ProcessFactory::get().add_factory("SuperaKeyPointCluster",this); }
    /// dtor
    ~SuperaKeyPointClusterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SuperaKeyPointCluster(instance_name); }
  };

}
#endif

/** @} */ // end of doxygen group 

