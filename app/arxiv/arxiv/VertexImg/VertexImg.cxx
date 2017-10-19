#ifndef __VERTEXIMG_CXX__
#define __VERTEXIMG_CXX__

#include "VertexImg.h"

#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventROI.h"

// larlite
#include "LArUtil/Geometry.h"
#include "LArUtil/TimeService.h"
#include "LArUtil/LArProperties.h"

namespace larcv {

  static VertexImgProcessFactory __global_VertexImgProcessFactory__;

  VertexImg::VertexImg(const std::string name)
    : ProcessBase(name)
  {}
    
  void VertexImg::configure(const PSet& cfg)
  {
    fROIProducerName    = cfg.get<std::string>("ROIProducerName");
    fImageProducerName  = cfg.get<std::string>("ImageProducerName");
    fOutputProducerName = cfg.get<std::string>("OutputProducerName");
    fImageTickStart     = cfg.get<int>("ImageTickStart");
    fVertexRadius       = cfg.get<int>("VertexRadius");
    fMarkValue          = cfg.get<int>("MarkValue",1);
    if ( fImageProducerName==fOutputProducerName )
      finplace = true;
    else
      finplace = false;
  }

  void VertexImg::initialize()
  {}

  bool VertexImg::process(IOManager& mgr)
  {
    

    EventImage2D* event_original = (EventImage2D*)mgr.get_data(kProductImage2D, fImageProducerName);
    EventImage2D* event_vertex   = NULL;
    EventROI*     event_roi      = (EventROI*)mgr.get_data(kProductROI, fROIProducerName);
    if (!finplace)
      event_vertex  = (EventImage2D*)mgr.get_data(kProductImage2D, fOutputProducerName);
    else
      event_vertex  = event_original;

    if ( !event_original ) {
      LARCV_ERROR() << "Could not open input images." << std::endl;
      return false;
    }
    if ( !event_vertex )  {
      LARCV_ERROR() << "Could not open output image holder." << std::endl;
      return false;      
    }

    // find neutrino interaction      
    bool vertex_found = false;
    TVector3 vertex;
    double vertex_t = 0.0;
    double vertex_tdrift = 0.0;
    double vertex_tick = 0;
    double vertex_tg4elec = 0;
    LARCV_DEBUG() << "Drift velocity: " << ::larutil::LArProperties::GetME()->DriftVelocity() << " cm/us" << std::endl;
    LARCV_DEBUG() << "Trigger Time: " << ::larutil::TimeService::GetME()->TriggerTime() << " us" << std::endl;
    LARCV_DEBUG() << "Trigger offset: " << ::larutil::TimeService::GetME()->TriggerOffsetTPC() << std::endl;
    for ( auto& roi : event_roi->ROIArray() ) {
      if ( roi.Type()==kROIBNB ) {
	vertex.SetX( roi.X() );
	vertex.SetY( roi.Y() );
	vertex.SetZ( roi.Z() );
	vertex_t       = roi.T(); // G4 time [ns]
	vertex_tdrift  = roi.X()/::larutil::LArProperties::GetME()->DriftVelocity(); // [us]
	vertex_tg4elec = ::larutil::TimeService::GetME()->G4ToElecTime( vertex_t ); // 
	vertex_tick    = fImageTickStart + (vertex_tg4elec-::larutil::TimeService::GetME()->TriggerTime() +vertex_tdrift + 400.333)/0.5; // hacks!!
	vertex_found   = true;
	break;
      }
    }

    double plane_offsets_ticks[3] = { 0.0, 
				      0.3/::larutil::LArProperties::GetME()->DriftVelocity()/0.5, 
				      0.6/::larutil::LArProperties::GetME()->DriftVelocity()/0.5 };

    if ( !vertex_found )
      return false;
    LARCV_DEBUG() << "Vertex: (" << vertex.X() << ", " << vertex.Y() << ", " << vertex.Z() << ","
		 << " t=" << vertex_t*1.0e-3 << " us : "
		 << " tdrift=" << vertex_tdrift << " us : "
		 << " tg4elec=" << vertex_tg4elec << " us :  "
		 << " tg4elec-trig=" << vertex_tg4elec-::larutil::TimeService::GetME()->TriggerTime() << " us :  "
		 << " tick=" << vertex_tick << ")" << std::endl;


    std::vector< larcv::Image2D > image_v;
    event_original->Move(image_v ); // original moves ownership of its image vectors to us
 
    std::vector< larcv::Image2D > vertex_image_v; // space for new images if we are not working in place

    std::vector<float> _buffer;
    for ( size_t i=0; i<image_v.size(); ++i ) {

      // get access to the data
      larcv::Image2D& original_image = image_v[i];
      int plane = original_image.meta().plane();
      
      // get the image size
      int orig_rows = original_image.meta().rows();
      int orig_cols = original_image.meta().cols();
      
      // get the origin
      float topleft_x = original_image.meta().min_x();

      // get width
      float width_per_pixel  = original_image.meta().pixel_width();
      float height_per_pixel = original_image.meta().pixel_height();

      // get vertex in image
      // copied from supera
      double min_wire=0;
      double max_wire=::larutil::Geometry::GetME()->Nwires(plane)-1;
      double wire=::larutil::Geometry::GetME()->WireCoordinate(vertex,plane) + 0.5;
      if(wire<min_wire) wire = min_wire;
      if(wire>max_wire) wire = max_wire;

      int pixel_vertex_wire = (wire - topleft_x)/width_per_pixel;
      int pixel_vertex_time = (vertex_tick + plane_offsets_ticks[plane] - original_image.meta().min_y())/height_per_pixel;
      
      LARCV_DEBUG() << "origin: (" << topleft_x << "," << original_image.meta().min_y() << ")" << std::endl;
      LARCV_DEBUG() << "pixel vertex plane=" << plane << " (" << pixel_vertex_wire << ", " << pixel_vertex_time << ") " 
		    << "max(" << orig_cols << "," << orig_rows << ") " 
		    << "ds(" << width_per_pixel << "," << height_per_pixel << ")" << std::endl;

      // define the new image
      larcv::Image2D new_image( original_image.meta() );
      new_image.paint( 0.0 );

      if ( (pixel_vertex_wire>=0 && pixel_vertex_wire < orig_cols) && (pixel_vertex_time>=0 && pixel_vertex_time<orig_rows) ) {
	
	for (int r=pixel_vertex_time-fVertexRadius; r<=pixel_vertex_time+fVertexRadius; r++) {
	  if ( r<0 ) continue;
	  if ( r>=orig_rows ) continue;
	  for (int c=pixel_vertex_wire-fVertexRadius; c<=pixel_vertex_wire+fVertexRadius; c++) {
	    if ( c<0 ) continue;
	    if ( c>=orig_cols ) continue;
	    double dr = r-pixel_vertex_time;
	    double dc = c-pixel_vertex_wire;
	    if ( sqrt( dr*dr + dc*dc ) < fVertexRadius )
	      LARCV_DEBUG() << "Mark pixel: (" << c << "," << orig_rows-r << ")" << std::endl;
	      new_image.set_pixel( orig_rows-r, c, fMarkValue );
	  }
	}
      }
      
      vertex_image_v.emplace_back( std::move(new_image) );
    }

    // store
    if ( finplace ) {
      // give the new images
      event_original->Emplace( std::move(vertex_image_v) );
    }
    else {
      // return the original images
      event_original->Emplace( std::move( image_v ) );
      // we give the new image2d container our relabeled images
      event_vertex->Emplace( std::move(vertex_image_v) );
    }
    
    return true;

  }

  void VertexImg::finalize()
  {}

}
#endif
