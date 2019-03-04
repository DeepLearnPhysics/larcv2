#ifndef __SEGMENTRELABEL_CXX__
#define __SEGMENTRELABEL_CXX__

#include "SegmentRelabel.h"
#include <vector>

#include "DataFormat/EventImage2D.h"

namespace larcv {

  static SegmentRelabelProcessFactory __global_SegmentRelabelProcessFactory__;

  SegmentRelabel::SegmentRelabel(const std::string name)
    : ProcessBase(name)
  {}
    
  void SegmentRelabel::configure(const PSet& cfg)
  {
    fInputSegmentProducer = cfg.get<std::string>("InputSegmentProducerName"); // input producer
    fOutputSegmentProducer = cfg.get<std::string>("OutputSegmentProducerName"); // input producer
    PSet  fRelabelPset   = cfg.get<PSet>("LabelMap"); 
    // the label map should look something like:
    //  { 0:[1,2,5], 
    //    1:[0,3], 
    //    2:[4,5]
    //       ...}: the index will be used as the new label. items in vector are old labels

    // parse the label map
    fOrigToNewMap.clear();
    for (auto const& it : fRelabelPset.keys() ) {
      int key = atoi( it.c_str() );
      std::vector<int> old_label_list = fRelabelPset.get< std::vector<int> >( it );
      for (auto &old_label : old_label_list ) {
	fOrigToNewMap[old_label] = key;
	LARCV_DEBUG() << "Reassigning " << old_label << " to " << key << std::endl;
      }
    }
    
    
  }

  void SegmentRelabel::initialize()
  {}

  bool SegmentRelabel::process(IOManager& mgr)
  {
    // input segmentation images
    bool inplace = false;
    if ( fOutputSegmentProducer==fInputSegmentProducer )
      inplace = true;

    EventImage2D* event_original_segment = (EventImage2D*)mgr.get_data(kProductImage2D, fInputSegmentProducer);
    EventImage2D* event_relabel_segment = NULL;
    if (!inplace)
      event_relabel_segment  = (EventImage2D*)mgr.get_data(kProductImage2D, fOutputSegmentProducer);
    else
      event_relabel_segment = event_original_segment;

    if ( !event_original_segment ) {
      LARCV_ERROR() << "Could not open input images." << std::endl;
      return false;
    }
    if ( !event_relabel_segment )  {
      LARCV_ERROR() << "Could not open output image holder." << std::endl;
      return false;      
    }

    std::vector< larcv::Image2D> image_v;
    event_original_segment->Move(image_v ); // original moves ownership of its image vectors to us
 
    std::vector< larcv::Image2D > relabel_image_v; // space for new images if we are not working in place


    std::vector<float> _buffer;
    for ( size_t i=0; i<image_v.size(); ++i ) {

      // get access to the data
      larcv::Image2D& original_image = image_v[i];
      auto const& labels = original_image.as_vector(); 

      // make a space for the relabeling
      _buffer.resize( labels.size(), 0.0 ); // 

      // do the relabeling
      for (size_t ival=0; ival<labels.size(); ival++) {
	int label=std::floor( labels.at(ival)+0.5 );
	int relabel=fOrigToNewMap[label];
	_buffer.at(ival) = (float)relabel;
      }
      
      // now place it
      if ( inplace ) {
	// if in place, we copy it back into the old data
	original_image.copy(0,0,_buffer,_buffer.size()); 
      }
      else {
	// else we make a new one and put it into the new relabeled iamge vector
	Image2D img( original_image.meta(), _buffer );
	relabel_image_v.emplace_back( std::move(img) );
      }
    }

    // store

    // no matter what, give the image vector back to the original event2d container
    event_original_segment->Emplace( std::move(image_v) );

    if (!inplace) {
      // if not inplace, we give the new image2d container our relabeled images
      event_relabel_segment->Emplace( std::move(relabel_image_v) );
    }
    
    return true;
  }

  void SegmentRelabel::finalize()
  {}

}
#endif
