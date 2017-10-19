#ifndef __HIRESIMAGEDIVIDER_CXX__
#define __HIRESIMAGEDIVIDER_CXX__

#include "HiResImageDivider.h"
#include "DataFormat/EventImage2D.h"
#include "LArUtil/SpaceChargeMicroBooNE.h"
#include "TFile.h"
#include "TTree.h"

#include "DividerAlgo.h"

#include <set>
#include <random>
#include <iostream>

namespace larcv {
  namespace hires {
    static HiResImageDividerProcessFactory __global_HiResImageDividerProcessFactory__;
    
    HiResImageDivider::HiResImageDivider(const std::string name)
      : ProcessBase(name)
    {}

    void HiResImageDivider::ApplySCE(larcv::Vertex& vtx)
    {
      static larutil::SpaceChargeMicroBooNE sce;
      auto pos = sce.GetPosOffsets(vtx.X(), vtx.Y(), vtx.Z());
      vtx.Reset( (vtx.X() - pos[0] + 0.7),
		 (vtx.Y() + pos[1]),
		 (vtx.Z() + pos[2]),
		 vtx.T());
    }
    
    void HiResImageDivider::configure(const PSet& cfg)
    {
      fApplySCE           = cfg.get<bool>("ApplySCE",true);   // apply space-charge-effect correction
      fUseDivFile         = cfg.get<bool>("UseDivFile",true);       // load division definitions from file
      if ( !fUseDivFile ) {
	fDivisionFile       = cfg.get<std::string>("DivisionFile",""); // division file [not used]
	fNumZdivisions      = cfg.get<int>("NumZdivisions");           // number of pieces to divide Z dimension [must define]
	fNumYdivisions      = cfg.get<int>("NumYdivisions");           // number of pieces to divide Y dimension [must define]
	fNumTdivisions      = cfg.get<int>("NumTdivisions");           // number of pieces to divide T dimension [must define]
	fOverlapDivisions   = cfg.get<bool>("OverlapDivisions",false); // also make overlapping divisions that are offset by half-widths
      }
      else {
	fDivisionFile       = cfg.get<std::string>("DivisionFile");      // division file [must have this]
	fNumZdivisions      = cfg.get<int>("NumZdivisions",1);           // number of pieces to divide Z dimension [not used]
	fNumYdivisions      = cfg.get<int>("NumYdivisions",1);           // number of pieces to divide Y dimension [not used]
	fNumTdivisions      = cfg.get<int>("NumTdivisions",1);           // number of pieces to divide T dimension [not used]
	fOverlapDivisions   = cfg.get<bool>("OverlapDivisions",false); // also make overlapping divisions that are offset by half-widths	
      }

      fNPlanes            = cfg.get<int>( "NPlanes", 3 );
      fTickStart          = cfg.get<int>( "TickStart", 2400 );
      fTickPreCompression = cfg.get<int>( "TickPreCompression", 6 );
      fWirePreCompression = cfg.get<int>( "WirePreCompression", 1 );
      fMaxWireImageWidth  = cfg.get<int>( "MaxWireImageWidth" );
      fInputPMTProducer   = cfg.get<std::string>( "InputPMTProducer" );
      fInputROIProducer   = cfg.get<std::string>( "InputROIProducer" );
      fOutputROIProducer  = cfg.get<std::string>( "OutputROIProducer" );
      fNumNonVertexDivisionsPerEvent = cfg.get<int>( "NumNonVertexDivisionsPerEvent" );
      fInputImageProducer = cfg.get<std::string>( "InputImageProducer" );
      fOutputImageProducer = cfg.get<std::string>( "OutputImageProducer" );
      fInputSegmentationProducer = cfg.get<std::string>( "InputSegmentationProducer" );
      fOutputSegmentationProducer = cfg.get<std::string>( "OutputSegmentationProducer" );
      fInputPMTWeightedProducer = cfg.get<std::string>( "InputPMTWeightedProducer" );
      fOutputPMTWeightedProducer = cfg.get<std::string>( "OutputPMTWeightedProducer" );
      fCropSegmentation = cfg.get<bool>( "CropSegmentation" );
      fCropPMTWeighted  = cfg.get<bool>( "CropPMTWeighted" );
      fNumPixelRedrawThresh_v = cfg.get< std::vector<int> >( "NumPixelRedrawThresh" );
      fInterestingPixelThresh_v = cfg.get< std::vector<float> >( "InterestingPixelThresh" );
      fRedrawOnNEmptyPlanes = cfg.get<int>("RedrawOnNEmptyPlanes",2);
      fMaxRedrawAttempts = cfg.get<int>("MaxRedrawAttempts");
      fDivideWholeImage = cfg.get<bool>("DivideWholeImage");
    }
    
