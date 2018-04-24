#ifndef __SEGMENT2DFROMPDG_CXX__
#define __SEGMENT2DFROMPDG_CXX__

#include "larcv/core/DataFormat/EventVoxel2D.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "Segment2DFromPDG.h"
#include "ImageMod.h"

namespace larcv {

  static Segment2DFromPDGProcessFactory __global_Segment2DFromPDGProcessFactory__;

  Segment2DFromPDG::Segment2DFromPDG(const std::string name)
    : ProcessBase(name)
  {}

  void Segment2DFromPDG::configure(const PSet& cfg)
  {
    _output_producer   = cfg.get<std::string>( "OutputImage"     );
    _label_producer    = cfg.get<std::string>( "OutputSegment"   );
    _weight_producer   = cfg.get<std::string>( "OutputWeight", "");
    _input_producer    = cfg.get<std::string>( "InputImage"      );
    _cluster_producer  = cfg.get<std::string>( "InputCluster"    );
    _particle_producer = cfg.get<std::string>( "InputParticle"   );

    _dist_surrounding   = cfg.get<size_t>( "DistSurrounding");
    _projection_v       = cfg.get<std::vector<size_t> >("Projection" );

    _label_type         = (LabelType_t)(cfg.get<unsigned short>("LabelType"));

    std::vector<int> empty_list;
    std::vector<std::set<int> > pdg_list;
    while (true) {
      std::string param_name = "ParticleSet" + std::to_string(pdg_list.size());
      auto another_set = cfg.get<std::vector<int> >(param_name, empty_list);
      if (another_set.empty() && pdg_list.size()) break;
      for (size_t i = 0; i < pdg_list.size(); ++i) {
        for (auto const& pdg : another_set) {
          if (pdg_list[i].find(pdg) == pdg_list[i].end()) continue;
          LARCV_CRITICAL() << param_name << " includes PDG " << pdg
                           << " which was already used in ParticleSet" << i << std::endl;
          throw larbys();
        }
      }
      std::set<int> temp_pdg_list;
      for (auto const& pdg : another_set) temp_pdg_list.insert(pdg);
      pdg_list.emplace_back(std::move(temp_pdg_list));
    }

    _pdg_list.clear();
    _pdg_list.resize(pdg_list.size());
    for (size_t class_index = 0; class_index < pdg_list.size(); ++class_index) {
      LARCV_INFO() << "Assining to class " << class_index << " ... " << std::flush;
      for (auto pdg : pdg_list[class_index]) {
        _pdg_list[class_index].push_back(pdg);
        LARCV_INFO() << pdg << " ";
      }
      LARCV_INFO() << std::endl;
    }
  }

  void Segment2DFromPDG::initialize()
  {}

  larcv::Image2D 
  Segment2DFromPDG::CreateSegment2D(const  larcv::ClusterPixel2D& cluster_v,
                                    const  larcv::ParticleSet& particle_v) {

    // Ignore pdgcode==0 (last boolean)
    auto label_v = Image2DByParticlePDG( cluster_v, particle_v, _pdg_list, true, true );
    std::vector<float> data;
    data.resize(label_v.front().meta().size(), 0.);
    switch (_label_type) {
    case kOverwrite:
      for (size_t i = 0; i < data.size(); ++i) {
        size_t label_class = 0;
        for (size_t j = 0; j < label_v.size(); ++j)
          if (label_v[j].as_vector()[i] > 0.) label_class = (j + 1);
        data[i] = label_class;
      }
      break;
    case kPixelValue:
      for (size_t i = 0; i < data.size(); ++i) {
        size_t label_class = 0;
        float  max_q = 0;
        for (size_t j = 0; j < label_v.size(); ++j) {
          if (label_v[j].as_vector()[i] <= max_q) continue;
          max_q = label_v[j].as_vector()[i];
          label_class = (j + 1);
        }
        data[i] = label_class;
      }
      break;
    default:
      LARCV_CRITICAL() << "LabelType_t " << (int)(_label_type) << " not supported..." << std::endl;
      throw larbys();
    }
    larcv::ImageMeta meta(label_v.front().meta());
    larcv::Image2D res(std::move(meta), std::move(data));

    return res;
  }

