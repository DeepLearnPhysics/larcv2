#ifndef __SUPERA_LAR2IMAGE_CXX__
#define __SUPERA_LAR2IMAGE_CXX__

#include "LAr2Image.h"
#include "Base/larcv_logger.h"

namespace supera {

  larcv::Image2D Hit2Image2D(const larcv::ImageMeta & meta,
			     const std::vector<supera::LArHit_t>& hits, const int time_offset)
  {
    //int nticks = meta.rows();
    //int nwires = meta.cols();
    //size_t row_comp_factor = (size_t)(meta.pixel_height());
    const int ymax = meta.max_y() - 1; // Need in terms of row coordinate
    const int ymin = (meta.min_y() >= 0 ? meta.min_y() : 0);
    larcv::Image2D img(meta);

    LARCV_SINFO() << "Filling an image: " << meta.dump();
    LARCV_SINFO() << "(ymin,ymax) = (" << ymin << "," << ymax << ")" << std::endl;

    for(auto const& h : hits) {
      auto const& wire_id = ::supera::ChannelToWireID(h.Channel());

      if ((int)(wire_id.Plane) != meta.plane()) continue;

      size_t col = 0;
      try { col = meta.col(wire_id.Wire); }
      catch (const larcv::larbys&) { continue; }

      int row = int(h.PeakTime()+0.5) + time_offset;
      if(row > ymax || row < ymin) continue;
      img.set_pixel(ymax-row,col,h.Integral());
    }
    return img;
  }

  std::vector<larcv::Image2D> Hit2Image2D(const std::vector<larcv::ImageMeta>& meta_v,
					  const std::vector<supera::LArHit_t>& hits,
					  const int time_offset)
  {
    std::vector<larcv::Image2D> res_v;
    for (size_t p = 0; p < ::supera::Nplanes(); ++p) {
      auto const& meta = meta_v.at(p);
      res_v.emplace_back(std::move(Hit2Image2D(meta,hits,time_offset)));
      res_v.back().index(res_v.size()-1);
    }
    return res_v;
  }
  
  larcv::Image2D Wire2Image2D(const larcv::ImageMeta& meta,
			      const std::vector<supera::LArWire_t>& wires,
			      const int time_offset)
  {
    //int nticks = meta.rows();
    //int nwires = meta.cols();
    size_t row_comp_factor = (size_t)(meta.pixel_height());
    const int ymax = meta.max_y() - 1; // Need in terms of row coordinate
    const int ymin = (meta.min_y() >= 0 ? meta.min_y() : 0);
    larcv::Image2D img(meta);
    img.paint(0.);
    
    LARCV_SINFO() << "Filling an image: " << meta.dump();
    LARCV_SINFO() << "(ymin,ymax) = (" << ymin << "," << ymax << ")" << std::endl;

    for (auto const& wire : wires) {

      auto const& wire_id = ::supera::ChannelToWireID(wire.Channel());

      if ((int)(wire_id.Plane) != meta.plane()) continue;

      size_t col = 0;
      try {
	col = meta.col(wire_id.Wire);
      } catch (const ::larcv::larbys&) {
	continue;
      }

      for (auto const& range : wire.SignalROI().get_ranges()) {

	auto const& adcs = range.data();
	//double sumq = 0;
	//for(auto const& v : adcs) sumq += v;
	//sumq /= (double)(adcs.size());
	//if(sumq<3) continue;

	int start_index = range.begin_index() + time_offset;
	int end_index   = start_index + adcs.size() - 1;
	if (start_index > ymax || end_index < ymin) continue;

	if (row_comp_factor > 1) {

	  for (size_t index = 0; index < adcs.size(); ++index) {
	    if ((int)index + start_index < ymin) continue;
	    if ((int)index + start_index > ymax) break;
	    auto row = meta.row((double)(start_index + index));
	    img.set_pixel(row, col, adcs[index]);
	  }
	} else {
	  // Fill matrix from start_index => end_index of matrix row
	  // By default use index 0=>length-1 index of source vector
	  int nskip = 0;
	  int nsample = adcs.size();
	  if (end_index   > ymax) {
	    LARCV_SDEBUG() << "End index (" << end_index << ") exceeding image bound (" << ymax << ")" << std::endl;
	    nsample   = adcs.size() - (end_index - ymax);
	    end_index = ymax;
	    LARCV_SDEBUG() << "Corrected End index = " << end_index << std::endl;
	  }
	  if (start_index < ymin) {
	    LARCV_SDEBUG() << "Start index (" << start_index << ") exceeding image bound (" << ymin << ")" << std::endl;
	    nskip = ymin - start_index;
	    nsample -= nskip;
	    start_index = ymin;
	    LARCV_SDEBUG() << "Corrected Start index = " << start_index << std::endl;
	  }
	  LARCV_SDEBUG() << "Calling a reverse_copy..." << std::endl
			 << "      source wf : start index = " << range.begin_index() << " length = " << adcs.size() << std::endl
			 << "      (row,col) : (" << (ymax - end_index) << "," << col << ")" << std::endl
			 << "      nskip     : "  << nskip << std::endl
			 << "      nsample   : "  << nsample << std::endl;
	  try {
	    img.reverse_copy(ymax - end_index,
			     col,
			     adcs,
			     nskip,
			     nsample);
	  } catch (const ::larcv::larbys& err) {
	    LARCV_SCRITICAL() << "Attempted to fill an image..." << std::endl
			      << meta.dump()
			      << "(ymin,ymax) = (" << ymin << "," << ymax << ")" << std::endl
			      << "Called a reverse_copy..." << std::endl
			      << "      source wf : plane = " << wire_id.Plane << " wire = " << wire_id.Wire << std::endl
			      << "      timing    : start index = " << range.begin_index() << " length = " << adcs.size() << std::endl
			      << "      (row,col) : (" << (ymax - end_index) << "," << col << ")" << std::endl
			      << "      nskip     : "  << nskip << std::endl
			      << "Re-throwing an error:" << std::endl;
	    throw err;
	  }
	}
      }
    }
    return img;
  }

