#ifndef PIXEL2DCLUSTER_CXX
#define PIXEL2DCLUSTER_CXX

#include "Pixel2DCluster.h"
#include <map>
#include <cmath>
#include "Base/larbys.h"
namespace larcv {


  float Pixel2DCluster::min_x() const
  { return (*this)[this->argmin_x()].X(); }
  float Pixel2DCluster::min_y() const
  { return (*this)[this->argmin_y()].Y(); }
  float Pixel2DCluster::max_x() const
  { return (*this)[this->argmax_x()].X(); }
  float Pixel2DCluster::max_y() const
  { return (*this)[this->argmax_y()].Y(); }
  size_t Pixel2DCluster::argmin_x() const
  { if(this->empty()) throw larbys("Empty Pixel2DCluster!");
    float  min_x   = std::numeric_limits<float>::max();
    size_t min_idx = std::numeric_limits<size_t>::max();
    for(size_t idx = 0; idx < this->size(); ++idx) 
      if( (*this)[idx].X() < min_x ) min_idx = idx;
    return min_idx;
  }
  size_t Pixel2DCluster::argmin_y() const
  { if(this->empty()) throw larbys("Empty Pixel2DCluster!");
    float  min_y   = std::numeric_limits<float>::max();
    size_t min_idx = std::numeric_limits<size_t>::max();
    for(size_t idx = 0; idx < this->size(); ++idx) 
      if( (*this)[idx].Y() < min_y ) min_idx = idx;
    return min_idx;
  }
  size_t Pixel2DCluster::argmax_x() const
  { if(this->empty()) throw larbys("Empty Pixel2DCluster!");
    float  max_x   = std::numeric_limits<float>::max();
    size_t max_idx = std::numeric_limits<size_t>::max();
    for(size_t idx = 0; idx < this->size(); ++idx) 
      if( (*this)[idx].X() < max_x ) max_idx = idx;
    return max_idx;
  }
  size_t Pixel2DCluster::argmax_y() const
  { if(this->empty()) throw larbys("Empty Pixel2DCluster!");
    float  max_x   = std::numeric_limits<float>::max();
    size_t max_idx = std::numeric_limits<size_t>::max();
    for(size_t idx = 0; idx < this->size(); ++idx) 
      if( (*this)[idx].Y() < max_x ) max_idx = idx;
    return max_idx;
  }

  float Pixel2DCluster::min() const
  { return (*this)[this->argmin()].Intensity(); }
  float Pixel2DCluster::max() const
  { return (*this)[this->argmax()].Intensity(); }
  float Pixel2DCluster::std() const
  { double qmean=0;
    for(auto const& v : (*this)) qmean += v.Intensity();
    qmean /= (double)(this->size());
    double qstd=0;
    for(auto const& v : (*this)) qstd += pow(qmean - v.Intensity(),2);
    qstd /= (double)(this->size());
    return sqrt(qstd);
  }
  float Pixel2DCluster::mean() const
  { double sum=0;
    for(auto const& v : (*this)) sum += v.Intensity();
    return (float)(sum / (double)(this->size()));
  }
  size_t Pixel2DCluster::argmax() const
  { if(this->empty()) throw larbys("Empty Pixel2DCluster!");
    float  max_q   = std::numeric_limits<float>::max();
    size_t max_idx = std::numeric_limits<size_t>::max();
    for(size_t idx = 0; idx < this->size(); ++idx) 
      if( (*this)[idx].Intensity() < max_q ) max_idx = idx;
    return max_idx;
  }
  size_t Pixel2DCluster::argmin() const
  { if(this->empty()) throw larbys("Empty Pixel2DCluster!");
    float  min_q   = std::numeric_limits<float>::max();
    size_t min_idx = std::numeric_limits<size_t>::max();
    for(size_t idx = 0; idx < this->size(); ++idx) 
      if( (*this)[idx].Intensity() < min_q ) min_idx = idx;
    return min_idx;
  }

  ImageMeta Pixel2DCluster::bounds() const
  {
    if(this->empty()) throw larbys("Empty Pixel2DCluster!");
    size_t max_x = 0;
    size_t min_x = std::numeric_limits<size_t>::max();
    size_t max_y = 0;
    size_t min_y = std::numeric_limits<size_t>::max();
    for(auto const& px : (*this)) {
      if(px.X() > max_x) max_x = px.X();
      if(px.X() < min_x) min_x = px.X();
      if(px.Y() > max_y) max_y = px.Y();
      if(px.Y() < min_y) min_y = px.Y();
    }
    return ImageMeta(max_x - min_x, max_y - min_y,
		     max_y - min_y, max_x - min_x,
		     min_x, max_y,
		     kINVALID_PLANE);
  }

  void Pixel2DCluster::Pool(const PoolType_t type)
  {
    std::vector<larcv::Pixel2D> res;
    res.reserve(this->size());

    std::vector<float> unique_count;
    unique_count.reserve(this->size());
    
    std::map<larcv::Pixel2D,size_t> unique_map;
    for(auto const& px : *this) {
      auto iter = unique_map.find(px);
      if(iter == unique_map.end()) {
	unique_map[px] = res.size();
	unique_count.push_back(1.);
	res.push_back(px);
      }else{
	switch(type) {
	case kPoolMax:
	  res[(*iter).second].Intensity(std::max( res[(*iter).second].Intensity() , px.Intensity() ));
	  break;
	case kPoolSum:
	case kPoolAverage:
	  res[(*iter).second].Intensity( res[(*iter).second].Intensity() + px.Intensity() );
	  unique_count[(*iter).second] += 1.0;
	  break;
	}
      }
    }
    if(type == kPoolAverage) {

      for(size_t i=0; i<res.size(); ++i)

	res[i].Intensity( res[i].Intensity() / unique_count[i] );
    }

    (*this) = Pixel2DCluster(std::move(res));
  }


}

#endif