  larcv::Image2D
  Segment2DFromPDG::CreateWeight2D(const larcv::ClusterPixel2D& cluster_v,
                                   const larcv::Image2D& input_image) {
    // Ignore last cluster
    auto weight_image = Weight2DFromPixelCount(cluster_v, false, 0, 1, 10);

    // Mask irrlevant pixels
    Mask(weight_image, input_image, 0.0, 0., false);

    auto const& meta  = weight_image.meta(); 

    if (_boundary_data.size() != meta.size())
      _boundary_data.resize(meta.size(), 0.);
    for (auto& v : _boundary_data) v = 0.;

    // Next, compute "surrounding pixels"
    float surrounding_weight = 0;
    int target_row, target_col;
    int nrows = meta.rows();
    int ncols = meta.cols();
    size_t n_surrounding_pixel = 0;
    bool flag = false;
    for (int row = 0; row < nrows; ++row) {
      for (int col = 0; col < ncols; ++col) {
        float weight = (size_t)(weight_image.pixel(row, col));
        if (weight > 0.)
          continue;
        flag = false;
        for (target_row = (int)(row + _dist_surrounding); (int)(target_row + _dist_surrounding) >= row; --target_row) {
          if (target_row >= nrows) continue;
          if (target_row < 0) break;
          for (target_col = (int)(col + _dist_surrounding); (int)(target_col + _dist_surrounding) >= col; --target_col) {
            if (target_col >= ncols) continue;
            if (target_col < 0) break;
            weight = weight_image.pixel(target_row, target_col);
            if (weight > 0.) {
              flag = true;
              break;
            }
          }
          if (flag) break;
        }
        _boundary_data[meta.index(row, col)] = (flag ? 1. : 0.);
        if (flag) ++n_surrounding_pixel;
      }
    }

    if (n_surrounding_pixel)
      surrounding_weight = 1. / (float)(n_surrounding_pixel);

    LARCV_INFO() << "Found " << n_surrounding_pixel << " pixels surrounding non-zero pixels ..." << std::endl;
    LARCV_INFO() << "Weight: " << surrounding_weight << std::endl;
    int ahoaho=0;
    for(auto const& v : _boundary_data) {
      if(v<1) continue;
      ahoaho += 1;
    }
    /*
    auto weight_data = weight_image.move();
    for (size_t idx = 0; idx < weight_data.size(); ++idx) {
      if(_boundary_data[idx]<1) continue;
      weight_data[idx] = surrounding_weight;
    }
*/

    auto weight_data = weight_image.move();
    // Next, compute "background pixels"
    size_t n_background_pixel = 0;
    for (auto const& v : weight_data) {
      if (v == 0.) n_background_pixel += 1;
    }
    float background_weight = 1. / (float)(n_background_pixel);
    LARCV_INFO() <<"Background pixel count " << n_background_pixel << " weight = " << background_weight<<std::endl;
    for (auto& v       : weight_data) {
      if (v == 0.) v = background_weight;
    }
    // Normalize
    float sum = 0.;
    for (auto const& v : weight_data) sum += v;
    for (auto& v       : weight_data) v /= sum;

    weight_image.move(std::move(weight_data));
    return weight_image;
  }

  bool Segment2DFromPDG::process(IOManager & mgr)
  {
    // Retrieve input
    auto const& event_image    = mgr.get_data<larcv::EventImage2D>        ( _input_producer    );
    auto const& event_cluster  = mgr.get_data<larcv::EventClusterPixel2D> ( _cluster_producer  );
    auto const& event_particle = mgr.get_data<larcv::EventParticle>       ( _particle_producer );

    for (size_t input_index = 0; input_index < event_image.as_vector().size(); ++input_index) {

      auto const& input_image = event_image.as_vector()[input_index];
      auto const& input_meta  = input_image.meta();
      auto const& cluster_v  = event_cluster.cluster_pixel_2d(input_meta.id());

      // Check if this projection should be skipped or not
      if (!_projection_v.empty()) {
        bool skip = true;
        for (auto const& p : _projection_v) {
          if (p != input_meta.id()) continue;
          skip = false;
          break;
        }
        if (skip) {
          LARCV_INFO() << "Skipping projection " << input_meta.id() << std::endl;
          continue;
        }
      }

      // Create output name suffix
      std::string suffix = "_projection" + std::to_string((int)(input_meta.id()));

      // Save output
      auto& output_image = mgr.get_data<larcv::EventImage2D> (_output_producer + suffix);
      output_image.append(input_image);

      // Create label image
      auto segment_image = CreateSegment2D(cluster_v, event_particle);
      // Mask label by data
      Mask(segment_image, input_image, 0.0, 0., false);
      // Save output
      auto& output_segment  = mgr.get_data<larcv::EventImage2D> (_label_producer  + suffix);
      output_segment.emplace(std::move(segment_image));

      // Create weight image (if requested)
      if (!_weight_producer.empty()) {
        // Ignore last cluster
        auto weight_image = CreateWeight2D(cluster_v, input_image);
        // Save output
        auto& output_weight = mgr.get_data<larcv::EventImage2D> (_weight_producer + suffix);
        output_weight.emplace(std::move(weight_image));
      }
    }

    return true;
  }

  void Segment2DFromPDG::finalize()
  {}

}
#endif