    void HiResImageDivider::initialize()
    {
      // Here we load the division definitions
      // We can use a pre-calculated divisions from file, or recalculate them using the DividerAlgo.
      if ( fUseDivFile )
	LoadDivisionsFromFile();
      else
	CalculateDivisions();

      fProcessedEvent=0;
      fROISkippedEvent=0;
      fProcessedROI=0;
      fROISkipped=0;
    }
    
    bool HiResImageDivider::process(IOManager& mgr)
    {
      ++fProcessedEvent;
      // This processor does the following:
      // 1) read in hi-res images (from producer specified in config)
      // 2) (how to choose which one we clip?)

      larcv::EventImage2D input_event_images = *((larcv::EventImage2D*)(mgr.get_data(kProductImage2D,fInputImageProducer)));
      larcv::EventImage2D* input_seg_images;
      larcv::EventROI event_roi;

      // If it exists, we get the ROI which will guide us on how to use the image
      // This does not exist for cosmics, in which case we create
      ProducerID_t roi_producer_id = mgr.producer_id(::larcv::kProductROI,fInputROIProducer);
      larcv::ROI roi;
      if(roi_producer_id != kINVALID_PRODUCER) {
	LARCV_INFO() << "ROI by producer " << fInputROIProducer << " found. Searching for MC ROI..." << std::endl;
	event_roi = *((larcv::EventROI*)(mgr.get_data(roi_producer_id)));
	if(!fInputSegmentationProducer.empty())
	  input_seg_images = (larcv::EventImage2D*)(mgr.get_data(kProductImage2D,fInputSegmentationProducer));
      }else{
	LARCV_INFO() << "ROI by producer " << fInputROIProducer << " not found. Constructing Cosmic ROI..." << std::endl;
	// Input ROI did not exist. Assume this means cosmics and create one
	roi.Type(kROICosmic);
      }

      // First we decide what divisions need to be cropped
      std::vector<int> divlist;

      if ( fDivideWholeImage ) {
	generateFitleredWholeImageDivision( divlist, input_event_images );
      }
      else {
	if ( roi.Type()==kROICosmic )
	  generateSingleCosmicDivision( divlist, input_event_images, roi );
	else
	  generateSingleMCDivision( divlist, event_roi, roi );

	if(!isInteresting(roi)) {
	  LARCV_CRITICAL() << "Did not find any interesting ROI and/or failed to construct Cosmic ROI..." << std::endl;
	  if(roi_producer_id != kINVALID_PRODUCER) {
	    LARCV_ERROR() << "Input ROI does exist. Looping over ROI types and printing out..." << std::endl;
	    for(auto const& roi : event_roi.ROIArray()) LARCV_ERROR() << roi.dump();
	    LARCV_ERROR() << "Dump finished..." << std::endl;
	  }
	  // Return false not to store this event in case of filter IO mode
	  return false;
	}
      }

      // We call IOManger::store_entry which calls clear_data of all products.
      // To keep necessary input event information, let's copy-construct them
      
      auto const& event_id = mgr.event_id();
      const size_t input_run    = event_id.run();
      const size_t input_subrun = event_id.subrun();
      const size_t input_event  = event_id.event();
      bool store_entry = false;
      LARCV_INFO() << "Reading-in (run,subrun,event) = (" << input_run << "," << input_subrun << "," << input_event << ")" << std::endl;
      
      // now we loop through and make divisions
      for ( auto const& idiv : divlist ) {
	
	if ( idiv==-1 )
	  continue;

	larcv::hires::DivisionDef const& vertex_div = m_divisions.at( idiv );

	//std::cout << "Vertex in ROI: " << roi.X() << ", " << roi.Y() << ", " << roi.Z() << std::endl;
	
	// now we crop out certain pieces
	// The input images
	auto output_event_images = (larcv::EventImage2D*)(mgr.get_data( kProductImage2D,fOutputImageProducer) );
	output_event_images->clear();
	LARCV_DEBUG() << "Crop " << fInputImageProducer << " Images." << std::endl;
	cropEventImages( input_event_images, vertex_div, *output_event_images );

	larcv::EventImage2D* output_seg_images = nullptr;
	larcv::EventImage2D* output_pmtweighted_images = nullptr;
	//
	// Image is cropped based on DivisionDef which is found from ROI's vertex
	// However ROI's vertex do not necessarily overlap with the same ROI's 2D bounding box
	// in case of a neutrino interaction because the former is a neutrino interaction vertex
	// while the latter is based on particles' trajectories that deposited energy. An example
	// is a neutron produced at vertex and hit proton far away from the vertex. So, here, we
	// ask, if it is non-cosmic type ROI, created image's meta overlaps with ROI's image meta.
	//
	if(roi.Type() != kROICosmic) {
	  // if not cosmic, we check if neutrino image has enough interesting pixels
	  if ( !isAbovePixelThreshold( *output_event_images ) ) {
	    ++fROISkippedEvent;
	    LARCV_NORMAL() << "Found an event w/ neutrino vertex without enough interesting pixels (" << fROISkippedEvent << " events skipped so far)" << std::endl;
	    for(auto const& img : output_event_images->Image2DArray()) LARCV_INFO() << img.meta().dump();
	    for(auto const& aroi : event_roi.ROIArray()) LARCV_INFO() << aroi.dump();
	    output_event_images->clear();
	    continue;
	  }
	  try{
	    for(auto const& img : output_event_images->Image2DArray())
	      roi.BB(img.meta().plane()).overlap(img.meta());
	  }catch(const larbys& err) {
	    ++fROISkippedEvent;
	    LARCV_NORMAL() << "Found an event w/ neutrino vertex not within ROI bounding box (" << fROISkippedEvent << " events so far)" << std::endl;
	    for(auto const& img : output_event_images->Image2DArray()) LARCV_INFO() << img.meta().dump();
	    for(auto const& aroi : event_roi.ROIArray()) LARCV_INFO() << aroi.dump();
	    output_event_images->clear();
	    continue;
	  }
	}

	// Output Segmentation
	if ( fCropSegmentation && input_seg_images->Image2DArray().size() ) {
	  // the semantic segmentation is only filled in the neighboor hood of the interaction
	  // we overlay it into a full image (and then crop out the division)
	  larcv::EventImage2D full_seg_images;
	  for ( unsigned int p=0; p<3; p++ ) {
	    larcv::Image2D const& img = input_event_images.at( p ); 
	    larcv::ImageMeta seg_image_meta( img.meta().width(), img.meta().height(),
					     img.meta().rows(), img.meta().cols(),
					     img.meta().min_x(), img.meta().max_y(),
					     img.meta().plane() );
	    larcv::Image2D seg_image( seg_image_meta );
	    seg_image.paint( 0.0 );
	    seg_image.overlay( input_seg_images->at(p) );
	    full_seg_images.Emplace( std::move(seg_image) );
	  }
	  LARCV_DEBUG() << "Crop " << fInputSegmentationProducer << " Images." << std::endl;
	  output_seg_images = (larcv::EventImage2D*)(mgr.get_data(kProductImage2D,fOutputSegmentationProducer));
	  output_seg_images->clear();
	  cropEventImages( full_seg_images, vertex_div, *output_seg_images );
	  
	}// if crop seg
      
	// Output PMT weighted
	if ( fCropPMTWeighted )  {
	  LARCV_DEBUG() << "Load " << fInputPMTWeightedProducer << " Images." << std::endl;
	  larcv::EventImage2D input_pmtweighted_images = *((larcv::EventImage2D*)(mgr.get_data(kProductImage2D,fInputPMTWeightedProducer)));
	  LARCV_DEBUG() << "Allocate " << fOutputPMTWeightedProducer << " Images." << std::endl;
	  output_pmtweighted_images = (larcv::EventImage2D*)(mgr.get_data(kProductImage2D,fOutputPMTWeightedProducer));
	  output_pmtweighted_images->clear();
	  LARCV_DEBUG() << "Crop " << fInputPMTWeightedProducer << " Images." << std::endl;
	  cropEventImages( input_pmtweighted_images, vertex_div, *output_pmtweighted_images );	
	}

	// Finally let's store ROI w/ updated ImageMeta arrays
	//
	// 0) Retrieve output image array and input ROI array (for the latter "if exists")
	// 1) Loop over ROI array (or single ROI for "cosmic" = input does not exist), ask overlap in 2D plane ImageMeta with each image
	auto output_rois = (larcv::EventROI*)(mgr.get_data(kProductROI,fOutputROIProducer));
	output_rois->clear();
	
	if(roi_producer_id != kINVALID_PRODUCER) {
	  // Loop over and store in output
	  
	  for(auto const& aroi : event_roi.ROIArray()) {
	    ++fProcessedROI;
	    std::vector<larcv::ImageMeta> out_meta_v;
	    try {
	      //LARCV_INFO() << "Creating particle ROI for: " << roi.dump() << std::endl;
	      for(auto const& bb : aroi.BB()) {
		auto const& img_meta = output_event_images->at(bb.plane()).meta();
		out_meta_v.push_back(img_meta.overlap(bb));
	      }
	    }catch(const larbys& err){
	      LARCV_INFO() << "Found an ROI bounding box that has no overlap with neutrino vertex box. Skipping..." << std::endl;
	      LARCV_INFO() << aroi.dump() << std::endl;
	      out_meta_v.clear();
	      ++fROISkipped;
	    }
	  
	    ::larcv::ROI out_roi(aroi);
	    out_roi.SetBB(out_meta_v);	    
	    output_rois->Emplace(std::move(out_roi));
	  }
	}else{
	  std::vector<larcv::ImageMeta> out_meta_v;
	  for(auto const& img : output_event_images->Image2DArray()) out_meta_v.push_back(img.meta());
	  roi.SetBB(out_meta_v);
	  output_rois->Emplace(std::move(roi));
	}

	if(output_rois->ROIArray().empty()) {
	  output_event_images->clear();
	  if(output_pmtweighted_images) output_pmtweighted_images->clear();
	  if(output_seg_images) output_seg_images->clear();	  
	}else{
	  store_entry = true;

	  if(this->event_creator() && store_entry) {
	    mgr.set_id(input_run,input_subrun,input_event);
	    LARCV_INFO() << "Storing entry for a division..." << std::endl;
	    mgr.save_entry();
	  }
	}

      }//end of divlist loop
      LARCV_INFO() << "Done storing divisions" << std::endl;
      return store_entry;
    }
    