  std::vector<larcv::Image2D>
  Wire2Image2D(const std::vector<larcv::ImageMeta>& meta_v,
	       const std::vector<supera::LArWire_t>& wires,
	       const int time_offset)
  {
    std::vector<larcv::Image2D> res_v;
    for (size_t p = 0; p < ::supera::Nplanes(); ++p) {
      auto const& meta = meta_v.at(p);
      res_v.emplace_back(std::move(Wire2Image2D(meta,wires,time_offset)));
      res_v.back().index(res_v.size()-1);
    }
    return res_v;
  }

  larcv::Image2D OpDigit2Image2D(const larcv::ImageMeta& meta,
				 const std::vector<supera::LArOpDigit_t>& opdigit_v,
				 int time_offset)
  {
    larcv::Image2D img(meta);
    img.paint(2048);

    std::vector<float> tmp_wf(meta.rows(), 2048);
    for (auto const& opdigit : opdigit_v) {
      if (opdigit.size() < 1000) continue;
      auto const col = opdigit.ChannelNumber();
      if (meta.min_x() > col) continue;
      if (col >= meta.max_x()) continue;
      //
      // HACK: right way is to use TimeService + trigger.
      //       for now I just record PMT beamgate tick=0 as edge of an image (w/ offset)
      //
      size_t nskip = 0;
      if (time_offset < 0) nskip = (-1 * time_offset);
      if (nskip >= opdigit.size()) continue;
      for (auto& v : tmp_wf) v = 2048;
      size_t num_pixel = std::min(meta.rows(), opdigit.size() - nskip);
      for (size_t i = 0; i < num_pixel; ++i) tmp_wf[i] = (float)(opdigit[nskip + i]);
      img.copy(0, col, &(tmp_wf[0]), num_pixel);
      //img.reverse_copy(0,col,opdigit,nskip,num_pixel);
    }
    return img;
  }

