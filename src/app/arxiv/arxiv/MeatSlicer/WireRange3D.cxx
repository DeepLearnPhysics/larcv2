#ifndef WIRERANGE3D_CXX
#define WIRERANGE3D_CXX

#define WIRERANGE3D_CRITICAL() if(_verbosity<6) std::cerr << "  \033[5;1;33;41m[CRITICAL]\033[94m<<PulledPork3DSlicer::" << __FUNCTION__ << "::L" << __LINE__ << ">>\033[00m "
#define WIRERANGE3D_ERROR()    if(_verbosity<5) std::cerr << "     \033[91m[ERROR]\033[94m <<PulledPork3DSlicer::" << __FUNCTION__ << "::L" << __LINE__ << ">>\033[00m "
#define WIRERANGE3D_WARNING()  if(_verbosity<4) std::cout << "   \033[93m[WARNING]\033[94m <<PulledPork3DSlicer::" << __FUNCTION__ << ">>\033[00m "
#define WIRERANGE3D_NORMAL()   if(_verbosity<3) std::cout << "    \033[95m[NORMAL]\033[94m <<PulledPork3DSlicer>>\033[00m "
#define WIRERANGE3D_INFO()     if(_verbosity<2) std::cout << "      \033[92m[INFO]\033[94m <<PulledPork3DSlicer::" << __FUNCTION__ << ">>\033[00m "
#define WIRERANGE3D_DEBUG()    if(_verbosity<1) std::cout << "     \033[94m[DEBUG]\033[94m <<PulledPork3DSlicer::" << __FUNCTION__ << "::L" << __LINE__ << ">>\033[00m "

#include "WireRange3D.h"
#include <random>
#include <sstream>
namespace supera {

  std::string WireRange3D::PrintConfig() const
  {
    std::stringstream ss;
    ss << "WireRange3D configuration..." << std::endl
       << "    X grid size: " << _grid_size_x << " ... bounds: " << _min_fiducial.x << " => " << _max_fiducial.x
       << " ... width = " << _width.x << ", padding = " << _padding.x << std::endl
       << "    Y grid size: " << _grid_size_y << " ... bounds: " << _min_fiducial.y << " => " << _max_fiducial.y
       << " ... width = " << _width.y << ", padding = " << _padding.y << std::endl
       << "    Z grid size: " << _grid_size_z << " ... bounds: " << _min_fiducial.z << " => " << _max_fiducial.z
       << " ... width = " << _width.z << ", padding = " << _padding.z << std::endl;
    if(_constraint_v.empty())
      ss << "    No constraint point registered..." << std::endl << std::endl;
    else {
      ss << "    List of constraints:" << std::endl;
      for(auto const& pt : _constraint_v)
	ss << "        Grid: (" << pt.x << "," << pt.y << "," << pt.z << ")"
	   << "... Real: (" << pt.x * GridSizeX() << "," << pt.y * GridSizeY() << "," << pt.z * GridSizeZ() << ")" << std::endl;
      ss<< std::endl;
    }
    return std::string(ss.str());
  }

  supera::GridPoint3D WireRange3D::GridPoint3D(double x, double y, double z) const
  {
    if(_grid_size_x <=0. ||
       _grid_size_y <=0. ||
       _grid_size_z <=0. ) {
      WIRERANGE3D_CRITICAL() << "Grid size is not set yet!" << std::endl;
      throw std::exception();
    }
    supera::GridPoint3D pt;
    pt.x = (int)(x / _grid_size_x + 0.5);
    pt.y = (int)(y / _grid_size_y + 0.5);
    pt.z = (int)(z / _grid_size_z + 0.5);
    return pt;
  }
  
  bool WireRange3D::Valid() const
  {
    if(!_width.Valid()) return false;
    if(!_min_fiducial.Valid()) return false;
    if(!_max_fiducial.Valid()) return false;
    if(_grid_size_x<0. || _grid_size_y<0. || _grid_size_z<0.) return false;
    return true;
  }

