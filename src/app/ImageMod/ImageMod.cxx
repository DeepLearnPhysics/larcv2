#ifndef __LARCV_IMAGEMOD_CXX__
#define __LARCV_IMAGEMOD_CXX__

#include "ImageMod.h"
#include "larcv/core/Base/larcv_logger.h"
#include "larcv/core/Base/larbys.h"
#include <set>
namespace larcv {

  Image2D Weight2DFromPixelCount(const  ClusterPixel2D& clusters,
                                 bool   normalize,
                                 size_t ignore_front,
                                 size_t ignore_back,
                                 size_t gather_small_count)
  {
    ImageMeta meta(clusters.meta());
    auto const& vox_array_v = clusters.as_vector();
    std::vector<float> image_data(meta.size(), 0.);
    
    std::vector<bool> small_particle_v(vox_array_v.size(),false);
    double small_particle_pxcount = 0.;
    for (size_t i = ignore_front; (i+ignore_back) < vox_array_v.size(); ++i) {
      auto const& vox_array = vox_array_v[i].as_vector();
      if(vox_array.size()<1) continue;
      if(vox_array.size()<gather_small_count) {
        small_particle_v[i] = true;
        small_particle_pxcount += vox_array.size();
        continue;
      }
      double weight = 1. / ((double)(vox_array.size()));
      //std::cout<<"Particle " << i << " ... " << vox_array.size() << " hits ... weight = " << weight << std::endl;
      for (auto const& vox : vox_array) {
        //image_data[vox.id()] += weight;
        if(image_data[vox.id()] < weight) image_data[vox.id()] = weight;
      }
    }
    //std::cout<<"Small particle pixel count " << small_particle_pxcount << " weight = " << 1./small_particle_pxcount<<std::endl;
    // Gather small clusters together
    for (size_t i = ignore_front; (i+ignore_back) < vox_array_v.size(); ++i) {
      if(!(small_particle_v[i])) continue;

      auto const& vox_array = vox_array_v[i].as_vector();
      double weight = 1. / small_particle_pxcount;
      for (auto const& vox : vox_array) {
        //image_data[vox.id()] += weight;
        if(image_data[vox.id()] < weight) image_data[vox.id()] = weight;
      }
    }

    // Count background pixels
    double weight_bg = 0;
    for (auto const& v : image_data) {
      if (v > 0) continue;
      weight_bg += 1.;
    }
    weight_bg = 1. / weight_bg;
    for (auto& v : image_data) {
      if (v > 0) continue;
      v = weight_bg;
    }

    // Normalize if requested
    if (normalize) {
      float weight_sum = 0;
      for (auto const& v : image_data) weight_sum += v;
      for (auto& v : image_data) v /= weight_sum;
    }

    return Image2D(std::move(meta), std::move(image_data));
  }

  std::vector<larcv::Image2D>
  Image2DByParticlePDG(const  larcv::ClusterPixel2D& cluster_v,
                       const  larcv::ParticleSet& particle_v,
                       const  std::vector<std::vector<int> > pdgcodes_v,
                       bool   append_unlisted_pdg,
                       bool   ignore_zero_pdgcode)
  {
    // Check particle array size is same or cluster_v.size()-1
    auto const& clusters  = cluster_v.as_vector();
    auto const& particles = particle_v.as_vector();
    if (particles.size() != clusters.size() && (particles.size() + 1) != clusters.size()) {
      LARCV_SERROR() << "particle_v size " << particles.size() << " vs. cluster_v size " << clusters.size() << std::endl;
      throw larbys();
    }
    // Check pdg is not repeated
    std::set<int> used_pdg;
    for (auto const& pdgcodes : pdgcodes_v) {
      for(auto const& pdgcode : pdgcodes) {
        if(used_pdg.find(pdgcode) != used_pdg.end()) {
          LARCV_SERROR() << "Duplicated use of pdg code " << pdgcode << std::endl;
          throw larbys();
        }
        used_pdg.insert(pdgcode);
      }
    }

    std::vector<larcv::Image2D> images;
    size_t num_images = (append_unlisted_pdg ? pdgcodes_v.size() + 1 : pdgcodes_v.size());
    for (size_t i = 0; i < num_images; ++i) {
      ImageMeta meta(cluster_v.meta());
      ((BBox2D)meta).update((larcv::ProjectionID_t)i);
      larcv::Image2D img(meta);
      img.paint(0.);
      images.emplace_back(std::move(img));
    }
    for (size_t i = 0; i < particles.size(); ++i) {
      int pdg_code = particles[i].pdg_code();
      auto const& cluster  = clusters[i].as_vector();
      if(cluster.size() < 1) continue;
      if(pdg_code == 0 && ignore_zero_pdgcode) continue;
      int image_index = -1;
      for (size_t class_index = 0; class_index < pdgcodes_v.size(); ++class_index) {
        auto const& pdgcodes = pdgcodes_v[class_index];
        for(auto const& pdgcode : pdgcodes) {
          if(pdgcode != pdg_code) continue;
          image_index = class_index;
          break;
        }
        if(image_index >=0) break;
      }
      if (image_index < 0 && append_unlisted_pdg) image_index = (num_images - 1);
      if (image_index < 0) continue;

      auto& image = images[image_index];
      auto data = image.move();
      for (auto const& vox : cluster)
        data[vox.id()] += vox.value();
      image.move(std::move(data));
    }
    return images;
  }

  void Mask(Image2D& image, const Image2D& ref, float min_threshold, float mask_value, bool ignore_origin) {

    auto ref_image = Image2D(image.meta());
    ref_image.paint(0);

    LARCV_SDEBUG() << "Overlaying:\n" << image.meta().dump() << ref.meta().dump() << std::flush;

    if (ignore_origin) {
      if (image.meta().cols() != ref.meta().cols() ||
          image.meta().rows() != ref.meta().rows() ) {
        LARCV_SCRITICAL() << "IgnoreRefOrigin option = true ... only allowed if ref and target image dimensions are identical!" << std::endl;
        throw larbys();
      }
        std::vector<float> ref_data = ref.as_vector();
        ref_image.move(std::move(ref_data));
      }
    else { ref_image.overlay( ref ); }

    if (image.as_vector().size() != ref_image.as_vector().size()) {
      LARCV_CRITICAL() << "Different size among the target (" << image.as_vector().size()
                       << ") and reference (" << ref_image.as_vector().size()
                       << ")!" << std::endl;
      throw larbys();
    }

    auto const& ref_data = ref_image.as_vector();
    auto data = image.move();
    for (size_t px_idx = 0; px_idx < ref_data.size(); ++px_idx)

      if (ref_data[px_idx] <= min_threshold) data[px_idx] = mask_value;

    image.move(std::move(data));
  }

}

#endif
