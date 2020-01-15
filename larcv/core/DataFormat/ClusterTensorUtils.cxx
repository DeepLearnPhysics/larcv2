#ifndef LARCV_CLUSTERTENSORUTILS_CXX
#define LARCV_CLUSTERTENSORUTILS_CXX

#include "ClusterTensorUtils.h"

namespace larcv {

  larcv::Image2D as_image2d(const SparseTensor2D& tensor)
  { return as_image2d((VoxelSet)tensor,tensor.meta()); }
  
  larcv::Image2D as_image2d(const VoxelSet& tensor, const ImageMeta& meta)
  {
    std::vector<float> data(meta.size(),0.);
    for(auto const& voxel : tensor.as_vector())
      data[voxel.id()] = voxel.value();
    
    return Image2D(std::move(ImageMeta(meta)),std::move(data));
  }

  larcv::SparseTensor2D as_sparse_tensor2d(const ClusterPixel2D& clusters)
  {
    VoxelSet vs;
    for(auto const& cluster : clusters.as_vector()) {
      for(auto const& voxel : cluster.as_vector())
	{ vs.add(voxel); }
    }
    return SparseTensor2D(std::move(vs),std::move(ImageMeta(clusters.meta())));
  }

  larcv::Image2D as_image2d(const ClusterPixel2D& clusters)
  {
    std::vector<float> data(clusters.meta().size(),0.);
    for(auto const& cluster : clusters.as_vector()) {
      for(auto const& voxel : cluster.as_vector())
	{ data[voxel.id()] = voxel.value(); }
    }

    return Image2D(std::move(ImageMeta(clusters.meta())),std::move(data));
  }

  larcv::SparseTensor3D as_sparse_tensor3d(const ClusterVoxel3D& clusters)
  {
    VoxelSet vs;
    for(auto const& cluster : clusters.as_vector()) {
      for(auto const& voxel : cluster.as_vector())
	{ vs.add(voxel); }
    }
    return SparseTensor3D(std::move(vs),std::move(Voxel3DMeta(clusters.meta())));
  }

  FlatTensorContents as_flat_arrays(const VoxelSet& tensor, const ImageMeta& meta)
  {
    FlatTensorContents res;
    res.x.resize(tensor.size());
    res.y.resize(tensor.size());
    res.z.resize(0);
    res.value.resize(tensor.size());

    size_t x,y;
    auto const& vox_array = tensor.as_vector();
    for(size_t i=0; i<tensor.as_vector().size(); ++i) {
      auto const& vox = vox_array[i];
      meta.index_to_rowcol(vox.id(),y,x);
      res.x[i] = x;
      res.y[i] = y;
      res.value[i] = vox.value();
    }
    return res;
  }

  FlatTensorContents as_flat_arrays(const VoxelSet& tensor, const Voxel3DMeta& meta)
  {
    FlatTensorContents res;
    res.x.resize(tensor.size());
    res.y.resize(tensor.size());
    res.z.resize(tensor.size());
    res.value.resize(tensor.size());

    size_t x,y,z;
    auto const& vox_array = tensor.as_vector();
    for(size_t i=0; i<tensor.as_vector().size(); ++i) {
      auto const& vox = vox_array[i];
      meta.id_to_xyz_index(vox.id(),x,y,z);
      res.x[i] = x;
      res.y[i] = y;
      res.z[i] = z;
      res.value[i] = vox.value();
    }
    return res;
  }

  larcv::EventSparseTensor3D generate_semantics(const EventClusterVoxel3D& clusters, const EventParticle& particles) {

    auto const& cluster_v  = clusters.as_vector();
    auto const& particle_v = particles.as_vector();
    
    larcv::VoxelSet res;
    size_t total_size=0;
    for(auto const& vs : cluster_v) total_size += vs.size();
    res.reserve(total_size);

    std::vector<size_t> part_idx_v;
    part_idx_v.reserve(particle_v.size());
    // create a reverse-ordered shape type
    std::vector<larcv::ShapeType_t> ordered_type_v = {larcv::kShapeLEScatter,
						      larcv::kShapeDelta,
						      larcv::kShapeMichel,
						      larcv::kShapeTrack,
						      larcv::kShapeShower};
    // loop over shape type + loop over particles to order particles in the reverse of preferred semantics
    for(auto const& shape_type : ordered_type_v) {
      for(size_t idx=0; idx<particle_v.size(); ++idx)
	if(particle_v[idx].shape() == shape_type) part_idx_v.push_back(idx);
    }

    // now loop over index + paint the voxels with semantic types.
    // later particle type override the semantic type
    for(auto const& idx : part_idx_v) {
      auto const& vs = cluster_v[idx].as_vector();
      auto const& particle = particle_v[idx];
      float semantic = particle.shape();
      if(particle.pdg_code()==2212) semantic = larcv::kShapeUnknown;
      
      for(auto const& vox : vs)
	res.emplace(vox.id(),semantic,false);
    }

    larcv::EventSparseTensor3D tensor3d;
    tensor3d.emplace(std::move(res),clusters.meta());
    return tensor3d;
  }

  
}
#endif