  void WireRange3D::RederiveBounds() {
    _min_bound = _min_fiducial;
    _max_bound = _max_fiducial;
    for(auto const& constraint_pt : _constraint_v) {
      if( (_min_bound.x + _width.x - _padding.x) <= constraint_pt.x ) _min_bound.x = (constraint_pt.x - _width.x + _padding.x + 1);
      if( (_min_bound.y + _width.y - _padding.y) <= constraint_pt.y ) _min_bound.y = (constraint_pt.y - _width.y + _padding.y + 1);
      if( (_min_bound.z + _width.z - _padding.z) <= constraint_pt.z ) _min_bound.z = (constraint_pt.z - _width.z + _padding.z + 1);
      if( (_max_bound.x - _width.x + _padding.x) >= constraint_pt.x ) _max_bound.x = (constraint_pt.x + _width.x - _padding.x - 1);
      if( (_max_bound.y - _width.y + _padding.y) >= constraint_pt.y ) _max_bound.y = (constraint_pt.y + _width.y - _padding.y - 1);
      if( (_max_bound.z - _width.z + _padding.z) >= constraint_pt.z ) _max_bound.z = (constraint_pt.z + _width.z - _padding.z - 1);    }
  }

  void WireRange3D::SetPadding(double x, double y, double z) {
    auto padding = this->GridPoint3D(x,y,z);
    if(_constraint_v.empty()) {
      _padding = padding;
      return;
    }
    // Check if this width is compatible with registered constraints
    auto min_bound = GridPoint3D(std::numeric_limits<int>::max(),std::numeric_limits<int>::max(),std::numeric_limits<int>::max());
    auto max_bound = GridPoint3D(std::numeric_limits<int>::min(),std::numeric_limits<int>::min(),std::numeric_limits<int>::min());
    for(auto const& constraint_pt : _constraint_v) {
      if(min_bound.x > constraint_pt.x) min_bound.x = constraint_pt.x;
      if(min_bound.y > constraint_pt.y) min_bound.y = constraint_pt.y;
      if(min_bound.z > constraint_pt.z) min_bound.z = constraint_pt.z;
      if(max_bound.x < constraint_pt.x) max_bound.x = constraint_pt.x;
      if(max_bound.y < constraint_pt.y) max_bound.y = constraint_pt.y;
      if(max_bound.z < constraint_pt.z) max_bound.z = constraint_pt.z;
    }
    if( (max_bound.x - min_bound.x) >= (_width.x + padding.x) ||
	(max_bound.y - min_bound.y) >= (_width.y + padding.y) ||
	(max_bound.z - min_bound.z) >= (_width.z + padding.z) ) {
      WIRERANGE3D_CRITICAL() << "Padding (" << x << "," << y << "," << z << ") cannot hold all constraints!" << std::endl;
      throw std::exception();
    }
    _padding = padding;
    this->RederiveBounds();
  }
  
  void WireRange3D::SetMin(double x, double y, double z) {
    auto pt = this->GridPoint3D(x,y,z);
    for(auto const& constraint_pt : _constraint_v) {
      if(constraint_pt.x < pt.x ||
	 constraint_pt.y < pt.y ||
	 constraint_pt.z < pt.z ) {
	WIRERANGE3D_CRITICAL() << "Minimum bound @ (" << x << "," << y << "," << z << ") cannot hold registered constraint!" << std::endl;
	throw std::exception();
      }
    }
    _min_fiducial = pt;
  }

  void WireRange3D::SetMax(double x, double y, double z) {
    auto pt = this->GridPoint3D(x,y,z);
    for(auto const& constraint_pt : _constraint_v) {
      if(constraint_pt.x > pt.x ||
	 constraint_pt.y > pt.y ||
	 constraint_pt.z > pt.z ) {
	WIRERANGE3D_CRITICAL() << "Maximum bound @ (" << x << "," << y << "," << z << ") cannot hold registered constraint!" << std::endl;
	throw std::exception();
      }
    }
    _max_fiducial = pt;
    this->RederiveBounds();
  }