  std::vector<larcv::Image2D>
  SimCh2Image2D(const std::vector<larcv::ImageMeta>& meta_v,
		const std::vector<larcv::ROIType_t>& track2type_v,
		const std::vector<supera::LArSimCh_t>& sch_v,
		const int time_offset)
  {
    LARCV_SINFO() << "Filling semantic-segmentation ground truth image..." << std::endl;
    std::vector<larcv::Image2D> img_v;
    for (auto const& meta : meta_v) {
      LARCV_SINFO() << meta.dump();
      img_v.emplace_back(larcv::Image2D(meta));
    }

    static std::vector<float> column;
    for (auto const& img : img_v) {
      if (img.meta().rows() >= column.size())
	column.resize(img.meta().rows() + 1, (float)(::larcv::kROIUnknown));
    }

    for (auto const& sch : sch_v) {
      auto ch = sch.Channel();
      auto const& wid = ::supera::ChannelToWireID(ch);
      auto const& plane = wid.Plane;
      auto& img = img_v.at(plane);
      auto const& meta = img.meta();

      size_t col = wid.Wire;
      if (col < meta.min_x()) continue;
      if (meta.max_x() <= col) continue;
      if (plane != img.meta().plane()) continue;

      col -= (size_t)(meta.min_x());

      // Initialize column vector
      for (auto& v : column) v = (float)(::larcv::kROIUnknown);
      //for (auto& v : column) v = (float)(-1);

      for (auto const tick_ides : sch.TDCIDEMap()) {
	int tick = supera::TPCTDC2Tick((double)(tick_ides.first)) + time_offset;
	if (tick < meta.min_y()) continue;
	if (tick >= meta.max_y()) continue;
	// Where is this tick in column vector?
	size_t index = (size_t)(meta.max_y() - tick);
	// Pick type
	double energy = 0;
	::larcv::ROIType_t roi_type =::larcv::kROIUnknown;
	for (auto const& edep : tick_ides.second) {
	  if (edep.energy < energy) continue;
	  if (std::abs(edep.trackID) >= (int)(track2type_v.size())) continue;
	  auto temp_roi_type = track2type_v[std::abs(edep.trackID)];
	  if (temp_roi_type ==::larcv::kROIUnknown) continue;
	  energy = edep.energy;
	  roi_type = (::larcv::ROIType_t)temp_roi_type;
	}
	column[index] = roi_type;
      }
      // mem-copy column vector
      img.copy(0, col, column, img.meta().rows());
    }
    return img_v;
  }

  larcv::Voxel3DSet
  SimCh2Voxel3D(const larcv::Voxel3DMeta& meta,
		const std::vector<int>& track_v,
		const std::vector<supera::LArSimCh_t>& sch_v,
		const int time_offset,
		const size_t plane)
  {
    LARCV_SINFO() << "Filling Voxel3D ground truth volume..." << std::endl;
    larcv::Voxel3DSet res(meta);
    //double x, y, z, x_tick;
    double y, z, x_tick;
    //std::cout << "x_offset " << x_offset << std::endl;
    for (auto const& sch : sch_v) {
      auto ch = sch.Channel();
      
      auto const& wid = ::supera::ChannelToWireID(ch);
      if( plane != (size_t)(wid.Plane) ) continue;

      for (auto const tick_ides : sch.TDCIDEMap()) {

	x_tick = (supera::TPCTDC2Tick(tick_ides.first) + time_offset) * supera::TPCTickPeriod()  * supera::DriftVelocity();
	/*
	std::cout << tick_ides.first << " : " << supera::TPCTDC2Tick(tick_ides.first) << " : " << time_offset << " : " << x_tick << std::flush;
	std::cout << (supera::TPCTDC2Tick(tick_ides.first) + time_offset) << std::endl
		  << (supera::TPCTDC2Tick(tick_ides.first) + time_offset) * supera::TPCTickPeriod() << std::endl
		  << (supera::TPCTDC2Tick(tick_ides.first) + time_offset) * supera::TPCTickPeriod() * supera::DriftVelocity() << std::endl
		  << std::endl;
	*/
	for (auto const& edep : tick_ides.second) {

	  if (std::abs(edep.trackID) >= (int)(track_v.size())) continue;

	  if (track_v[std::abs(edep.trackID)]<=0) continue;

	  //x = edep.x;
	  y = edep.y;
	  z = edep.z;
	  //supera::ApplySCE(x,y,z);
	  //std::cout << " ... " << x << std::endl;
	  // Now use tick-based position for x
	  auto vid = meta.ID(x_tick,y,z);
	  if(vid == larcv::kINVALID_VOXEL3DID) continue;

	  larcv::Voxel3D vx(vid,edep.energy);
	  res.Emplace(std::move(vx));
	}
      }
    }
    return res;
  }

