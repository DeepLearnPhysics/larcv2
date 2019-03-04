/**
 * \file CosmicTrackAna.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class CosmicTrackAna
 *
 * @author vgenty
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __COSMICTRACKANA_H__
#define __COSMICTRACKANA_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class CosmicTrackAna ... these comments are used to generate
     doxygen documentation!
  */
  class CosmicTrackAna : public ProcessBase {

  public:
    
    /// Default constructor
    CosmicTrackAna(const std::string name="CosmicTrackAna");
    
    /// Default destructor
    ~CosmicTrackAna(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();


  private:
    std::string _thrumu_px_prod;
    std::string _stopmu_px_prod;
    std::string _top_px_prod;
    std::string _bot_px_prod;
    std::string _up_px_prod;
    std::string _down_px_prod;
    std::string _anode_px_prod;
    std::string _cathode_px_prod;

    TTree* _tree;

    int _run;
    int _subrun;
    int _event;
    int _entry;

    int _n_thru_mu_trk;
    int _n_thru_mu_pix_p0;
    int _n_thru_mu_pix_p1;
    int _n_thru_mu_pix_p2;
    float _avg_thru_mu_pix;

    int _n_stop_mu_trk;
    int _n_stop_mu_pix_p0;
    int _n_stop_mu_pix_p1;
    int _n_stop_mu_pix_p2;
    float _avg_stop_mu_pix;

    int _n_top_pts;
    int _n_bot_pts;
    int _n_up_pts;
    int _n_down_pts;
    int _n_anode_pts;
    int _n_cathode_pts;

    void clear();
  };

  /**
     \class larcv::CosmicTrackAnaFactory
     \brief A concrete factory class for larcv::CosmicTrackAna
  */
  class CosmicTrackAnaProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    CosmicTrackAnaProcessFactory() { ProcessFactory::get().add_factory("CosmicTrackAna",this); }
    /// dtor
    ~CosmicTrackAnaProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new CosmicTrackAna(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