    void HiResImageDivider::finalize()
    {
      LARCV_NORMAL() << "Skipped events due to vertex-box not overlapping with ROI: " << fROISkippedEvent << " / " << fProcessedEvent << std::endl;
      LARCV_NORMAL() << "Skipped ROI due to not within vertex-box: " << fROISkipped << " / " << fProcessedROI << std::endl;
    }

    // -------------------------------------------------------
    // Division list generators

    void HiResImageDivider::generateSingleCosmicDivision( std::vector< int >& divlist, const EventImage2D& input_event_images, larcv::ROI& roi ) {
      // we randomly pick a division (by randomly drawing a position in the detector
      // we return a division which satisfies some minimum amount of interesting pixel threshold
      bool viewok = false;
      int ntries = 0;

      roi.Type(kROICosmic);
	
      int idiv = -1;

      while (!viewok && ntries<fMaxRedrawAttempts) {
	ntries++;
	// FIXME: need a way to get detector dimension somehow...
	const double zmin = 0;
	const double zmax = 1036.0;
	const double ymin = -116.;
	const double ymax = 116.;
	const double xmin = 0.;
	const double xmax = 255.;
	const double tmin = 3125.; // in ns
	const double tmax = tmin + 1600.;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.,1.);
	roi.Position( dis(gen) * (xmax - xmin) + xmin,
		      dis(gen) * (ymax - ymin) + ymin,
		      dis(gen) * (zmax - zmin) + zmin,
		      dis(gen) * (tmax - tmin) + tmin);
	
	idiv = findVertexDivision( roi );
	if ( idiv==-1 ) {
	  viewok = false;
	  continue;
	}
	larcv::hires::DivisionDef const& vertex_div = m_divisions.at( idiv );
	larcv::EventImage2D cosmic_test;
	cropEventImages( input_event_images, vertex_div, cosmic_test );
	if ( isAbovePixelThreshold( cosmic_test ) )  {
	  viewok = true;
	  break;
	}
      }
      if ( !viewok ) {
	LARCV_ERROR() << "could not find cosmic roi with enough interesting pixels.\n" << std::endl;
      }
      else {
	divlist.push_back(idiv);
      }
    }

