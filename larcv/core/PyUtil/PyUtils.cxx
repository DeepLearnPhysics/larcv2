#ifndef __LARCV_PYUTILS_CXX__
#define __LARCV_PYUTILS_CXX__

#include "PyUtils.h"
#include "larcv/core/Base/larcv_logger.h"
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
//#include <numpy/ndarrayobject.h>
#include "numpy/arrayobject.h"

namespace larcv {

void SetPyUtil() {
  static bool once = false;
  if (!once) {
    import_array();
    once = true;
  }
}

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

PyObject *as_ndarray(const Image2D &img) {
  SetPyUtil();
  npy_intp dim_data[2];
  dim_data[0] = img.meta().cols();
  dim_data[1] = img.meta().rows();
  auto const &vec = img.as_vector();
  return PyArray_Transpose(((PyArrayObject*)(PyArray_SimpleNewFromData(2, dim_data, NPY_FLOAT, (char *)&(vec[0])))),NULL);
  //return PyArray_FromDimsAndData(2, dim_data, NPY_FLOAT, (char *)&(vec[0]));
}

void copy_array(PyObject *arrayin, const std::vector<float> &cvec) {
  SetPyUtil();
  PyArrayObject *ptr = (PyArrayObject *)(arrayin);
  /*
  std::cout<< PyArray_NDIM(ptr) << std::endl
           << PyArray_DIM(ptr,0)<<std::endl
           << PyArray_SIZE(ptr) << std::endl;
  */

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

VoxelSet as_tensor3d(PyObject* pyarray, float min_threshold) {
  SetPyUtil();
  float ***carray;
  // Create C arrays from numpy objects:
  const int dtype = NPY_FLOAT;
  PyArray_Descr *descr = PyArray_DescrFromType(dtype);
  npy_intp dims[4];
  int ret = PyArray_AsCArray(&pyarray, (void***)&carray, dims, 3, descr);
  if ( ret < 0) {
    LARCV_CRITICAL() << "Cannot convert to 3D C-array (return code " << ret << ")" << std::endl;
    throw larbys();
  }
  std::cout << dims[0] << " " << dims[1] << " " << dims[2] << std::endl;
  std::vector<float> res_data(dims[0] * dims[1] * dims[2], 0.);
  float val=0.;
  for (int i = 0; i < dims[0]; ++i) {
    for (int j = 0; j < dims[1]; ++j) {
      for (int k = 0; k < dims[2]; ++k) {
        val = (float)(carray[i][j][k]);
        std::cout << "(k,j,i) = (" << k << "," << j << "," << i << ") @ " << val << std::endl;
      }
    }
  }
  PyArray_Free(pyarray, (void *)carray);

  return VoxelSet();
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
}

#endif
