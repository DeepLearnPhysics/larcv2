#ifndef __LARCV_IMAGEMOD_H__
#define __LARCV_IMAGEMOD_H__

#include "larcv/core/DataFormat/Image2D.h"
#include "larcv/core/DataFormat/Voxel2D.h"
#include "larcv/core/DataFormat/Particle.h"

namespace larcv {

	larcv::Image2D
	Weight2DFromPixelCount(const  larcv::ClusterPixel2D& cluster_v, 
						   bool   normalize = true,
						   size_t ignore_front = 0,
						   size_t ignore_back  = 0,
						   size_t gather_small_count = 0);

	std::vector<larcv::Image2D>
	Image2DByParticlePDG(const  larcv::ClusterPixel2D& cluster_v,
	                     const  larcv::ParticleSet& particle_v,
	                     const  std::vector<std::vector<int> > pdgcodes_v,
	                     bool   append_unlisted_pdg = true,
	                     bool   ignore_zero_pdgcode = true);

	void Mask( Image2D& image, 
			   const Image2D& ref, 
			   float min_threshold, 
			   float mask_value=0., 
			   bool ignore_origin=false);

}

#endif
