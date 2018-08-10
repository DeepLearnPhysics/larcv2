#ifndef __LARCV_PYUTILS_H__
#define __LARCV_PYUTILS_H__

struct _object;
typedef _object PyObject;

//#ifndef __CLING__
//#ifndef __CINT__
#include <Python.h>
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
//#include <numpy/ndarrayobject.h>
#include "numpy/arrayobject.h"
//#endif
//#endif

#include "larcv/core/DataFormat/Image2D.h"
#include "larcv/core/DataFormat/Voxel3D.h"
#include "larcv/core/DataFormat/Voxel3DMeta.h"

namespace larcv {
/// Utility function: call one-time-only numpy module initialization (you don't
/// have to call)
void SetPyUtil();
///
template <class T>
PyObject* _as_ndarray(const std::vector<T>& data);
PyObject* as_ndarray(const std::vector< short              > &data);
PyObject* as_ndarray(const std::vector< unsigned short     > &data);
PyObject* as_ndarray(const std::vector< int                > &data);
PyObject* as_ndarray(const std::vector< unsigned int       > &data);
PyObject* as_ndarray(const std::vector< long long          > &data);
PyObject* as_ndarray(const std::vector< unsigned long long > &data);
PyObject* as_ndarray(const std::vector< float              > &data);
PyObject* as_ndarray(const std::vector< double             > &data);
/// larcv::Image2D to numpy array converter
PyObject* as_ndarray(const Image2D &img);
/// larcv::Image2D to numpy array converter
PyObject* as_caffe_ndarray(const Image2D &img);
/// larcv::VoxelSet to numpy array converter
PyObject* as_ndarray(const SparseTensor3D &data, bool clear_mem=false);
/// copy array
template <class T>
void _copy_array(PyObject *arrayin, const std::vector<T> &cvec);
void copy_array(PyObject *arrayin, const std::vector< unsigned short > &cvec);
void copy_array(PyObject *arrayin, const std::vector< unsigned int   > &cvec);
void copy_array(PyObject *arrayin, const std::vector< short          > &cvec);
void copy_array(PyObject *arrayin, const std::vector< int            > &cvec);
void copy_array(PyObject *arrayin, const std::vector< long long      > &cvec);
void copy_array(PyObject *arrayin, const std::vector< float          > &cvec);
void copy_array(PyObject *arrayin, const std::vector< double         > &cvec);

/// Algorithm to extract three 1D arrays (x y and value) from 2D VoxelSet
void as_flat_arrays(const VoxelSet& tensor, const ImageMeta& meta,
		    PyObject* x, PyObject* y, PyObject* value);

/// Algorithm to extract four 1D arrays (x y z and value) from 3D VoxelSet
void as_flat_arrays(const VoxelSet& tensor, const Voxel3DMeta& meta,
		    PyObject* x, PyObject* y, PyObject* z, PyObject* value);

/// Algorithm to extract four 1D arrays (index and value) from 3D VoxelSet
void as_flat_arrays(const VoxelSet& tensor, const Voxel3DMeta& meta,
		    PyObject* index, PyObject* value);

//void copy_array(PyObject *arrayin, const std::vector<float> &cvec);
// void copy_array(PyObject *arrayin);//, const std::vector<float>& vec);

Image2D as_image2d_meta(PyObject *, ImageMeta meta);

Image2D as_image2d(PyObject *);

VoxelSet as_tensor2d(PyObject * values, PyObject * indexes);

VoxelSet as_tensor3d(PyObject *, float min_threshold=0);

// allows one to avoid some loops in python
void fill_img_col(Image2D &img, std::vector<short> &adcs, const int col,const float pedestal = 0.0);
                  //const int timedownsampling, const float pedestal = 0.0);

template <class T> int ctype_to_numpy();
template<> int ctype_to_numpy<short>();
template<> int ctype_to_numpy<unsigned short>();
template<> int ctype_to_numpy<int>();
template<> int ctype_to_numpy<unsigned int>();
template<> int ctype_to_numpy<long long>();
template<> int ctype_to_numpy<unsigned long long>();
template<> int ctype_to_numpy<float>();
template<> int ctype_to_numpy<double>();
template <class T> PyObject* numpy_array(std::vector<size_t> dims);
}

#endif
