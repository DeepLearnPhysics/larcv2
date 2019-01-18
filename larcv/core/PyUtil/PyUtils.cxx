#ifndef __LARCV_PYUTILS_CXX__
#define __LARCV_PYUTILS_CXX__

#include "PyUtils.h"
#include "larcv/core/Base/larcv_logger.h"
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
//#include <numpy/ndarrayobject.h>
#include "numpy/arrayobject.h"
#include <cassert>

namespace larcv {

void SetPyUtil() {
  static bool once = false;
  if (!once) {
    _import_array();
    once = true;
  }
}

PyObject *as_ndarray(const Image2D &img) {
  SetPyUtil();
  npy_intp dim_data[2];
  dim_data[0] = img.meta().cols();
  dim_data[1] = img.meta().rows();
  auto const &vec = img.as_vector();
  return PyArray_Transpose(((PyArrayObject*)(PyArray_SimpleNewFromData(2, dim_data, NPY_FLOAT, (char *)&(vec[0])))),NULL);
  //return PyArray_FromDimsAndData(2, dim_data, NPY_FLOAT, (char *)&(vec[0]));
}

PyObject *as_ndarray(const SparseTensor3D& data, bool clear_mem) {
  SetPyUtil();
  npy_intp dim_data[3];
  dim_data[0] = data.meta().num_voxel_x();
  dim_data[1] = data.meta().num_voxel_y();
  dim_data[2] = data.meta().num_voxel_z();

  static std::vector<float> local_data;
  local_data.resize(data.meta().size());
  for(auto &v : local_data) v = 0.;

  for(auto const& vox : data.as_vector())
    local_data[vox.id()] = vox.value();

  auto res = PyArray_Transpose(((PyArrayObject*)(PyArray_SimpleNewFromData(3, dim_data, NPY_FLOAT, (char *)&(local_data[0])))),NULL);
  //return PyArray_FromDimsAndData(2, dim_data, NPY_FLOAT, (char *)&(vec[0]));

  if(clear_mem) local_data.clear();
  return res;
}

PyObject *as_ndarray(const SparseTensor2D& data, bool clear_mem) {
  SetPyUtil();
  npy_intp dim_data[2];
  dim_data[0] = data.meta().cols();
  dim_data[1] = data.meta().rows();

  static std::vector<float> local_data;
  local_data.resize(data.meta().size());
  for(auto &v : local_data) v = 0.;

  for(auto const& vox : data.as_vector()) local_data[vox.id()]=vox.value();

  auto res = PyArray_Transpose(((PyArrayObject*)(PyArray_SimpleNewFromData(2, dim_data, NPY_FLOAT, (char *)&(local_data[0])))),NULL);
  //return PyArray_FromDimsAndData(2, dim_data, NPY_FLOAT, (char *)&(vec[0]));

  if(clear_mem) local_data.clear();
  return res;
}

/*
PyObject *as_ndarray(const SparseTensor2D& data, bool clear_mem) {
  SetPyUtil();
  PyObject* res = PyList_New(data.as_vector().size());
  for( size_t i=0; i<data.as_vector().size(); ++i) {
    npy_intp dim_data[2];
    dim_data[0] = data.meta().cols();
    dim_data[1] = data.meta().rows();

    static std::vector<float> local_data;
    local_data.resize(data.meta().size());
    for(auto &v : local_data) v = 0.;

    for(auto const& vox : data.as_vector()[i])
      local_data[vox.id()] = vox.value();

    auto ar = PyArray_Transpose(((PyArrayObject*)(PyArray_SimpleNewFromData(2, dim_data, NPY_FLOAT, (char *)&(vec[0])))),NULL);
    PyList_SetItem(res,Py_ssize_t(i),ar);
  }

  return res;
}
*/
void fill_3d_pcloud(const SparseTensor3D& data, PyObject* pyarray, PyObject* select) {
  SetPyUtil();

  float **carray;
  const int dtype = NPY_FLOAT;
  PyArray_Descr *descr = PyArray_DescrFromType(dtype);
  npy_intp dims[2];
  if (PyArray_AsCArray(&pyarray, (void **)&carray, dims, 2, descr) < 0) {
    logger::get("PyUtil").send(larcv::msg::kCRITICAL, __FUNCTION__, __LINE__,
			       "ERROR: cannot convert pyarray to 2D C-array");
    throw larbys();
  }

  size_t npts = data.size();
  int* select_ptr = nullptr;
  if(select) {
    auto select_pyptr = (PyArrayObject *)(select);
    // Check dimension size is 1:
    if (PyArray_NDIM(select_pyptr) != 1){
      logger::get("PyUtil").send(larcv::msg::kCRITICAL, __FUNCTION__, __LINE__,
				 "ERROR: select array must be 1D!");
      throw larbys();
    }
    if((int)npts < PyArray_SIZE(select_pyptr)) {
      logger::get("PyUtil").send(larcv::msg::kCRITICAL, __FUNCTION__, __LINE__,
				 "ERROR: select array size exceeds max data length!");
      throw larbys();
    }
    npts = PyArray_SIZE(select_pyptr);
    npy_intp loc[1];
    loc[0] = 0;
    select_ptr = (int*)(PyArray_GetPtr(select_pyptr,loc));
  }

  if(npts > data.size() || (dims[1] != 1 && dims[1] != 3 && dims[1] != 4)) {
    logger::get("PyUtil").send(larcv::msg::kCRITICAL,__FUNCTION__,__LINE__,
			       "ERROR: dimension mismatch");
    throw larbys();
  }

  auto const& vs = data.as_vector();

  for(size_t i=0; i<npts; ++i) {
    size_t index = i;
    if(select_ptr)
      index = select_ptr[i];
    
    auto const& vox = vs.at(index);
    auto pt = data.meta().position(vox.id());
    if(dims[1] == 1)
      carray[i][0] = vox.value();
    else if(dims[1] == 3) {
      carray[i][0] = pt.x;
      carray[i][1] = pt.y;
      carray[i][2] = pt.z;
    }
    if(dims[1] == 4) {
      carray[i][0] = pt.x;
      carray[i][1] = pt.y;
      carray[i][2] = pt.z;
      carray[i][3] = vox.value();
    }
  }

  return;
}

void fill_3d_voxels(const SparseTensor3D& data, PyObject* pyarray, PyObject* select) {
  SetPyUtil();

  int **carray;
  const int dtype = NPY_INT;
  PyArray_Descr *descr = PyArray_DescrFromType(dtype);
  npy_intp dims[2];
  if (PyArray_AsCArray(&pyarray, (void **)&carray, dims, 2, descr) < 0) {
    logger::get("PyUtil").send(larcv::msg::kCRITICAL, __FUNCTION__, __LINE__,
			       "ERROR: cannot convert pyarray to 2D C-array");
    throw larbys();
  }

  size_t npts = data.size();
  int* select_ptr = nullptr;
  if(select) {
    auto select_pyptr = (PyArrayObject *)(select);
    // Check dimension size is 1:
    if (PyArray_NDIM(select_pyptr) != 1){
      logger::get("PyUtil").send(larcv::msg::kCRITICAL, __FUNCTION__, __LINE__,
				 "ERROR: select array must be 1D!");
      throw larbys();
    }
    if((int)npts < PyArray_SIZE(select_pyptr)) {
      logger::get("PyUtil").send(larcv::msg::kCRITICAL, __FUNCTION__, __LINE__,
				 "ERROR: select array size exceeds max data length!");
      throw larbys();
    }
    npts = PyArray_SIZE(select_pyptr);
    npy_intp loc[1];
    loc[0] = 0;
    select_ptr = (int*)(PyArray_GetPtr(select_pyptr,loc));
  }

  if(npts > data.size() || dims[1] != 3 ) {
    logger::get("PyUtil").send(larcv::msg::kCRITICAL,__FUNCTION__,__LINE__,
			       "ERROR: dimension mismatch");
    throw larbys();
  }

  auto const& vs = data.as_vector();
  size_t ix,iy,iz;

  for(size_t i=0; i<npts; ++i) {
    size_t index = i;
    if(select_ptr)
      index = select_ptr[i];
    
    auto const& vox = vs.at(index);
    data.meta().id_to_xyz_index(vox.id(),ix,iy,iz);
    carray[i][0] = ix;
    carray[i][1] = iy;
    carray[i][2] = iz;
  }

  return;
}

void fill_2d_pcloud(const SparseTensor2D& data, PyObject* pyarray, PyObject* select) {
  SetPyUtil();

  float **carray;
  const int dtype = NPY_FLOAT;
  PyArray_Descr *descr = PyArray_DescrFromType(dtype);
  npy_intp dims[2];
  if (PyArray_AsCArray(&pyarray, (void **)&carray, dims, 2, descr) < 0) {
    logger::get("PyUtil").send(larcv::msg::kCRITICAL, __FUNCTION__, __LINE__,
			       "ERROR: cannot convert pyarray to 2D C-array");
    throw larbys();
  }

  size_t npts = data.size();
  int* select_ptr = nullptr;
  if(select) {
    auto select_pyptr = (PyArrayObject *)(select);
    // Check dimension size is 1:
    if (PyArray_NDIM(select_pyptr) != 1){
      logger::get("PyUtil").send(larcv::msg::kCRITICAL, __FUNCTION__, __LINE__,
				 "ERROR: select array must be 1D!");
      throw larbys();
    }
    if((int)npts < PyArray_SIZE(select_pyptr)) {
      logger::get("PyUtil").send(larcv::msg::kCRITICAL, __FUNCTION__, __LINE__,
				 "ERROR: select array size exceeds max data length!");
      throw larbys();
    }
    npts = PyArray_SIZE(select_pyptr);
    npy_intp loc[1];
    loc[0] = 0;
    select_ptr = (int*)(PyArray_GetPtr(select_pyptr,loc));
  }

  if(npts > data.size() || (dims[1] != 1 && dims[1] != 2 && dims[1] != 3)) {
    logger::get("PyUtil").send(larcv::msg::kCRITICAL,__FUNCTION__,__LINE__,
			       "ERROR: dimension mismatch");
    throw larbys();
  }

  auto const& vs = data.as_vector();

  for(size_t i=0; i<npts; ++i) {
    size_t index = i;
    if(select_ptr)
      index = select_ptr[i];
    
    auto const& vox = vs.at(index);
    auto pt = data.meta().position(vox.id());
    if(dims[1] == 1){
      carray[i][0] = vox.value();
    }
    else if(dims[1] == 2) {
      carray[i][0] = pt.x;
      carray[i][1] = pt.y;
    }
    if(dims[1] == 3) {
      carray[i][0] = pt.x;
      carray[i][1] = pt.y;
      carray[i][2] = vox.value();
    }
  }

  return;
}

void fill_2d_voxels(const SparseTensor2D& data, PyObject* pyarray, PyObject* select) {
  SetPyUtil();

  int **carray;
  const int dtype = NPY_INT;
  PyArray_Descr *descr = PyArray_DescrFromType(dtype);
  npy_intp dims[2];
  if (PyArray_AsCArray(&pyarray, (void **)&carray, dims, 2, descr) < 0) {
    logger::get("PyUtil").send(larcv::msg::kCRITICAL, __FUNCTION__, __LINE__,
			       "ERROR: cannot convert pyarray to 2D C-array");
    throw larbys();
  }

  size_t npts = data.size();
  int* select_ptr = nullptr;
  if(select) {
    auto select_pyptr = (PyArrayObject *)(select);
    // Check dimension size is 1:
    if (PyArray_NDIM(select_pyptr) != 1){
      logger::get("PyUtil").send(larcv::msg::kCRITICAL, __FUNCTION__, __LINE__,
				 "ERROR: select array must be 1D!");
      throw larbys();
    }
    if((int)npts < PyArray_SIZE(select_pyptr)) {
      logger::get("PyUtil").send(larcv::msg::kCRITICAL, __FUNCTION__, __LINE__,
				 "ERROR: select array size exceeds max data length!");
      throw larbys();
    }
    npts = PyArray_SIZE(select_pyptr);
    npy_intp loc[1];
    loc[0] = 0;
    select_ptr = (int*)(PyArray_GetPtr(select_pyptr,loc));
  }

  if(npts > data.size() || dims[1] != 2 ) {
    logger::get("PyUtil").send(larcv::msg::kCRITICAL,__FUNCTION__,__LINE__,
			       "ERROR: dimension mismatch");
    throw larbys();
  }

  auto const& vs = data.as_vector();
  size_t row,col;

  for(size_t i=0; i<npts; ++i) {
    size_t index = i;
    if(select_ptr)
      index = select_ptr[i];
    
    auto const& vox = vs.at(index);
    data.meta().index_to_rowcol(vox.id(),row,col);
    carray[i][0] = row;
    carray[i][1] = col;
  }

  return;
}

/*
void copy_array(PyObject *arrayin, const std::vector<float> &cvec) {
  SetPyUtil();
  PyArrayObject *ptr = (PyArrayObject *)(arrayin);

  //std::cout<< PyArray_NDIM(ptr) << std::endl
  //         << PyArray_DIM(ptr,0)<<std::endl
  //         << PyArray_SIZE(ptr) << std::endl;

  // Check dimension size is 1:
  if (PyArray_NDIM(ptr) != 1){
    throw std::exception();
  }

  if ((long)(cvec.size()) != PyArray_SIZE(ptr))
    throw std::exception();
  npy_intp loc[1];
  loc[0] = 0;
  auto fptr = (float *)(PyArray_GetPtr(ptr, loc));
  for (size_t i = 0; i < size_t(PyArray_SIZE(ptr)); ++i) {
    // std::cout << fptr[i] << std::endl;
    fptr[i] = cvec[i];
  };
}
*/

void as_flat_arrays(const VoxelSet& tensor, const ImageMeta& meta,
                    PyObject* x, PyObject* y, PyObject* value)
{
  SetPyUtil();
  PyArrayObject *xarr = (PyArrayObject*)(x);
  PyArrayObject *yarr = (PyArrayObject*)(y);
  PyArrayObject *varr = (PyArrayObject*)(value);

  auto const& voxel_v = tensor.as_vector();

  assert(PyArray_NDIM(xarr) == 1);
  assert(PyArray_NDIM(yarr) == 1);
  assert(PyArray_NDIM(varr) == 1);

  if(PyArray_SIZE(xarr) < (int)(voxel_v.size()) ||
     PyArray_SIZE(yarr) < (int)(voxel_v.size()) ||
     PyArray_SIZE(varr) < (int)(voxel_v.size())) {
    std::cerr << "PyArray size smaller than data size!" << std::endl;
    throw std::exception();
  }

  npy_intp loc[1];
  loc[0] = 0;
  auto xptr =   (int*)(PyArray_GetPtr(xarr, loc));
  auto yptr =   (int*)(PyArray_GetPtr(yarr, loc));
  auto vptr = (float*)(PyArray_GetPtr(varr, loc));

  size_t xpos, ypos;

  for (size_t i = 0; i < voxel_v.size(); ++i) {
    // std::cout << fptr[i] << std::endl;
    auto const& vox = voxel_v[i];
    meta.index_to_rowcol(vox.id(),ypos,xpos);
    xptr[i] = (int)(xpos);
    yptr[i] = (int)(ypos);
    vptr[i] = (float)(vox.value());
  };
}

void as_flat_arrays(const VoxelSet& tensor, const Voxel3DMeta& meta,
                    PyObject* x, PyObject* y, PyObject* z, PyObject* value)
{
  SetPyUtil();
  PyArrayObject *xarr = (PyArrayObject*)(x);
  PyArrayObject *yarr = (PyArrayObject*)(y);
  PyArrayObject *zarr = (PyArrayObject*)(z);
  PyArrayObject *varr = (PyArrayObject*)(value);

  auto const& voxel_v = tensor.as_vector();

  assert(PyArray_NDIM(xarr) == 1);
  assert(PyArray_NDIM(yarr) == 1);
  assert(PyArray_NDIM(zarr) == 1);
  assert(PyArray_NDIM(varr) == 1);

  if(PyArray_SIZE(xarr) < (int)(voxel_v.size()) ||
     PyArray_SIZE(yarr) < (int)(voxel_v.size()) ||
     PyArray_SIZE(zarr) < (int)(voxel_v.size()) ||
     PyArray_SIZE(varr) < (int)(voxel_v.size())) {
    std::cerr << "PyArray size smaller than data size!" << std::endl;
    throw std::exception();
  }

  npy_intp loc[1];
  loc[0] = 0;
  auto xptr =   (int*)(PyArray_GetPtr(xarr, loc));
  auto yptr =   (int*)(PyArray_GetPtr(yarr, loc));
  auto zptr =   (int*)(PyArray_GetPtr(zarr, loc));
  auto vptr = (float*)(PyArray_GetPtr(varr, loc));

  size_t xpos, ypos, zpos;

  for (size_t i = 0; i < voxel_v.size(); ++i) {
    // std::cout << fptr[i] << std::endl;
    auto const& vox = voxel_v[i];
    meta.id_to_xyz_index(vox.id(), xpos, ypos, zpos);
    xptr[i] = (int)(xpos);
    yptr[i] = (int)(ypos);
    zptr[i] = (int)(zpos);
    vptr[i] = (float)(vox.value());
  };
}

void as_flat_arrays(const VoxelSet& tensor, const Voxel3DMeta& meta,
                    PyObject* index, PyObject* value)
{
  SetPyUtil();
  PyArrayObject *iarr = (PyArrayObject*)(index);
  PyArrayObject *varr = (PyArrayObject*)(value);

  auto const& voxel_v = tensor.as_vector();

  assert(PyArray_NDIM(iarr) == 1);
  assert(PyArray_NDIM(varr) == 1);

  if(PyArray_SIZE(iarr) < (int)(voxel_v.size()) ||
     PyArray_SIZE(varr) < (int)(voxel_v.size())) {
    std::cerr << "PyArray size smaller than data size!" << std::endl;
    throw std::exception();
  }

  npy_intp loc[1];
  loc[0] = 0;
  auto iptr =   (int*)(PyArray_GetPtr(iarr, loc));
  auto vptr = (float*)(PyArray_GetPtr(varr, loc));

  //size_t xpos, ypos, zpos;

  for (size_t i = 0; i < voxel_v.size(); ++i) {
    // std::cout << fptr[i] << std::endl;
    auto const& vox = voxel_v[i];
    iptr[i] = vox.id();
    vptr[i] = vox.value();
  };
}

template<class T>
void _copy_array(PyObject *arrayin, const std::vector<T> &cvec) {
  SetPyUtil();
  PyArrayObject *ptr = (PyArrayObject *)(arrayin);

  //std::cout<< PyArray_NDIM(ptr) << std::endl
  //         << PyArray_DIM(ptr,0)<<std::endl
  //         << PyArray_SIZE(ptr) << std::endl;

  // Check dimension size is 1:
  if (PyArray_NDIM(ptr) != 1){
    throw std::exception();
  }

  if ((long)(cvec.size()) != PyArray_SIZE(ptr))
    throw std::exception();
  npy_intp loc[1];
  loc[0] = 0;
  auto fptr = (T *)(PyArray_GetPtr(ptr, loc));
  for (size_t i = 0; i < size_t(PyArray_SIZE(ptr)); ++i) {
    // std::cout << fptr[i] << std::endl;
    fptr[i] = cvec[i];
  };
}

template void _copy_array< unsigned short >(PyObject *arrayin, const std::vector< unsigned short > &cvec);
template void _copy_array< unsigned int   >(PyObject *arrayin, const std::vector< unsigned int   > &cvec);
template void _copy_array< short          >(PyObject *arrayin, const std::vector< short          > &cvec);
template void _copy_array< int            >(PyObject *arrayin, const std::vector< int            > &cvec);
template void _copy_array< long long      >(PyObject *arrayin, const std::vector< long long      > &cvec);
template void _copy_array< float          >(PyObject *arrayin, const std::vector< float          > &cvec);
template void _copy_array< double         >(PyObject *arrayin, const std::vector< double         > &cvec);

void copy_array(PyObject *arrayin, const std::vector< unsigned short > &cvec) { _copy_array(arrayin, cvec); }
void copy_array(PyObject *arrayin, const std::vector< unsigned int   > &cvec) { _copy_array(arrayin, cvec); }
void copy_array(PyObject *arrayin, const std::vector< short          > &cvec) { _copy_array(arrayin, cvec); }
void copy_array(PyObject *arrayin, const std::vector< int            > &cvec) { _copy_array(arrayin, cvec); }
void copy_array(PyObject *arrayin, const std::vector< long long      > &cvec) { _copy_array(arrayin, cvec); }
void copy_array(PyObject *arrayin, const std::vector< float          > &cvec) { _copy_array(arrayin, cvec); }
void copy_array(PyObject *arrayin, const std::vector< double         > &cvec) { _copy_array(arrayin, cvec); }

PyObject *as_caffe_ndarray(const Image2D &img) {
  SetPyUtil();
  npy_intp dim_data[2];
  dim_data[0] = img.meta().rows();
  dim_data[1] = img.meta().cols();
  auto const &vec = img.as_vector();
  return PyArray_SimpleNewFromData(2, dim_data, NPY_FLOAT, (char *)&(vec[0]));
}

larcv::Image2D as_image2d_meta(PyObject *pyarray, ImageMeta meta) {
  SetPyUtil();
  float **carray;
  // Create C arrays from numpy objects:
  const int dtype = NPY_FLOAT;
  PyArray_Descr *descr = PyArray_DescrFromType(dtype);
  npy_intp dims[2];
  if (PyArray_AsCArray(&pyarray, (void **)&carray, dims, 2, descr) < 0) {
    logger::get("PyUtil").send(larcv::msg::kCRITICAL, __FUNCTION__, __LINE__,
                               "ERROR: cannot convert to 2D C-array");
    throw larbys();
  }

  std::vector<float> res_data(dims[0] * dims[1], 0.);
  for (int i = 0; i < dims[0]; ++i) {
    for (int j = 0; j < dims[1]; ++j) {
      res_data[i + j * dims[0]] = (float)(carray[i][j]);
    }
  }
  PyArray_Free(pyarray, (void *)carray);

  Image2D res(std::move(meta), std::move(res_data));
  return res;
}

larcv::Image2D as_image2d(PyObject *pyarray) {
  SetPyUtil();
  float **carray;
  // Create C arrays from numpy objects:
  const int dtype = NPY_FLOAT;
  PyArray_Descr *descr = PyArray_DescrFromType(dtype);
  npy_intp dims[2];
  if (PyArray_AsCArray(&pyarray, (void **)&carray, dims, 2, descr) < 0) {
    logger::get("PyUtil").send(larcv::msg::kCRITICAL, __FUNCTION__, __LINE__,
                               "ERROR: cannot convert to 2D C-array\n");
    throw larbys();
  }

  std::vector<float> res_data(dims[0] * dims[1], 0.);
  for (int i = 0; i < dims[0]; ++i) {
    for (int j = 0; j < dims[1]; ++j) {
      res_data[i + j * dims[0]] = (float)(carray[i][j]);
    }
  }
  PyArray_Free(pyarray, (void *)carray);

  ImageMeta meta(0., 0., (double)(dims[0]), (double)(dims[1]),
                 (size_t)(dims[0]),
                 (size_t)(dims[1]),
                 larcv::kINVALID_PROJECTIONID);

  Image2D res(std::move(meta), std::move(res_data));
  return res;
}


larcv::VoxelSet as_tensor2d(PyObject * values_in, PyObject * indexes_in) {
  SetPyUtil();

  // Cast to pyarrayobject
  PyArrayObject *values  = (PyArrayObject *)(values_in);
  PyArrayObject *indexes = (PyArrayObject *)(indexes_in);

  // Dtype needs to be float32, uint64:
  const int dtype_values = NPY_FLOAT32;
  PyArray_Descr *descr_values = PyArray_DescrFromType(dtype_values);

  const int dtype_indexes = NPY_UINT64;
  PyArray_Descr *descr_indexes = PyArray_DescrFromType(dtype_indexes);

  // Each of values, indexes should come in as a 1D array.


  if (PyArray_NDIM(values) != 1 || PyArray_NDIM(indexes) != 1){
    LARCV_CRITICAL() << "Must feed flattened values and indexes to larcv.as_tensor2d!" << std::endl;
    throw larbys();
  }

  if (PyArray_SIZE(values) != PyArray_SIZE(indexes)){
    LARCV_CRITICAL() << "Values and Indexes do not have the same size!" << std::endl;
    throw larbys();
  }

  // // Require the right input values:
  // if (*PyArray_DTYPE(values) != * descr_values){
  //   LARCV_CRITICAL() << "Values must be type float 32!" << std::endl;
  //   throw larbys();
  // }
  // if (*PyArray_DTYPE(values) != * descr_indexes){
  //   LARCV_CRITICAL() << "Indexes must be type uint 64!" << std::endl;
  //   throw larbys();
  // }

  // Create C arrays from numpy objects:
  float * carray_values;

  npy_intp dims_values[1];
  if (PyArray_AsCArray(&values_in, (void *)&carray_values, dims_values, 1, descr_values) < 0) {
    logger::get("PyUtil").send(larcv::msg::kCRITICAL, __FUNCTION__, __LINE__,
                               "ERROR: cannot convert values to 1D C-array\n");
    throw larbys();
  }

  // Create C arrays from numpy objects:
  uint64_t * carray_indexes;

  npy_intp dims_indexes[1];
  if (PyArray_AsCArray(&indexes_in, (void *)&carray_indexes, dims_indexes, 1, descr_indexes) < 0) {
    logger::get("PyUtil").send(larcv::msg::kCRITICAL, __FUNCTION__, __LINE__,
                               "ERROR: cannot convert indexes to 1D C-array\n");
    throw larbys();
  }

  // Loop over them simultaneously and add voxels to a voxel set
  VoxelSet res;
  res.reserve(dims_values[0]);
  for (int i = 0; i < dims_values[0]; ++i){
    res.emplace(carray_indexes[i],carray_values[i],true);
  }

  PyArray_Free(values_in,  (void *)carray_values);
  PyArray_Free(indexes_in, (void *)carray_indexes);

  return res;
}

VoxelSet as_tensor3d(PyObject* pyarray, float min_threshold) {
  SetPyUtil();
  float ***carray;
  // Create C arrays from numpy objects:
  const int dtype = NPY_FLOAT;
  PyArray_Descr *descr = PyArray_DescrFromType(dtype);
  npy_intp dims[3];
  int ret = PyArray_AsCArray(&pyarray, (void***)&carray, dims, 3, descr);
  if ( ret < 0) {
    LARCV_CRITICAL() << "Cannot convert to 3D C-array (return code " << ret << ")" << std::endl;
    throw larbys();
  }
  VoxelSet res;
  float val=0.;
  size_t id = 0;
  for (int i = 0; i < dims[0]; ++i) {
    for (int j = 0; j < dims[1]; ++j) {
      for (int k = 0; k < dims[2]; ++k) {
        val = (float)(carray[i][j][k]);
        if(val <= min_threshold) continue;
        id = i * dims[1] * dims[2] + j * dims[2] + k;
        res.emplace(id,val,true);
      }
    }
  }
  PyArray_Free(pyarray, (void *)carray);

  return res;
}

VoxelSet as_tensor3d(PyObject* pyarray, const Voxel3DMeta& meta, float min_threshold) {
  SetPyUtil();
  float **carray;
  // Create C arrays from numpy objects:
  const int dtype = NPY_FLOAT;
  PyArray_Descr *descr = PyArray_DescrFromType(dtype);
  npy_intp dims[2];
  int ret = PyArray_AsCArray(&pyarray, (void**)&carray, dims, 2, descr);
  if ( ret < 0) {
    LARCV_CRITICAL() << "Cannot convert to 2D C-array (return code " << ret << ")" << std::endl;
    throw larbys();
  }
  if (dims[1] != 4) {
    LARCV_CRITICAL() << "The 2nd dimenstion must be length 4! (length " << dims[1] << ")" << std::endl;
    throw larbys();
  }
  VoxelSet res;
  double x,y,z;
  float v;
  //size_t id = 0;
  for (int i = 0; i < dims[0]; ++i) {
    x = (double)(carray[i][0]);
    y = (double)(carray[i][1]);
    z = (double)(carray[i][2]);
    v = (float )(carray[i][3]);
    if(v <= min_threshold) continue;
    res.emplace(meta.id(x,y,z),v,true);
  }

  PyArray_Free(pyarray, (void *)carray);

  return res;
}

VoxelSet as_tensor3d(PyObject* pos_array, PyObject* val_array, const Voxel3DMeta& meta, float min_threshold) {
  SetPyUtil();
  int **iarray;
  // Create C arrays from numpy objects:
  const int pos_dtype = NPY_INT;
  PyArray_Descr *pos_descr = PyArray_DescrFromType(pos_dtype);
  npy_intp pos_dims[2];
  int pos_ret = PyArray_AsCArray(&pos_array, (void**)&iarray, pos_dims, 2, pos_descr);
  if ( pos_ret < 0) {
    LARCV_CRITICAL() << "Cannot convert to 2D C-array (return code " << pos_ret << ")" << std::endl;
    throw larbys();
  }
  if (pos_dims[1] != 3) {
    LARCV_CRITICAL() << "The 2nd dimenstion must be length 3! (length " << pos_dims[1] << ")" << std::endl;
    throw larbys();
  }

  float *farray;
  const int val_dtype = NPY_FLOAT;
  PyArray_Descr *val_descr = PyArray_DescrFromType(val_dtype);
  npy_intp val_dims[1];
  int val_ret = PyArray_AsCArray(&val_array, (void*)&farray, val_dims, 1, val_descr);
  if ( val_ret < 0) {
    LARCV_CRITICAL() << "Cannot convert to 2D C-array (return code " << val_ret << ")" << std::endl;
    throw larbys();
  }
  if (pos_dims[0] != val_dims[0]) {
    LARCV_CRITICAL() << "The dimenstion mismatch (" << val_dims[0] << "!=" << pos_dims[0] << ")" << std::endl;
    throw larbys();
  }

  VoxelSet res;
  res.reserve(pos_dims[0]);
  int ix,iy,iz;
  float v;
  //size_t id = 0;
  for (int i = 0; i < pos_dims[0]; ++i) {
    ix = (int)(iarray[i][0]);
    iy = (int)(iarray[i][1]);
    iz = (int)(iarray[i][2]);
    v = (float )(farray[i]);
    if(v <= min_threshold) continue;
    res.emplace(meta.index(ix,iy,iz),v,true);
  }

  PyArray_Free(pos_array, (void *)iarray);
  PyArray_Free(val_array, (void *)farray);

  return res;
}

VoxelSet as_tensor3d(const SparseTensor3D& vs, PyObject* val_array, float min_threshold) {
  
  SetPyUtil();
  // Create C arrays from numpy objects:
  float *farray;
  const int val_dtype = NPY_FLOAT;
  PyArray_Descr *val_descr = PyArray_DescrFromType(val_dtype);
  npy_intp val_dims[1];
  int val_ret = PyArray_AsCArray(&val_array, (void*)&farray, val_dims, 1, val_descr);
  if ( val_ret < 0) {
    LARCV_CRITICAL() << "Cannot convert to 2D C-array (return code " << val_ret << ")" << std::endl;
    throw larbys();
  }
  if ((int)(vs.as_vector().size()) != val_dims[0]) {
    LARCV_CRITICAL() << "The dimenstion mismatch (" << val_dims[0] << "!=" << vs.as_vector().size() << ")" << std::endl;
    throw larbys();
  }

  VoxelSet res;
  res.reserve(vs.as_vector().size());
  float v;
  //size_t id = 0;
  auto const& voxel_v = vs.as_vector();
  for (size_t i = 0; i < voxel_v.size(); ++i) {
    v = (float )(farray[i]);
    if(v <= min_threshold) continue;
    res.emplace(voxel_v[i].id(),v,true);
  }

  PyArray_Free(val_array, (void *)farray);

  return res;
}

void fill_img_col(Image2D &img, std::vector<short> &adcs, const int col, const float pedestal)
//const int timedownsampling, const float pedestal) {
{
  if (col < 0 || col >= (long)(img.meta().cols()))
    return;

  for (int iadc = 0; iadc < (int)adcs.size(); iadc++) {
    if (iadc <= img.meta().min_y() || iadc >= img.meta().max_y())
      continue;
    int irow = img.meta().row(iadc);
    float val = img.pixel(irow, col);
    img.set_pixel(irow, col, val + ((float)adcs.at(iadc) - pedestal));
  }
}

template<> int ctype_to_numpy<short>() { SetPyUtil(); SetPyUtil(); return NPY_INT16; }
template<> int ctype_to_numpy<unsigned short>() { SetPyUtil(); return NPY_UINT16; }
template<> int ctype_to_numpy<int>() { SetPyUtil(); return NPY_INT32; }
template<> int ctype_to_numpy<unsigned int>() { SetPyUtil(); return NPY_UINT32; }
template<> int ctype_to_numpy<long long>() { SetPyUtil(); return NPY_INT64; }
template<> int ctype_to_numpy<unsigned long long>() { SetPyUtil(); return NPY_UINT64; }
template<> int ctype_to_numpy<float>() { SetPyUtil(); return NPY_FLOAT32; }
template<> int ctype_to_numpy<double>() { SetPyUtil(); return NPY_FLOAT64; }

/*
PyObject *as_ndarray(const std::vector<float> &vec) {
  SetPyUtil();

  if (vec.size() >= INT_MAX) {
    LARCV_CRITICAL() << "Length of data vector too long to specify ndarray. "
                        "Use by batch call."
                     << std::endl;
    throw larbys();
  }
  int nd = 1;
  npy_intp dims[1];
  dims[0] = (int)vec.size();
  PyArrayObject *array = (PyArrayObject *)PyArray_SimpleNewFromData(
      nd, dims, NPY_FLOAT, (char *)&(vec[0]));
  return PyArray_Return(array);
}
*/

template <class T>
PyObject *_as_ndarray(const std::vector<T> &vec) {
  SetPyUtil();

  if (vec.size() >= INT_MAX) {
    LARCV_CRITICAL() << "Length of data vector too long to specify ndarray. "
                        "Use by batch call."
                     << std::endl;
    throw larbys();
  }
  int nd = 1;
  npy_intp dims[1];
  dims[0] = (int)vec.size();
  PyArrayObject *array = (PyArrayObject *)PyArray_SimpleNewFromData(
      nd, dims, ctype_to_numpy<T>(), (char *)&(vec[0]));
  return PyArray_Return(array);
}

template PyObject* _as_ndarray< short              > (const std::vector< short              >& vec);
template PyObject* _as_ndarray< unsigned short     > (const std::vector< unsigned short     >& vec);
template PyObject* _as_ndarray< int                > (const std::vector< int                >& vec);
template PyObject* _as_ndarray< unsigned int       > (const std::vector< unsigned int       >& vec);
template PyObject* _as_ndarray< long long          > (const std::vector< long long          >& vec);
template PyObject* _as_ndarray< unsigned long long > (const std::vector< unsigned long long >& vec);
template PyObject* _as_ndarray< float              > (const std::vector< float              >& vec);
template PyObject* _as_ndarray< double             > (const std::vector< double             >& vec);

PyObject* as_ndarray(const std::vector< short              >& vec) { return _as_ndarray< short              >(vec); }
PyObject* as_ndarray(const std::vector< unsigned short     >& vec) { return _as_ndarray< unsigned short     >(vec); }
PyObject* as_ndarray(const std::vector< int                >& vec) { return _as_ndarray< int                >(vec); }
PyObject* as_ndarray(const std::vector< unsigned int       >& vec) { return _as_ndarray< unsigned int       >(vec); }
PyObject* as_ndarray(const std::vector< long long          >& vec) { return _as_ndarray< long long          >(vec); }
PyObject* as_ndarray(const std::vector< unsigned long long >& vec) { return _as_ndarray< unsigned long long >(vec); }
PyObject* as_ndarray(const std::vector< float              >& vec) { return _as_ndarray< float              >(vec); }
PyObject* as_ndarray(const std::vector< double             >& vec) { return _as_ndarray< double             >(vec); }

template<class T>
PyObject* numpy_array(std::vector<size_t> dims)
{
  SetPyUtil();
  int nd_ = dims.size();
  npy_intp dims_[nd_];
  for(size_t i=0; i<dims.size(); ++i) dims_[i] = dims[i];
  /*
  std::cout<<"NUMPY TYPE " << ctype_to_numpy<T>() << std::endl;
  std::cout<<"DIMS " << dims.size() << std::endl;
  std::cout<<"ND " << nd_ << std::endl;
  std::cout<<"Shape ";
  for(size_t i=0; i<dims.size(); ++i) std::cout<< " " << dims_[i];
  std::cout<<std::endl;
  */
  //PyObject* res = PyArray_SimpleNew(nd_,dims_,ctype_to_numpy<T>());
  PyObject* res = PyArray_ZEROS(nd_,dims_,ctype_to_numpy<T>(),0);
  //Py_INCREF(res);
  /*
  std::cout<<PyArray_NDIM((PyArrayObject*)res) << std::endl;
  std::cout<<PyArray_SIZE((PyArrayObject*)res) << std::endl;
  */
  PyArrayObject *ptr = (PyArrayObject*)(res);
  // Check dimension size is 1:
  //std::cout<<"ndim " << PyArray_NDIM(ptr) << std::endl;
  //size_t len = PyArray_SIZE(ptr);
  //std::cout<<"len " << len <<std::endl;
  //npy_intp loc[1];
  //loc[0] = 0;
  //auto fptr = (T *)(PyArray_GetPtr(ptr, loc));
  /*
  std::cout<<"fptr " << fptr << std::endl;
  for (size_t i = 0; i < len; ++i) {
    std::cout << fptr[i] << std::endl;
    fptr[i] = T(1);
  }
  */
  PyArray_INCREF(ptr);

  return res;
}

template PyObject* numpy_array<float>(std::vector<size_t>dims);

}

#endif
