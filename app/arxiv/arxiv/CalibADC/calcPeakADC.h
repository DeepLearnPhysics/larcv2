/**
 * \file calcPeakADC.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class calcPeakADC
 *
 * @author twongjirad
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __CALCPEAKADC_H__
#define __CALCPEAKADC_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include <string>
#include "TTree.h"
#include "TFile.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class calcPeakADC ... these comments are used to generate
     doxygen documentation!
  */
  class calcPeakADC : public ProcessBase {

  public:
    
    /// Default constructor
    calcPeakADC(const std::string name="calcPeakADC");
    
    /// Default destructor
    ~calcPeakADC(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    // params
    float fThreshold;
    float fDeadtime;
    int fNewCols;
    int fNewRows;
    bool fDebug;
    std::string fImageProducer;
    
    // ana output
    TTree* ttree;
    float peakmax;
    int wireid;
    int planeid;
    float width;
  };

  /**
     \class larcv::calcPeakADCFactory
     \brief A concrete factory class for larcv::calcPeakADC
  */
  class calcPeakADCProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    calcPeakADCProcessFactory() { ProcessFactory::get().add_factory("calcPeakADC",this); }
    /// dtor
    ~calcPeakADCProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new calcPeakADC(instance_name); }

  };

}

#endif
/** @} */ // end of doxygen group 

