/**
 * \file PulledPork3DSlicer.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class PulledPork3DSlicer
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __PULLEDPORK3DSLICER_H__
#define __PULLEDPORK3DSLICER_H__
#include "FMWKInterface.h"
#include "WireRange3D.h"
#include "ImageMetaMakerBase.h"

namespace supera {

  /**
     \class PulledPork3DSlicer
     User defined class PulledPork3DSlicer ... these comments are used to generate
     doxygen documentation!
  */
  class PulledPork3DSlicer : public ImageMetaMakerBase {

  public:
    
    /// Default constructor
    PulledPork3DSlicer() : ImageMetaMakerBase("PulledPork3DSlicer")
			 , _slicer()
    {}
    
    /// Default destructor
    ~PulledPork3DSlicer(){}

    static bool Is(const ImageMetaMakerBase* ptr)
    { return (ptr && ptr->name() == "PulledPork3DSlicer"); }

    void configure(const supera::Config_t&);

    void AddConstraint(const supera::LArMCTruth_t& mctruth);

    void AddConstraint(const std::vector<supera::LArMCTruth_t>& mctruth_v);

    void
    GenerateMeta(const std::vector<supera::LArSimCh_t>& simch_v,
		 const int time_offset);

    void
    GenerateMeta(const std::vector<supera::LArSimCh_t>& simch_v,
		 const int time_offset,
		 const std::vector<int>& trackid_v);

    void ClearEventData();

    bool Test() const;

    inline const std::vector<larcv::ImageMeta>& Meta() const
    { return _meta_v;}
    
  private:

    unsigned short _origin;
    std::vector<size_t> _wire_pixel_v;
    supera::WireRange3D _slicer;
    bool _apply_sce;
    size_t _time_pixel;
    double _t0_g4ns;
    std::vector<larcv::ImageMeta> _meta_v;

    std::vector<larcv::ImageMeta>
    DeriveMeta(const std::vector<supera::GridPoint3D>& point_v,
	       const int time_offset ) const;

    void
    GenerateMeta(const std::vector<supera::LArSimCh_t>& simch_v,
		 const int time_offset,
		 const std::vector<int>& trackid_v,
		 const bool use_track_id);
  };

}
#endif
/** @} */ // end of doxygen group 