    void HiResImageDivider::generateSingleMCDivision( std::vector< int >& divlist, EventROI& event_roi, larcv::ROI& roi ) {
      int idiv = -1;
      if(!event_roi.ROIArray().empty()){
	LARCV_INFO() << "ROI by producer " << fInputROIProducer << " found. Searching for MC ROI..." << std::endl;
	for ( auto const& aroi : event_roi.ROIArray() ) 
	  if ( isInteresting(aroi) ) { roi = aroi; break; }

	idiv = findVertexDivision( roi );
      }
      
      if ( idiv==-1 ) {
	LARCV_ERROR() << "No divisions were found that contained an event vertex.\n" << roi.dump() << std::endl;
      }
      else {
	divlist.push_back(idiv);
      }
    }
    
    void HiResImageDivider::generateFitleredWholeImageDivision( std::vector< int >& divlist, const EventImage2D& input_event_images) {
      // we loop through all divisions and make a test crop. we then test this cropped region if it
      // satisfies the conditions to be deemed interesting enough to save
      divlist.clear();
      for ( int idiv=0; idiv<(int)m_divisions.size(); idiv++ ) {
	// div is a larcv::hires::DivisionDef
	larcv::hires::DivisionDef const& div = m_divisions.at(idiv);
	larcv::EventImage2D cropped;
	cropEventImages( input_event_images, div, cropped );
	if ( isAbovePixelThreshold( cropped ) )
	  divlist.push_back( idiv );
      }
      LARCV_INFO() << "Generated " << divlist.size() << " / " << m_divisions.size() << " divisions..." << std::endl;
    }

