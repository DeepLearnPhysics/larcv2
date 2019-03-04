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
		_tensor3d.clear_data();
		_meta = Voxel3DMeta();
	}

        void PyTensor3DMaker::append_ndarray(PyObject* img)
	{
		_tensor3d = std::move(as_tensor3d(img));
	}

	bool PyTensor3DMaker::process(IOManager& mgr)
	{
	        auto& ev_output = mgr.get_data<larcv::EventSparseTensor3D>(_producer_name);
		ev_output.emplace(std::move(_tensor3d),_meta);
		_tensor3d.clear_data();
		_meta = Voxel3DMeta();
		return true;
	}

	void PyTensor3DMaker::finalize()
	{}

}
#endif