  std::vector<std::vector<larcv::Pixel2DCluster> >
  SimCh2Pixel2DCluster(const std::vector<larcv::ImageMeta>& meta_v,
		       const std::vector<size_t>& row_compression_v,
		       const std::vector<size_t>& col_compression_v,
		       const std::vector<supera::LArSimCh_t>& sch_v,
		       const std::vector<size_t>& trackid2cluster,
		       const int time_offset)
  {
    std::vector<std::vector<larcv::Pixel2DCluster> > res_vv;

    static std::vector<float> column;
    for (auto const& meta : meta_v) {
      if (meta.rows() >= column.size())
	column.resize(meta.rows() + 1, 0);
    }
    
    // figure out # of clusters
    if(meta_v.size() != supera::Nplanes()) {
      LARCV_SCRITICAL() << "# Meta mismatch with # planes!" << std::endl;
      throw larcv::larbys();
    }
    // figure out # of clusters to be made
    size_t num_clusters=0;
    for(auto const& cidx : trackid2cluster) {
      if(cidx == larcv::kINVALID_SIZE) continue;
      if(cidx >= num_clusters) num_clusters = cidx + 1;
    }
    // loop over cluster index, construct cluster
    std::vector<larcv::Image2D> img_v;
    for(size_t plane=0; plane<supera::Nplanes(); ++plane)
      img_v.emplace_back(meta_v[plane]);
    std::vector<size_t> nonzero_npx_v(supera::Nplanes(),0);
    
    for(size_t cidx=0; cidx<num_clusters; ++cidx) {

      // initialize image container
      for(size_t plane=0; plane<supera::Nplanes(); ++plane) {
	img_v[plane].paint(0.);
	nonzero_npx_v[plane] = 0;
      }

      for (auto const& sch : sch_v) {
	
	auto ch = sch.Channel();
	auto const& wid   = ::supera::ChannelToWireID(ch);
	auto const& plane = wid.Plane;
	auto& img = img_v[plane];
	auto const& meta  = img.meta();
	auto& nonzero_npx = nonzero_npx_v[plane];
	  
	size_t col = wid.Wire;
	if (col < meta.min_x()) continue;
	if (meta.max_x() <= col) continue;
	if (plane != img.meta().plane()) continue;
	
	col -= (size_t)(meta.min_x());
	
	// Initialize column vector
	for (auto& v : column) v = 0.;
	//for (auto& v : column) v = (float)(-1);
	
	for (auto const tick_ides : sch.TDCIDEMap()) {
	  int tick = supera::TPCTDC2Tick((double)(tick_ides.first)) + time_offset;
	  if (tick < meta.min_y()) continue;
	  if (tick >= meta.max_y()) continue;
	  // Where is this tick in column vector?
	  size_t index = (size_t)(meta.max_y() - tick);
	  // Accummulate energy for relevant cluster
	  int trackid = 0;
	  double energy = 0;
	  for (auto const& edep : tick_ides.second) {
	    trackid = std::abs(edep.trackID);
	    if (trackid >= (int)(trackid2cluster.size())) continue;
	    if (trackid2cluster[trackid] != cidx) continue;
	    energy += edep.energy;
	  }
	  if(energy>0) {
	    column[index] = energy;
	    ++nonzero_npx;
	  }
	}
	// mem-copy column vector
	img.copy(0, col, column, img.meta().rows());
      }

      // Now convert them into Pixel2DCluster
      std::vector<larcv::Pixel2DCluster> res_v;
      res_v.reserve(supera::Nplanes());
      larcv::Pixel2D px;
      for(size_t plane=0; plane<supera::Nplanes(); ++plane) {

	larcv::Pixel2DCluster res;
	res.reserve(nonzero_npx_v[plane]);

	auto const& full_img  = img_v[plane];
	auto const& full_meta = full_img.meta();
	auto data = full_img.copy_compress(full_meta.rows() / row_compression_v.at(plane),
					   full_meta.cols() / col_compression_v.at(plane),
					   larcv::Image2D::CompressionModes_t::kSum);
	size_t nrows = full_meta.rows() / row_compression_v[plane];
	
	for(size_t px_idx=0; px_idx<data.size(); ++px_idx) {

	  if(data[px_idx] <= 0) continue;

	  px.Set( px_idx / nrows,
		  px_idx % nrows,
		  data[px_idx] );

	  res += px;
	}
	res_v.emplace_back(std::move(res));
      }
      res_vv.emplace_back(std::move(res_v));
    }
    return res_vv;
  }


}
#endif
