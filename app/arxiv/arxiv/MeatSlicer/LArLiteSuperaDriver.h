/**
 * \file LArLiteSuperaDriver.h
 *
 * \ingroup MeatSlicer
 * 
 * \brief Class def header for a class LArLiteSuperaDriver
 *
 * @author kazuhiro
 */

/** \addtogroup MeatSlicer

    @{*/

#ifndef LARLITE_LARLITESUPERADRIVER_H
#define LARLITE_LARLITESUPERADRIVER_H

#include "Analysis/ana_base.h"
#include "LArCVSuperaDriver.h"
namespace larlite {
  /**
     \class LArLiteSuperaDriver
     User custom analysis class made by SHELL_USER_NAME
   */
  class LArLiteSuperaDriver : public ana_base{
  
  public:

    /// Default constructor
    LArLiteSuperaDriver(){ _name="LArLiteSuperaDriver"; _fout=0;}

    /// Default destructor
    virtual ~LArLiteSuperaDriver(){}

    /** IMPLEMENT in LArLiteSuperaDriver.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in LArLiteSuperaDriver.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in LArLiteSuperaDriver.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    //larcv::LArCVSuperaDriver& CoreDriver() { return _supera; }

    void larcv_configure(std::string fname);

    void larcv_output_file(std::string fout);

  protected:
    ::larcv::LArCVSuperaDriver _supera;

  };
}
#endif

//**************************************************************************
// 
// For Analysis framework documentation, read Manual.pdf here:
//
// http://microboone-docdb.fnal.gov:8080/cgi-bin/ShowDocument?docid=3183
//
//**************************************************************************

/** @} */ // end of doxygen group 