    // -------------------------------------------------------

    bool HiResImageDivider::isInteresting( const larcv::ROI& roi ) {
      // Supposed to return the "primary" 
      //return (roi.Type() == kROIBNB || roi.Type() == kROICosmic);
      // Consider if this came from MCTruth: MCSTIndex should be kINVALID_USHORT
      return (roi.MCSTIndex() == kINVALID_USHORT);
    }
    
    bool HiResImageDivider::isAbovePixelThreshold( const larcv::EventImage2D& imgs ) {
      // warning, vectors assume plane ids are sequential and have no gaps
      std::vector<int> npixels(fInterestingPixelThresh_v.size(),0);
      for ( auto const& img: imgs.Image2DArray() ) {
	auto const plane = img.meta().plane();
	auto const& adc_v = img.as_vector();
	for ( auto const& adc : adc_v ) {
	  if ( adc>fInterestingPixelThresh_v.at(plane) ) {
	    npixels.at(plane)++;
	  }
	}
      }
      
      int nempty = 0;
      for ( size_t i=0;i<npixels.size(); i++ ) {
	LARCV_INFO() << "Plane " << i << " has "
		     << npixels[i] << " pixels above "
		     << fInterestingPixelThresh_v[i] << " [ADC] ... Threshold = " 
		     << fNumPixelRedrawThresh_v[i] << std::endl;
	if ( fNumPixelRedrawThresh_v.at(i)>0 && npixels.at(i)<fNumPixelRedrawThresh_v.at(i) )
	  nempty++;
      }
      LARCV_INFO() << "... " << nempty << " planes below threshold: returning "
		   << ( nempty >=fRedrawOnNEmptyPlanes ? "false" : "true" ) << std::endl;

      if ( nempty>=fRedrawOnNEmptyPlanes )
	return false;
      
      return true;
    }
    
