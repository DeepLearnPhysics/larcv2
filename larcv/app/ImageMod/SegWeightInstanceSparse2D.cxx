#ifndef __SegWeightInstanceSparse2D_CXX__
#define __SegWeightInstanceSparse2D_CXX__

#include "SegWeightInstanceSparse2D.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"

namespace larcv {

	static SegWeightInstanceSparse2DProcessFactory __global_SegWeightInstanceSparse2DProcessFactory__;

	SegWeightInstanceSparse2D::SegWeightInstanceSparse2D(const std::string name)
		: ProcessBase(name)
	{}

	void SegWeightInstanceSparse2D::configure(const PSet& cfg)
	{

		_cluster2d_producer = cfg.get<std::string>("Cluster2DProducer");

		_output_producer = cfg.get<std::string>("OutputProducer");

		_projection_v = cfg.get<std::vector<size_t> >("ProjectionID");

		_weight_max = cfg.get<double>("WeightMax");

		_weight_min = cfg.get<double>("WeightMin");


		if (_cluster2d_producer.empty()) {
			LARCV_CRITICAL() << "Cluster2D producer empty!" << std::endl;
			throw larbys();
		}

		if (_output_producer.empty()) {
			LARCV_CRITICAL() << "Output producer empty!" << std::endl;
			throw larbys();
		}

		if (_projection_v.empty()) {
			LARCV_CRITICAL() << "No projection ID requested for process" << std::endl;
			throw larbys();
		}

		_pool_type = (PoolType_t)(cfg.get<unsigned short>("PoolType", (unsigned short)kSumPool));

	}

	void SegWeightInstanceSparse2D::initialize()
	{}

	bool SegWeightInstanceSparse2D::process(IOManager& mgr)
	{
		// get isntance data
		auto const& event_cluster2d_v = mgr.get_data<larcv::EventClusterPixel2D>(_cluster2d_producer);
		// create output holder
		auto& event_output_v = mgr.get_data<larcv::EventSparseTensor2D>(_output_producer);
		std::vector<larcv::VoxelSet> out_vs_v;
		std::vector<larcv::ImageMeta> out_meta_v;

		// make sure projection id is available
		size_t max_projection_id = 0;
		for (auto const& projection : _projection_v) {
			max_projection_id = std::max(max_projection_id, (size_t)(projection));
			if (projection < event_cluster2d_v.as_vector().size()) continue;
			LARCV_CRITICAL() << "projection ID " << projection << " not found!" << std::endl;
			throw larbys();
		}

		// resize output
		out_vs_v.resize(max_projection_id+1);
		out_meta_v.resize(max_projection_id+1);

		// loop over specified projections
		for (auto const& projection : _projection_v) {

			auto const& cluster2d_v = event_cluster2d_v.as_vector()[projection];
			auto const& meta = cluster2d_v.meta();
			auto& out_vs = out_vs_v[projection];
			out_meta_v[projection] = meta;

			// compute weights 
			size_t vox_counts=0;
			std::vector<double> weights_v(cluster2d_v.size(),0.);
			for(size_t cluster_idx=0; cluster_idx<cluster2d_v.size(); ++cluster_idx) {
				size_t cluster_size = cluster2d_v.as_vector()[cluster_idx].size();
				if(cluster_size<1) continue;
				double weight = 1./ (double)(cluster_size);
				weights_v[cluster_idx] = std::max(std::min(_weight_max,weight),_weight_min);
				LARCV_INFO() << "Cluster " << cluster_idx
					     << " size " << cluster_size
					     << " weights " << weights_v[cluster_idx] << std::endl;
				vox_counts += cluster_size;
			}
			out_vs.reserve(vox_counts);

			//
			// Combine weights
			//
			if(_pool_type == kSumPool) {
			    for (size_t cluster_idx=0; cluster_idx < cluster2d_v.size(); ++cluster_idx ) {
			    	auto const& cluster2d = cluster2d_v.as_vector()[cluster_idx];
			    	auto const& weight = weights_v[cluster_idx];
			    	for(auto const& vox : cluster2d.as_vector())
			    		out_vs.emplace(vox.id(),weight,true);
			    }
			}else if(_pool_type == kMaxPool) {
			    for (size_t cluster_idx=0; cluster_idx < cluster2d_v.size(); ++cluster_idx ) {
			    	auto const& cluster2d = cluster2d_v.as_vector()[cluster_idx];
			    	auto const& weight = weights_v[cluster_idx];
			    	for(auto const& vox : cluster2d.as_vector()) {
			    		auto const& prev_vox = out_vs.find(vox.id());
			    		if(prev_vox.id() == larcv::kINVALID_VOXELID || prev_vox.value() < weight)
			    			out_vs.emplace(vox.id(),weight,false);
			    	}
			    }
			}else if(_pool_type == kAveragePool) {
			    larcv::VoxelSet vs_counts;
			    larcv::VoxelSet vs_sum;
			    vs_counts.reserve(vox_counts);
			    vs_sum.reserve(vox_counts);
			    for (size_t cluster_idx=0; cluster_idx < cluster2d_v.size(); ++cluster_idx ) {
			    	auto const& cluster2d = cluster2d_v.as_vector()[cluster_idx];
			    	auto const& weight = weights_v[cluster_idx];
			    	for(auto const& vox : cluster2d.as_vector()) {
			    		vs_sum.emplace(vox.id(),weight,true);
			    		vs_counts.emplace(vox.id(),1.,true);
			    	}
			    }
			    for(size_t vox_idx=0; vox_idx<vs_counts.size(); ++vox_idx) {
			    	auto const& vox_sum = vs_sum.as_vector()[vox_idx];
			    	auto const& vox_count = vs_counts.as_vector()[vox_idx];
			    	out_vs.emplace(vox_sum.id(), vox_sum.value() / vox_count.value(), false);
			    }
			}else if(_pool_type == kOverwrite) {
			    for (size_t cluster_idx=0; cluster_idx < cluster2d_v.size(); ++cluster_idx ) {
			    	auto const& cluster2d = cluster2d_v.as_vector()[cluster_idx];
			    	auto const& weight = weights_v[cluster_idx];
			    	for(auto const& vox : cluster2d.as_vector())
			    		out_vs.emplace(vox.id(),weight,false);
			    }
			}else{
				LARCV_CRITICAL() << "Unsupported pooling type: " << _pool_type << std::endl;
				throw larbys();
			}

		}

		for(size_t i=0; i<out_vs_v.size(); ++i)
			event_output_v.emplace(std::move(out_vs_v[i]),std::move(out_meta_v[i]));

		return true;
	}

	void SegWeightInstanceSparse2D::finalize()
	{}

}
#endif
