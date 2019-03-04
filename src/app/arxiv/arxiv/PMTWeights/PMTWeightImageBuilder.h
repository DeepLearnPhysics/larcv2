/**
 * \file PMTWeightImageBuilder.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class PMTWeightImageBuilder
 *
 * @author twongjirad
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __PMTWEIGHTIMAGEBUILDER_H__
#define __PMTWEIGHTIMAGEBUILDER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include <string>
#include <map>

#include "PMTWireWeights.h"

namespace larcv {
  namespace pmtweights {
    /**
       \class ProcessBase
       User defined class PMTWeightImageBuilder ... these comments are used to generate
       doxygen documentation!
    */
    class PMTWeightImageBuilder : public ProcessBase {
      
    public:
      
      /// Default constructor
      PMTWeightImageBuilder(const std::string name="PMTWeightImageBuilder");
      
      /// Default destructor
      ~PMTWeightImageBuilder();
      
      void configure(const PSet&);
      
      void initialize();
      
      bool process(IOManager& mgr);
      
      void finalize();

      std::string fGeoFile;
      std::string fPMTImageProducer;
      std::string fTPCImageProducer;
      std::string fOutputProducerName;
      int fNWirePixels;
      int fStartTick;
      int fEndTick;
      PMTWireWeights* m_WireWeights;
      bool fCheckSat;
      int fPMTImageIndex;
      float fHGpedestal;
      bool fSaveWeightMask;
      std::string fWeightMaskProducerName;
      bool fDebugSaveDistWeightMatrix;

      bool fResizedMat;
      std::map< int, larcv::Image2D> resizedWeightMat;

    };

    /**
       \class larcv::PMTWeightImageBuilderFactory
       \brief A concrete factory class for larcv::PMTWeightImageBuilder
    */
    class PMTWeightImageBuilderProcessFactory : public ProcessFactoryBase {
    public:
      /// ctor
      PMTWeightImageBuilderProcessFactory() { ProcessFactory::get().add_factory("PMTWeightImageBuilder",this); }
      /// dtor
      ~PMTWeightImageBuilderProcessFactory() {}
      /// creation method
      ProcessBase* create(const std::string instance_name) { return new PMTWeightImageBuilder(instance_name); }
    };
  }
}

#endif
/** @} */ // end of doxygen group 

