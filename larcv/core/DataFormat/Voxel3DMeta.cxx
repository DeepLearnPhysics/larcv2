#ifndef __LARCV_VOXEL3DMETA_CXX__
#define __LARCV_VOXEL3DMETA_CXX__

#include "Voxel3DMeta.h"
#include "larcv/core/Base/larbys.h"
#include <algorithm>
#include <sstream>
namespace larcv {

  Voxel3DMeta::Voxel3DMeta()
  { clear(); }

  void Voxel3DMeta::clear()
  {
    (BBox3D)(*this) = BBox3D();
    _valid = false;
    _xnum = _ynum = _znum = 0;
    _xlen = _ylen = _zlen = -1.;
    _unit = kUnitUnknown;
  }

  void Voxel3DMeta::update(size_t xnum, size_t ynum, size_t znum) {

    if(empty())
      throw larbys("Empty voxel volume definition cannot be updated with new voxel counts!");

    if(xnum == kINVALID_SIZE || xnum == 0)
      throw larbys("Voxel3DMeta::Set x voxel count not set!");

    if(ynum == kINVALID_SIZE || ynum == 0)
      throw larbys("Voxel3DMeta::Set y voxel count not set!");

    if(znum == kINVALID_SIZE || znum == 0)
      throw larbys("Voxel3DMeta::Set z voxel count not set!");

    _xlen = (max_x() - min_x()) / ((double)xnum);
    _xnum = xnum;

    _ylen = (max_y() - min_y()) / ((double)ynum);
    _ynum = ynum;

    _zlen = (max_z() - min_z()) / ((double)znum);
    _znum = znum;

    _num_element = _xnum * _ynum * _znum;
    _valid = true;
  }

  VoxelID_t Voxel3DMeta::id(double x, double y, double z) const
  {
    if(!_valid) throw larbys("Voxel3DMeta::ID cannot be called on invalid meta!");
    if(x > max_x() || x < min_x()) return kINVALID_VOXELID;
    if(y > max_y() || y < min_y()) return kINVALID_VOXELID;
    if(z > max_z() || z < min_z()) return kINVALID_VOXELID;

    VoxelID_t xindex = (x - min_x()) / _xlen;
    VoxelID_t yindex = (y - min_y()) / _ylen;
    VoxelID_t zindex = (z - min_z()) / _zlen;

    if(xindex == _xnum) xindex -= 1;
    if(yindex == _ynum) yindex -= 1;
    if(zindex == _znum) zindex -= 1;

    return (zindex * (_xnum * _ynum) + yindex * _xnum + xindex);
  }
  VoxelID_t Voxel3DMeta::index(const size_t i_x, const size_t i_y, const size_t i_z) const
  {
    if (!_valid) throw larbys("Voxel3DMeta::ID cannot be called on invalid meta!");
    if (i_x >= _xnum) return kINVALID_VOXELID;
    if (i_y >= _ynum) return kINVALID_VOXELID;
    if (i_z >= _znum) return kINVALID_VOXELID;

    return (i_z * (_xnum * _ynum) + i_y * _xnum + i_x);

  }

  VoxelID_t Voxel3DMeta::shift(const VoxelID_t origin_id,
                               const int shift_x,
                               const int shift_y,
                               const int shift_z) const
  {
    int id = origin_id;
    int zid = id / (_xnum * _ynum);
    id -= zid * (_xnum * _ynum);
    zid += shift_z;
    if(zid < 0 || zid >= (int)_znum) return kINVALID_VOXELID;

    int yid = id / _xnum;
    int xid = id - yid * _xnum;

    yid += shift_y;
    if(yid < 0 || yid >= (int)_ynum) return kINVALID_VOXELID;

    xid += shift_x;
    if(xid < 0 || xid >= (int)_xnum) return kINVALID_VOXELID;

    return (zid * (_xnum * _ynum) + yid * _xnum + xid);
  }