    int HiResImageDivider::findVertexDivision( const larcv::ROI& roi ) {
      auto vtx = roi.Position();
      if(fApplySCE) ApplySCE(vtx);
      int regionindex = 0;
      for ( std::vector< larcv::hires::DivisionDef >::iterator it=m_divisions.begin(); it!=m_divisions.end(); it++) {
	DivisionDef const& div = (*it);
	if ( div.isInsideDetRegion( vtx.X(), vtx.Y(), vtx.Z() ) )
	  return regionindex;
	regionindex++;
      }
      return -1;
    }

    /*
    bool HiResImageDivider::keepNonVertexDivision( const larcv::ROI& roi ) {
      return true;
    }
    */    
    void HiResImageDivider::cropEventImages( const larcv::EventImage2D& event_images, const larcv::hires::DivisionDef& div, larcv::EventImage2D& output_images ) { 

      // Output Image Container
      std::vector<larcv::Image2D> cropped_images;

      LARCV_DEBUG() << "Images to crop: "<< event_images.Image2DArray().size() << std::endl;

      for ( auto const& img : event_images.Image2DArray() ) {

	int iplane = (int)img.meta().plane();
	larcv::ImageMeta const& divPlaneMeta = div.getPlaneMeta( iplane );

	// divPlaneMeta is based on un-compressed time axis.
	// align the same scaling as img meta

	auto scaled = divPlaneMeta;
	
	scaled.update(scaled.height()/img.meta().pixel_height(),scaled.cols());
	
	auto cropmeta = img.meta().overlap(scaled);

	auto cropped = img.crop(cropmeta);

	cropped.resize(fMaxWireImageWidth,fMaxWireImageWidth,0.);

	//div.print();
	LARCV_DEBUG() << "image: " << std::endl << img.meta().dump() ;
	LARCV_DEBUG() << "div: " << std::endl << divPlaneMeta.dump() ;
	LARCV_DEBUG() << "scaled: " << std::endl << scaled.dump() ;
	LARCV_DEBUG() << "to-be-cropped: " << std::endl << cropmeta.dump() ;
	LARCV_DEBUG() << "cropped: " << std::endl << cropped.meta().dump() ;
	
	/*
	// we adjust the actual crop meta
	int tstart = divPlaneMeta.min_y();
	int twidth = fMaxWireImageWidth*fTickDownSample;
	int tmax = std::min( tstart+twidth, (int)img.meta().max_y() );

	larcv::ImageMeta cropmeta( divPlaneMeta.width(), twidth,
				   twidth, divPlaneMeta.width(), 
				   divPlaneMeta.min_x(), tmax );
	
	LARCV_INFO() << "image: " << std::endl << img.meta().dump() << std::endl;
	
	LARCV_INFO() << "div: " << std::endl << divPlaneMeta.dump() << std::endl;
	
	LARCV_INFO() << "crop: " << std::endl << cropmeta.dump() << std::endl;
		
	Image2D cropped = img.crop( cropmeta );
	LARCV_INFO() << "cropped." << std::endl;
	
	//cropped.resize( fMaxWireImageWidth*fTickDownSample, fMaxWireImageWidth, 0.0 );  // resize to final image size (and zero pad extra space)
	LARCV_INFO() << "resized." << std::endl;

	//cropped.compress( (int)cropped.meta().height()/6, fMaxWireImageWidth, larcv::Image2D::kSum );
	LARCV_INFO() << "downsampled. " << cropped.meta().height() << " x " << cropped.meta().width() << std::endl;
	*/	
	cropped_images.emplace_back( cropped );
	//LARCV_INFO() << "stored." << std::endl;
      }//end of plane loop

      output_images.Emplace( std::move( cropped_images ) );

    }

