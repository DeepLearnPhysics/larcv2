/**
 * \file HiResImageDivider.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class HiResImageDivider
 *
 * @author twongjirad
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __HIRESIMAGEDIVIDER_H__
#define __HIRESIMAGEDIVIDER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include <string>
#include <vector>

#include "DataFormat/ImageMeta.h"
#include "DataFormat/EventROI.h"
#include "PMTWeights/PMTWireWeights.h"
#include "DivisionDef.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/Vertex.h"
namespace larcv {
  namespace hires {
    /**
       \class ProcessBase
       User defined class HiResImageDivider ... these comments are used to generate
       doxygen documentation!
    */
    class HiResImageDivider : public ProcessBase {
      
    public:
      
      /// Default constructor
      HiResImageDivider(const std::string name="HiResImageDivider");
      
      /// Default destructor
      ~HiResImageDivider(){}
      
      void configure(const PSet&);
      
      void initialize();
      
      bool process(IOManager& mgr);
      
      void finalize();

    protected:

      bool fApplySCE;
      bool fUseDivFile;
      std::string fDivisionFile;
      int fNumZdivisions;
      int fNumYdivisions;
      int fNumTdivisions;
      bool fOverlapDivisions;

      int fNPlanes;
      int fTickStart;
      int fTickPreCompression;
      int fWirePreCompression;
      int fMaxWireImageWidth;
      int fMaxWireInRegion;
      int fNumNonVertexDivisionsPerEvent;
      bool fCropSegmentation;
      bool fCropPMTWeighted;
      int fMaxRedrawAttempts;
      int fRedrawOnNEmptyPlanes;
      std::vector< int > fNumPixelRedrawThresh_v;
      std::vector< float > fInterestingPixelThresh_v;
      std::vector< larcv::hires::DivisionDef > m_divisions;

      std::string fInputPMTProducer;
      std::string fInputROIProducer;
      std::string fOutputROIProducer;
      std::string fInputImageProducer;
      std::string fInputSegmentationProducer;
      std::string fInputPMTWeightedProducer;
      std::string fOutputImageProducer;
      std::string fOutputSegmentationProducer;
      std::string fOutputPMTWeightedProducer;
      bool fDivideWholeImage;

      size_t fProcessedEvent;
      size_t fROISkippedEvent;
      size_t fProcessedROI;
      size_t fROISkipped;
      
      void ApplySCE(larcv::Vertex& vtx);
      bool isInteresting( const larcv::ROI& roi );
      bool isAbovePixelThreshold( const larcv::EventImage2D& imgs );
      int findVertexDivision( const larcv::ROI& roi );
      /*
      bool keepNonVertexDivision( const larcv::ROI& roi );
      */
      void cropEventImages( const larcv::EventImage2D& event_images, const larcv::hires::DivisionDef& div, larcv::EventImage2D& output_images  );

      void generateSingleCosmicDivision( std::vector< int >& divlist, const EventImage2D& input_event_images, larcv::ROI& roi );
      void generateSingleMCDivision( std::vector< int >& divlist, EventROI& event_roi, larcv::ROI& roi );
      void generateFitleredWholeImageDivision( std::vector< int >& divlist, const EventImage2D& input_event_images);

      // Methods for loading division definitions
      void LoadDivisionsFromFile();
      void CalculateDivisions();

    };
    
    /**
     \class larcv::HiResImageDividerFactory
     \brief A concrete factory class for larcv::HiResImageDivider
    */
    class HiResImageDividerProcessFactory : public ProcessFactoryBase {
    public:
      /// ctor
      HiResImageDividerProcessFactory() { ProcessFactory::get().add_factory("HiResImageDivider",this); }
      /// dtor
      ~HiResImageDividerProcessFactory() {}
      /// creation method
      ProcessBase* create(const std::string instance_name) { return new HiResImageDivider(instance_name); }
    };
  }
}

#endif

/** @} */ // end of doxygen group 