  void WireRange3D::AddConstraint(double x, double y, double z) {
    if(!Valid()) {
      WIRERANGE3D_CRITICAL() << "Cannot add constraint before configuration..." << std::endl;
      throw std::exception();
    }
    auto pt = this->GridPoint3D(x,y,z);
    if(pt.x < _min_fiducial.x || pt.x > _max_fiducial.x ||
       pt.y < _min_fiducial.y || pt.y > _max_fiducial.y ||
       pt.z < _min_fiducial.z || pt.z > _max_fiducial.z) {
      WIRERANGE3D_CRITICAL() << "Constraint @ (" << x << "," << y << "," << z << ") violates the bound!" << std::endl;
      throw std::exception();
    }
    auto min_bound = pt;
    auto max_bound = pt;
    for(auto const& constraint_pt : _constraint_v) {
      if(min_bound.x > constraint_pt.x) min_bound.x = constraint_pt.x;
      if(min_bound.y > constraint_pt.y) min_bound.y = constraint_pt.y;
      if(min_bound.z > constraint_pt.z) min_bound.z = constraint_pt.z;
      if(max_bound.x < constraint_pt.x) max_bound.x = constraint_pt.x;
      if(max_bound.y < constraint_pt.y) max_bound.y = constraint_pt.y;
      if(max_bound.z < constraint_pt.z) max_bound.z = constraint_pt.z;
    }
    if( (max_bound.x - min_bound.x) >= _width.x ||
	(max_bound.y - min_bound.y) >= _width.y ||
	(max_bound.z - min_bound.z) >= _width.z ) {
      WIRERANGE3D_ERROR() << "Constraint @ (" << x << "," << y << "," << z << ")"
			  << " violates the width (" << _width.x << "," << _width.y << "," << _width.z << ")" << std::endl;
      return;
    }
    _constraint_v.emplace_back(pt);
    this->RederiveBounds();
  }

  void WireRange3D::SetWidth(double x, double y, double z) {
    auto width = this->GridPoint3D(x,y,z);
    if(_constraint_v.empty()) {
      _width = width;
      return;
    }
    // Check if this width is compatible with registered constraints
    auto min_bound = GridPoint3D(std::numeric_limits<int>::max(),std::numeric_limits<int>::max(),std::numeric_limits<int>::max());
    auto max_bound = GridPoint3D(std::numeric_limits<int>::min(),std::numeric_limits<int>::min(),std::numeric_limits<int>::min());
    for(auto const& constraint_pt : _constraint_v) {
      if(min_bound.x > constraint_pt.x) min_bound.x = constraint_pt.x;
      if(min_bound.y > constraint_pt.y) min_bound.y = constraint_pt.y;
      if(min_bound.z > constraint_pt.z) min_bound.z = constraint_pt.z;
      if(max_bound.x < constraint_pt.x) max_bound.x = constraint_pt.x;
      if(max_bound.y < constraint_pt.y) max_bound.y = constraint_pt.y;
      if(max_bound.z < constraint_pt.z) max_bound.z = constraint_pt.z;
    }
    if( (max_bound.x - min_bound.x) >= (width.x + _padding.x) ||
	(max_bound.y - min_bound.y) >= (width.y + _padding.y) ||
	(max_bound.z - min_bound.z) >= (width.z + _padding.z) ) {
      WIRERANGE3D_CRITICAL() << "Width (" << x << "," << y << "," << z << ") cannot hold all constraints!" << std::endl;
      throw std::exception();
    }
    _width = width;
    this->RederiveBounds();
  }