    void HiResImageDivider::LoadDivisionsFromFile() {

      // The image divisions are calculated before hand in the fixed grid model
      // we load the prefined region image definitions here
      LARCV_INFO() << "Loading Divisions from File." << std::endl;

      std::set<larcv::hires::DivisionID> unique_div_s;

      TFile* f = new TFile( Form("%s/app/HiResDivider/dat/%s", getenv("LARCV_BASEDIR"),fDivisionFile.c_str()), "open" );
      TTree* t = (TTree*)f->Get("imagedivider/regionInfo");
      int **planebounds = new int*[fNPlanes];
      int planenwires[fNPlanes];
      for (int p=0; p<fNPlanes; p++) {
	planebounds[p] = new int[2];
	char bname1[100];
	sprintf( bname1, "plane%d_wirebounds", (int)p );
	t->SetBranchAddress( bname1, planebounds[p] );

	char bname2[100];
	sprintf( bname2, "plane%d_nwires", (int)p );
	t->SetBranchAddress( bname2, &(planenwires[p]) );
	//std::cout << "setup plane=" << p << " branches" << std::endl;
      }
      
      float zbounds[2];
      float xbounds[2];
      float ybounds[2];
      int tickbounds[2];

      t->SetBranchAddress( "zbounds", zbounds );
      t->SetBranchAddress( "ybounds", ybounds );
      t->SetBranchAddress( "xbounds", xbounds );
      t->SetBranchAddress( "tickbounds", tickbounds );

      fMaxWireInRegion = 0;
      size_t entry = 0;
      size_t bytes = t->GetEntry(entry);
      while ( bytes>0 ) {
	for (size_t p=0; p<3; p++) {
	  if ( fMaxWireInRegion<planenwires[p] )
	    fMaxWireInRegion = planenwires[p];
	}
	int plane0[2], plane1[2], plane2[2];
	for (size_t i=0; i<2; i++) {
	  plane0[i] = (int)planebounds[0][i];
	  plane1[i] = (int)planebounds[1][i];
	  plane2[i] = (int)planebounds[2][i];
	  tickbounds[i] *= fTickPreCompression;
	  tickbounds[i] += fTickStart;
	}
	plane0[1] += fWirePreCompression;
	plane1[1] += fWirePreCompression;
	plane2[1] += fWirePreCompression;
	tickbounds[1] += fTickPreCompression;
 	LARCV_DEBUG() << "division entry " << entry << ": "
		      << " p0: [" << plane0[0] << "," << plane0[1] << "]"
		      << " p1: [" << plane1[0] << "," << plane1[1] << "]"
		      << " p2: [" << plane2[0] << "," << plane2[1] << "]"
		      << " t: ["  << tickbounds[0] << "," << tickbounds[1] << "]"
		      << std::endl;
	
	DivisionDef div( plane0, plane1, plane2, tickbounds, xbounds, ybounds, zbounds );

	auto const& div_id = div.ID();
	if(unique_div_s.find(div_id) == unique_div_s.end()) {
	  unique_div_s.insert(div.ID());
	  m_divisions.emplace_back(div);
	}
	entry++;
	bytes = t->GetEntry(entry);
	//std::cout << "Division tree entry:" << entry << " (" << bytes << ")" << std::endl;
      }

      if ( fMaxWireInRegion>fMaxWireImageWidth )
	fMaxWireImageWidth = fMaxWireInRegion;

      LARCV_INFO() << "Filled " << m_divisions.size() << " unique divisions..." << std::endl;
      LARCV_INFO() << "MaxWireImageWidth: " << fMaxWireImageWidth << std::endl;

      for (int p=0; p<fNPlanes; p++) {
	delete [] planebounds[p];
      }
      delete [] planebounds;
      
      f->Close();

    }

