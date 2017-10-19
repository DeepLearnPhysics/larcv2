/**
 * \file DeadWireAna.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class DeadWireAna
 *
 * @author vgenty
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __DEADWIREANA_H__
#define __DEADWIREANA_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include "LArUtil/SpaceChargeMicroBooNE.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class DeadWireAna ... these comments are used to generate
     doxygen documentation!
  */
  class DeadWireAna : public ProcessBase {

  public:
    
    /// Default constructor
    DeadWireAna(const std::string name="DeadWireAna");
    
    /// Default destructor
    ~DeadWireAna(){}

    void configure(const PSet&);
    void initialize();
    bool process(IOManager& mgr);
    void finalize();

  private:

    TTree *_tree;

    int _run;
    int _subrun;
    int _event;
    int _entry;

    int _vertex_in_dead_plane0;
    int _vertex_in_dead_plane1;
    int _vertex_in_dead_plane2;

    int _vertex_near_dead_plane0;
    int _vertex_near_dead_plane1;
    int _vertex_near_dead_plane2;

    int _nearest_wire_error;

    std::string _ev_img2d_prod;
    std::string _seg_roi_prod;
    float _d_dead;

    larutil::SpaceChargeMicroBooNE _sce;
  };

  /**
     \class larcv::DeadWireAnaFactory
     \brief A concrete factory class for larcv::DeadWireAna
  */
  class DeadWireAnaProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    DeadWireAnaProcessFactory() { ProcessFactory::get().add_factory("DeadWireAna",this); }
    /// dtor
    ~DeadWireAnaProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new DeadWireAna(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

