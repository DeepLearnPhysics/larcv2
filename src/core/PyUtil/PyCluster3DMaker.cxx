#ifndef __PYCluster3DMAKER_CXX__
#define __PYCluster3DMAKER_CXX__

#include "PyCluster3DMaker.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

	static PyCluster3DMakerProcessFactory __global_PyCluster3DMakerProcessFactory__;

	PyCluster3DMaker::PyCluster3DMaker(const std::string name)
		: ProcessBase(name)
	{}

	void PyCluster3DMaker::configure(const PSet& cfg)
	{ _producer_name = cfg.get<std::string>("Cluster3DProducer"); }

	void PyCluster3DMaker::initialize()
	{
		_run = _subrun = _event = kINVALID_SIZE;
		_cluster3d_v.clear_data();
		_meta.clear();
	}

        void PyCluster3DMaker::append_ndarray(PyObject* img)
	{
		LARCV_INFO() << "Appending an Cluster3D (current size=" << _cluster3d_v.size() << ")" << std::endl;
		_cluster3d_v.emplace(std::move(as_tensor3d(img)));
	}

	bool PyCluster3DMaker::process(IOManager& mgr)
	{
	        auto& ev_output = mgr.get_data<larcv::EventClusterVoxel3D>(_producer_name);
		ev_output.emplace(std::move(_cluster3d_v),_meta);
		_cluster3d_v.clear_data();
		_meta.clear();
		return true;
	}

	void PyCluster3DMaker::finalize()
	{}

}
#endif