    void HiResImageDivider::CalculateDivisions() {

      LARCV_INFO() << "Calculating Divisions." << std::endl;

      std::set<larcv::hires::DivisionID> unique_div_s;
      fMaxWireImageWidth = 0; // will fill this later

      float zbounds[2] = {    0.0, 1037.0 }; //{ 0.04, 1036.45 };
      float ybounds[2] = { -116.0,  118.0 }; //{-115.51,117.44};
      float tbounds[2] = { 2400.0, 8448.0 }; // 2400+6048 // TICKS
      float trigger_tick = 3200; // sets 'x=0'

      float zWidth = (zbounds[1]-zbounds[0])/fNumZdivisions;
      float yWidth = (ybounds[1]-ybounds[0])/fNumYdivisions;
      float tWidth = (tbounds[1]-tbounds[0])/fNumTdivisions; // in ticks

      int nz = fNumZdivisions;
      int ny = fNumYdivisions;
      int nt = fNumTdivisions;
      float zstep = zWidth;
      float ystep = yWidth;
      float tstep = tWidth;
      float driftx = 0.11*0.5; // (0.11 cm per microsecond) * 0.5 microseconds per tick
      
      if ( fOverlapDivisions ) {
	nz = 2*fNumZdivisions-1;
	ny = 2*fNumYdivisions-1;
	nt = 2*fNumTdivisions-1;
	zstep *= 0.5;
	ystep *= 0.5;
	tstep *= 0.5;
      }


      std::vector<int> uwires;
      std::vector<int> vwires;
      std::vector<int> ywires;
      
      divalgo::DividerAlgo algo;
	    
      for (int iz=0; iz<nz; iz++) {
	for (int iy=0; iy<ny; iy++) {

	  float box_z[2] = { zbounds[0]+iz*zstep, zbounds[0]+iz*zstep+zWidth };
	  float box_y[2] = { ybounds[0]+iy*ystep, ybounds[0]+iy*ystep+zWidth };
	  float bbox[4] = { box_z[0], box_y[0], box_z[1], box_y[1] };
	  algo.getregionwires( bbox[0], bbox[1], bbox[2], bbox[3], ywires, uwires, vwires );
	  
	  int ubounds[2] = { uwires.at(0), uwires.back() };
	  int vbounds[2] = { vwires.at(0), vwires.back() };
	  int ybounds[2] = { ywires.at(0), ywires.back() };

	  if ( (int)uwires.size()>fMaxWireImageWidth ) fMaxWireImageWidth = (int)uwires.size();
	  if ( (int)vwires.size()>fMaxWireImageWidth ) fMaxWireImageWidth = (int)vwires.size();
	  if ( (int)ywires.size()>fMaxWireImageWidth ) fMaxWireImageWidth = (int)ywires.size();

	  for (int it=0; it<nt; it++) {
	    
	    int tickbounds[2] = { (int)(tbounds[0] + it*tstep), (int)(tbounds[0] + it*tstep + tWidth) };
	    float box_x[2] = { (tickbounds[0]-trigger_tick)*driftx, (tickbounds[1]-trigger_tick)*driftx };

	    DivisionDef div( ubounds, vbounds, ybounds, tickbounds, box_x, box_y, box_z );

	    auto const& div_id = div.ID();
	    if(unique_div_s.find(div_id) == unique_div_s.end()) {
	      unique_div_s.insert(div.ID());
	      m_divisions.emplace_back(div);
	    }
	    
	  }//end of t-loop
	  
	}//end of y-loop
      }//end of z-loop

    }

  }
}
#endif
