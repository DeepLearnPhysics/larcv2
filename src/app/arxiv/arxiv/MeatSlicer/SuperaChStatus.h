/**
 * \file SuperaChStatus.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class SuperaChStatus
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SUPERACHSTATUS_H__
#define __SUPERACHSTATUS_H__
#include "DataFormat/ChStatus.h"
#include "SuperaBase.h"
#include "FMWKInterface.h"
#include "ParamsChStatus.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class SuperaChStatus ... these comments are used to generate
     doxygen documentation!
  */
  class SuperaChStatus : public SuperaBase,
			 public supera::ParamsChStatus {

  public:
    
    /// Default constructor
    SuperaChStatus(const std::string name="SuperaChStatus");
    
    /// Default destructor
    ~SuperaChStatus(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    bool is(const std::string question) const;

    void set_chstatus(::larcv::PlaneID_t plane, unsigned int wire, short status);

    const std::string& InputLabel() const { return _in_chstatus_producer; }

  private:
    std::string  _in_chstatus_producer;
    std::map<larcv::PlaneID_t,larcv::ChStatus> _status_m;
  };

  /**
     \class larcv::SuperaChStatusFactory
     \brief A concrete factory class for larcv::SuperaChStatus
  */
  class SuperaChStatusProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SuperaChStatusProcessFactory() { ProcessFactory::get().add_factory("SuperaChStatus",this); }
    /// dtor
    ~SuperaChStatusProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SuperaChStatus(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

