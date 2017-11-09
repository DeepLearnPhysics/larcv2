#ifndef __PYTensor3DMAKER_CXX__
#define __PYTensor3DMAKER_CXX__

#include "PyTensor3DMaker.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

	static PyTensor3DMakerProcessFactory __global_PyTensor3DMakerProcessFactory__;

	PyTensor3DMaker::PyTensor3DMaker(const std::string name)
		: ProcessBase(name)
	{}

	void PyTensor3DMaker::configure(const PSet& cfg)
	{ _producer_name = cfg.get<std::string>("Tensor3DProducer"); }

	void PyTensor3DMaker::initialize()
	{
		_run = _subrun = _event = kINVALID_SIZE;
		_tensor3d_v.clear();
	}

	void PyTensor3DMaker::append_ndarray(PyObject* img, const Voxel3DMeta& meta)
	{
		LARCV_INFO() << "Appending an Tensor3D (current size=" << _tensor3d_v.size() << ")" << std::endl;
		_tensor3d_v.emplace_back(std::move(as_tensor3d(img)));
	}

	bool PyTensor3DMaker::process(IOManager& mgr)
	{
		return true;
	}

	void PyTensor3DMaker::finalize()
	{}

}
#endif