  void WireRange3D::DeriveRange(const std::vector<supera::GridPoint3D>& points,
				supera::GridPoint3D& min_pt,
				supera::GridPoint3D& max_pt) const {

    if(!this->Valid()) {
      WIRERANGE3D_CRITICAL() << "Configuration incomplete!" << std::endl;
      throw std::exception();
    }

    static std::random_device rd;
    static std::mt19937 mt(rd());

    auto const min_bound = _min_bound;
    auto const max_bound = _max_bound;
    
    //
    // Local algorithm parameters
    // (note min_pt, max_pt are merely a container, not diagonal edge of containing cube!)
    //
    static const int MIN_DEFAULT = std::numeric_limits<int>::max();
    static const int MAX_DEFAULT = std::numeric_limits<int>::min();
    min_pt.x = min_pt.y = min_pt.z = MIN_DEFAULT;
    max_pt.x = max_pt.y = max_pt.z = MAX_DEFAULT;

    //
    // Determine x boundary
    //
    for(auto const& pt : points) {
      
      if( pt.x < min_bound.x || pt.x > max_bound.x ) {
	WIRERANGE3D_DEBUG() << "Ignoring point Grid X = " << pt.x
			    << " as it is outside the bound: " << min_bound.x
			    << " => " << max_bound.x << std::endl;
	continue;
      }
      WIRERANGE3D_DEBUG() << "Accepting point Grid X = " << pt.x << std::endl;

      if(pt.x < min_pt.x) min_pt.x = pt.x;
      if(pt.x > max_pt.x) max_pt.x = pt.x;
    }
    if(min_pt.x == MIN_DEFAULT || max_pt.x == MAX_DEFAULT) {
      WIRERANGE3D_INFO() << "No point found to fit within X constraint points! Generating random boundary within constraint..." << std::endl;
      std::uniform_int_distribution<int> min_gen(min_bound.x,max_bound.x - _width.x + 1);
      min_pt.x = min_gen(mt);
      max_pt.x = min_pt.x + _width.x - 1;
      WIRERANGE3D_INFO() << "New result: " << min_pt.x << " => " << max_pt.x << std::endl;
    }else{
      WIRERANGE3D_INFO() << "Generating X range..." << std::endl;
      this->Randomize(min_pt.x, max_pt.x, min_bound.x, max_bound.x, _width.x, _padding.x);
    }
    WIRERANGE3D_INFO() << std::endl;
    
    //
    // Determine z boundary
    //
    for(auto const& pt : points) {

      if( pt.x < min_pt.x || pt.x > max_pt.x)
	continue;
      if( pt.z < min_bound.z || pt.z > max_bound.z ) {
	WIRERANGE3D_DEBUG() << "Ignoring point Grid Z = " << pt.z
			    << " as it is outside the bound: " << min_bound.z << " => " << max_bound.z << std::endl;
	continue;
      }
      WIRERANGE3D_DEBUG() << "Accepting point Grid Z = " << pt.z << std::endl;
      
      if(pt.z < min_pt.z) min_pt.z = pt.z;
      if(pt.z > max_pt.z) max_pt.z = pt.z;
    }
    if(min_pt.z == MIN_DEFAULT || max_pt.z == MAX_DEFAULT) {
      WIRERANGE3D_INFO() << "No point found to fit within Z constraint points! Generating random boundary within constraint..." << std::endl;
      std::uniform_int_distribution<int> min_gen(min_bound.z,max_bound.z - _width.z + 1);
      min_pt.z = min_gen(mt);
      max_pt.z = min_pt.z + _width.z - 1;
      WIRERANGE3D_INFO() << "New result: " << min_pt.z << " => " << max_pt.z << std::endl;
    }else{
      WIRERANGE3D_INFO() << "Generating Z range..." << std::endl;
      this->Randomize(min_pt.z, max_pt.z, min_bound.z, max_bound.z, _width.z, _padding.z);
    }
    WIRERANGE3D_INFO() << std::endl;
	
    //
    // Determine y boundary
    //
    for(auto const& pt : points) {

      if( pt.x < min_pt.x || pt.x > max_pt.x)
	continue;
      if( pt.z < min_pt.z || pt.z > max_pt.z)
	continue;
      if( pt.y < min_bound.y || pt.y > max_bound.y ) {
	WIRERANGE3D_DEBUG() << "Ignoring point Grid Y = " << pt.y
			    << " as it is outside the bound: " << min_bound.y << " => " << max_bound.y << std::endl;
	continue;
      }
      WIRERANGE3D_DEBUG() << "Accepting point Grid Y = " << pt.y << std::endl;
      
      if(pt.y < min_pt.y) min_pt.y = pt.y;
      if(pt.y > max_pt.y) max_pt.y = pt.y;
    }
    if(min_pt.y == MIN_DEFAULT || max_pt.y == MAX_DEFAULT) {
      WIRERANGE3D_INFO() << "No point found to fit within Y constraint points! Generating random boundary within constraint..." << std::endl;
      std::uniform_int_distribution<int> min_gen(min_bound.y,max_bound.y - _width.y + 1);
      min_pt.y = min_gen(mt);
      max_pt.y = min_pt.y + _width.y - 1;
      WIRERANGE3D_INFO() << "New result: " << min_pt.y << " => " << max_pt.y << std::endl;
    }else{
      WIRERANGE3D_INFO() << "Generating Y range..." << std::endl;
      this->Randomize(min_pt.y, max_pt.y, min_bound.y, max_bound.y, _width.y, _padding.z);
    }
    WIRERANGE3D_INFO() << std::endl;
  }

