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
    import_array();
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

  size_t xpos, ypos, zpos;

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
