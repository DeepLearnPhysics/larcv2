/**
 * \file DBSCAN.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class DBSCAN
 *
 * @author twongjirad
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __DBSCAN_H__
#define __DBSCAN_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class DBSCAN ... these comments are used to generate
     doxygen documentation!
  */
  class DBSCAN : public ProcessBase {

  public:
    
    /// Default constructor
    DBSCAN(const std::string name="DBSCAN");
    
    /// Default destructor
    ~DBSCAN(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  protected:
    
    std::string ftpc_producer;
    std::vector<std::string> fauxdata_producers;
    std::vector<float> fauxdim_scale;

    std::string foutput_producer;
    std::vector<double> feps;
    std::vector<int> fminpoints;
    std::vector<double> fthreshold;

  };

  /**
     \class larcv::DBSCANFactory
     \brief A concrete factory class for larcv::DBSCAN
  */
  class DBSCANProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    DBSCANProcessFactory() { ProcessFactory::get().add_factory("DBSCAN",this); }
    /// dtor
    ~DBSCANProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new DBSCAN(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

