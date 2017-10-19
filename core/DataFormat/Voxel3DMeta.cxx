#ifndef __LARCV_VOXEL3DMETA_CXX__
#define __LARCV_VOXEL3DMETA_CXX__

#include "Voxel3DMeta.h"
#include "Base/larbys.h"
#include <algorithm>
#include <sstream>
namespace larcv {

  Voxel3DMeta::Voxel3DMeta()
  { Clear(); }

  void Voxel3DMeta::Clear()
  {
    _xmin = _xmax = _xlen = kINVALID_DOUBLE;
    _ymin = _ymax = _ylen = kINVALID_DOUBLE;
    _zmin = _zmax = _zlen = kINVALID_DOUBLE;
    _xnum = _ynum = _znum = kINVALID_SIZE;
    _valid = false;
  }
  
  void Voxel3DMeta::Set(double xmin, double xmax,
			double ymin, double ymax,
			double zmin, double zmax,
			size_t xnum,
			size_t ynum,
			size_t znum)
  {
    if(xmin == kINVALID_DOUBLE || xmax == kINVALID_DOUBLE)
      throw larbys("Voxel3DMeta::Set x boundary not set!");
    if(xmin >= xmax)
      throw larbys("Voxel3DMeta::Set xmin >= xmax!");

    if(ymin == kINVALID_DOUBLE || ymax == kINVALID_DOUBLE)
      throw larbys("Voxel3DMeta::Set y boundary not set!");
    if(ymin >= ymax)
      throw larbys("Voxel3DMeta::Set ymin >= ymax!");

    if(zmin == kINVALID_DOUBLE || zmax == kINVALID_DOUBLE)
      throw larbys("Voxel3DMeta::Set z boundary not set!");
    if(zmin >= zmax)
      throw larbys("Voxel3DMeta::Set zmin >= zmax!");

    if(xnum == kINVALID_SIZE || xnum == 0)
      throw larbys("Voxel3DMeta::Set x voxel count not set!");
    
    if(ynum == kINVALID_SIZE || ynum == 0)
      throw larbys("Voxel3DMeta::Set y voxel count not set!");
    
    if(znum == kINVALID_SIZE || znum == 0)
      throw larbys("Voxel3DMeta::Set z voxel count not set!");

    _xmin = xmin;
    _xmax = xmax;
    _xlen = (xmax - xmin) / ((double)xnum);
    _xnum = xnum;

    _ymin = ymin;
    _ymax = ymax;
    _ylen = (ymax - ymin) / ((double)ynum);
    _ynum = ynum;

    _zmin = zmin;
    _zmax = zmax;
    _zlen = (zmax - zmin) / ((double)znum);
    _znum = znum;
    
    if( (_xmin + _xlen * _xnum) != _xmax )
      throw larbys("Voxel3DMeta::Set (xmax - xmin) not divisible by xnum!");
    
    if( (_ymin + _ylen * _ynum) != _ymax )
      throw larbys("Voxel3DMeta::Set (ymax - ymin) not divisible by ynum!");

    if( (_zmin + _zlen * _znum) != _zmax )
      throw larbys("Voxel3DMeta::Set (zmax - zmin) not divisible by znum!");

    _num_element = _xnum * _ynum * _znum;
    _valid = true;
  }

  VoxelID_t Voxel3DMeta::ID(double x, double y, double z) const
  {
    if(!_valid) throw larbys("Voxel3DMeta::ID cannot be called on invalid meta!");
    if(x > _xmax || x < _xmin) return kINVALID_VOXELID;
    if(y > _ymax || y < _ymin) return kINVALID_VOXELID;
    if(z > _zmax || z < _zmin) return kINVALID_VOXELID;

    VoxelID_t xindex = (x - _xmin) / _xlen;
    VoxelID_t yindex = (y - _ymin) / _ylen;
    VoxelID_t zindex = (z - _zmin) / _zlen;

    if(xindex == _xnum) xindex -= 1;
    if(yindex == _ynum) yindex -= 1;
    if(zindex == _znum) zindex -= 1;

    return (zindex * (_xnum * _ynum) + yindex * _xnum + xindex);
  }

  const std::array<double,3> Voxel3DMeta::Position(VoxelID_t id) const
  {
    if(!_valid) throw larbys("Voxel3DMeta::Position cannot be called on invalid meta!");
    if(id >= _num_element) throw larbys("Voxel3DMeta::Position invalid VoxelID_t!");
    
    VoxelID_t zid = id / (_xnum * _ynum);
    id -= zid * (_xnum * _ynum);
    VoxelID_t yid = id / _xnum;
    VoxelID_t xid = (id - yid * _xnum);

    std::array<double,3> pos;
    pos[0] = _xmin + ((double)xid + 0.5) * _xlen;
    pos[1] = _ymin + ((double)yid + 0.5) * _ylen;
    pos[2] = _zmin + ((double)zid + 0.5) * _zlen;
    return pos;
  }
  
  double Voxel3DMeta::X(VoxelID_t id) const
  {
    if(!_valid) throw larbys("Voxel3DMeta::X cannot be called on invalid meta!");
    if(id >= _num_element) throw larbys("Voxel3DMeta::X invalid VoxelID_t!");
    
    VoxelID_t zid = id / (_xnum * _ynum);
    id -= zid * (_xnum * _ynum);
    VoxelID_t yid = id / _xnum;
    VoxelID_t xid = (id - yid * _xnum);

    return _xmin + ((double)xid + 0.5) * _xlen;
  }
  
  double Voxel3DMeta::Y(VoxelID_t id) const
  {
    if(!_valid) throw larbys("Voxel3DMeta::Y cannot be called on invalid meta!");
    if(id >= _num_element) throw larbys("Voxel3DMeta::Y invalid VoxelID_t!");
    
    VoxelID_t zid = id / (_xnum * _ynum);
    id -= zid * (_xnum * _ynum);
    VoxelID_t yid = id / _xnum;
    return _ymin + ((double)yid + 0.5) * _ylen;
  }
  
  double Voxel3DMeta::Z(VoxelID_t id) const
  {
    if(!_valid) throw larbys("Voxel3DMeta::Z cannot be called on invalid meta!");
    if(id >= _num_element) throw larbys("Voxel3DMeta::Z invalid VoxelID_t!");
	
    VoxelID_t zid = id / (_xnum * _ynum);
    return _zmin + ((double)zid + 0.5) * _zlen;
  }

  std::string  Voxel3DMeta::Dump() const
  {
    std::stringstream ss;
    ss << "X range: " << _xmin << " => " << _xmax << " ... " << _xnum << " bins" << std::endl
       << "Y range: " << _ymin << " => " << _ymax << " ... " << _ynum << " bins" << std::endl
       << "Z range: " << _zmin << " => " << _zmax << " ... " << _znum << " bins" << std::endl;
    return std::string(ss.str());
  }

};

#endif
