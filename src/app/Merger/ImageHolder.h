/**
 * \file ImageHolder.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ImageHolder
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __IMAGEHOLDER_H__
#define __IMAGEHOLDER_H__

#include "Processor/ProcessBase.h"
#include "DataFormat/ChStatus.h"
#include "DataFormat/Image2D.h"
#include "DataFormat/ROI.h"
#include <map>
namespace larcv {

  /**
     \class ProcessBase
     User defined class ImageHolder ... these comments are used to generate
     doxygen documentation!
  */
  class ImageHolder : public ProcessBase {

  public:
    
    /// Default constructor
    ImageHolder(const std::string name="ImageHolder");
    
    /// Default destructor
    virtual ~ImageHolder(){}

    void move_pmt_image   (Image2D&);
    void move_tpc_image   (std::vector<larcv::Image2D>&);
    void move_tpc_segment (std::vector<larcv::Image2D>&);
    void move_ch_status   (std::map<larcv::PlaneID_t,larcv::ChStatus>&);
    void move_roi         (std::vector<larcv::ROI>&);

    void retrieve_id(const EventBase* data)
    { _run=data->run(); _subrun=data->subrun(); _event=data->event(); }
    
    size_t run    () const { return _run;    }
    size_t subrun () const { return _subrun; }
    size_t event  () const { return _event;  }

  protected:

    size_t _run, _subrun, _event;
    std::vector<larcv::Image2D>  _tpc_image_v;
    std::vector<larcv::Image2D>  _tpc_segment_v;
    Image2D _pmt_image;
    std::map< ::larcv::PlaneID_t,larcv::ChStatus> _ch_status_m;
    std::vector<larcv::ROI> _roi_v;

  };

}

#endif
/** @} */ // end of doxygen group 