  Point3D Voxel3DMeta::position(VoxelID_t id) const
  {
    if(!_valid) throw larbys("Voxel3DMeta::pos cannot be called on invalid meta!");
    if(id >= _num_element) throw larbys("Voxel3DMeta::pos invalid VoxelID_t!");

    VoxelID_t zid = id / (_xnum * _ynum);
    id -= zid * (_xnum * _ynum);
    VoxelID_t yid = id / _xnum;
    VoxelID_t xid = (id - yid * _xnum);

    return Point3D(min_x() + ((double)xid + 0.5) * _xlen,
		   min_y() + ((double)yid + 0.5) * _ylen,
		   min_z() + ((double)zid + 0.5) * _zlen);
  }

  double Voxel3DMeta::pos_x(VoxelID_t id) const
  {
    if(!_valid) throw larbys("Voxel3DMeta::pos_x cannot be called on invalid meta!");
    if(id >= _num_element) throw larbys("Voxel3DMeta::pos_x invalid VoxelID_t!");

    VoxelID_t zid = id / (_xnum * _ynum);
    id -= zid * (_xnum * _ynum);
    VoxelID_t yid = id / _xnum;
    VoxelID_t xid = (id - yid * _xnum);

    return min_x() + ((double)xid + 0.5) * _xlen;
  }

  double Voxel3DMeta::pos_y(VoxelID_t id) const
  {
    if(!_valid) throw larbys("Voxel3DMeta::pos_y cannot be called on invalid meta!");
    if(id >= _num_element) throw larbys("Voxel3DMeta::pos_y invalid VoxelID_t!");

    VoxelID_t zid = id / (_xnum * _ynum);
    id -= zid * (_xnum * _ynum);
    VoxelID_t yid = id / _xnum;
    return min_y() + ((double)yid + 0.5) * _ylen;
  }

  double Voxel3DMeta::pos_z(VoxelID_t id) const
  {
    if(!_valid) throw larbys("Voxel3DMeta::pos_z cannot be called on invalid meta!");
    if(id >= _num_element) throw larbys("Voxel3DMeta::pos_z invalid VoxelID_t!");

    VoxelID_t zid = id / (_xnum * _ynum);
    return min_z() + ((double)zid + 0.5) * _zlen;
  }

  size_t Voxel3DMeta::id_to_x_index(VoxelID_t id) const
  {
    if(id >= _num_element) throw larbys("Voxel3DMeta::pos invalid VoxelID_t!");

    VoxelID_t zid = id / (_xnum * _ynum);
    id -= zid * (_xnum * _ynum);
    VoxelID_t yid = id / _xnum;
    VoxelID_t xid = (id - yid * _xnum);

    return xid;
  }

  size_t Voxel3DMeta::id_to_y_index(VoxelID_t id) const
  {
    if(id >= _num_element) throw larbys("Voxel3DMeta::pos invalid VoxelID_t!");

    VoxelID_t zid = id / (_xnum * _ynum);
    id -= zid * (_xnum * _ynum);
    VoxelID_t yid = id / _xnum;

    return yid;
  }


  size_t Voxel3DMeta::id_to_z_index(VoxelID_t id) const
  {
    if(id >= _num_element) throw larbys("Voxel3DMeta::pos invalid VoxelID_t!");

    VoxelID_t zid = id / (_xnum * _ynum);

    return zid;
  }

  void Voxel3DMeta::id_to_xyz_index(VoxelID_t id, size_t& x, size_t& y, size_t& z) const
  {
    if(id >= _num_element) throw larbys("Voxel3DMeta::pos invalid VoxelID_t!");
    z = id / (_xnum * _ynum);
    id -= z * (_xnum * _ynum);
    y = id / _xnum;
    x = (id - y * _xnum);
  }

  std::string  Voxel3DMeta::dump() const
  {
    std::stringstream ss;
    ss << "X range: " << min_x() << " => " << max_x() << " ... " << _xnum << " bins" << std::endl
       << "Y range: " << min_y() << " => " << max_y() << " ... " << _ynum << " bins" << std::endl
       << "Z range: " << min_z() << " => " << max_z() << " ... " << _znum << " bins" << std::endl;
    return std::string(ss.str());
  }

};

#endif