  void WireRange3D::Randomize(int& min_val, int& max_val,
			      const int min_bound, const int max_bound,
			      const int width, const int padding) const
  {
    WIRERANGE3D_INFO() << "Value range: " << min_val << " => " << max_val
		       << " ... absolute bound: " << min_bound << " => " << max_bound
		       << " ... target width: " << width << std::endl;
    static std::random_device rd;
    static std::mt19937 mt(rd());
    int space = width - (max_val - min_val + 1);
    int rand_bound_min = 0;
    int rand_bound_max = 0;
    if( space == 0 ) {
      WIRERANGE3D_INFO() << "Range exactly same as the target width. Nothing to do..." << std::endl;
      return;
    }
    
    else if( space > 0 ) {
      WIRERANGE3D_INFO() << "Data point range = " << (max_val - min_val + 1)
			 << " is smaller than target width = " << width << std::endl;
      // Width found is smaller than limit. 
      // Randomize x-edge within the available space.
      if( (min_val - min_bound) < (max_bound - max_val) ) {
	rand_bound_min = std::max(-space, min_bound - min_val);
	rand_bound_max = 0;
	WIRERANGE3D_INFO() << "Closer to min bound. Generating uniform random: "
			   << rand_bound_min << " => " << rand_bound_max << std::endl;
	std::uniform_int_distribution<int> shift_gen( rand_bound_min, rand_bound_max );
	int shift = shift_gen(mt);
	max_val = min_val + shift + width - 1;
	min_val = min_val + shift;
	WIRERANGE3D_INFO() << "Applying shift: " << shift << " ... result: " << min_val << " => " << max_val << std::endl;
      }else{
	rand_bound_min = 0;
	rand_bound_max = std::min( space, max_bound - max_val);
	WIRERANGE3D_INFO() << "Closer to max bound. Generating uniform random: "
			   << rand_bound_min << " => " << rand_bound_max << std::endl;
	std::uniform_int_distribution<int> shift_gen( rand_bound_min, rand_bound_max );
	int shift = shift_gen(mt);
	min_val = max_val + shift - width + 1;
	max_val = max_val + shift;
	WIRERANGE3D_INFO() << "Applying shift: " << shift << " ... result: " << min_val << " => " << max_val << std::endl;
      }
      
    }else {
      WIRERANGE3D_INFO() << "Data point range = " << (max_val - min_val + 1)
			 << " is larger than target width = " << width << std::endl;
      // Width found is larger than limit.
      // Randomize x-edge within the available space.
      rand_bound_min = min_val;
      rand_bound_max = max_val - width + 1;
      WIRERANGE3D_INFO() << "Generating uniform random: " << rand_bound_min << " => " << rand_bound_max << std::endl;
      std::uniform_int_distribution<int> min_gen( rand_bound_min, rand_bound_max );
      min_val = min_gen(mt);
      max_val = min_val + width - 1;
      WIRERANGE3D_INFO() << "New result: " << min_val << " => " << max_val << std::endl;
    }
  }

}

#endif
