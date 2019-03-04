/**
 * \file HistADC.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class HistADC
 *
 * @author taritree
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __HISTADC_H__
#define __HISTADC_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"

#include <vector>
#include "TH1D.h"
#include "TTree.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class HistADC ... these comments are used to generate
     doxygen documentation!
  */
  class HistADC : public ProcessBase {

  public:

    /// Default constructor
    HistADC(const std::string name = "HistADC");

    /// Default destructor
    ~HistADC() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    std::vector<TH1D*> m_hADC_v;
    std::string fImageProducer;
    std::string fParticleProducer;
    int fPlane0Thresh;
    int fPlane1Thresh;
    int fPlane2Thresh;

    TTree* m_tree;
    int m_plane;
    int m_npixels;
    float m_sum;
  };

  /**
     \class larcv::HistADCFactory
     \brief A concrete factory class for larcv::HistADC
  */
  class HistADCProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    HistADCProcessFactory() { ProcessFactory::get().add_factory("HistADC", this); }
    /// dtor
    ~HistADCProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new HistADC(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

