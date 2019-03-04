#ifndef IMAGEMETAMAKER_CXX
#define IMAGEMETAMAKER_CXX

#include "ImageMetaMaker.h"
#include "ImageMetaMakerFactory.h"
namespace supera {

  ImageMetaMakerBase* ImageMetaMaker::_shared_meta_maker = nullptr;

  void ImageMetaMaker::configure(const supera::Config_t& cfg)
  {
    if(_meta_maker) delete _meta_maker;
    _meta_maker = supera::CreateImageMetaMaker(cfg);
  }
  
  const std::vector<larcv::ImageMeta>& ImageMetaMaker::Meta() const
  {
    if(_meta_maker) return _meta_maker->Meta();
    auto sptr = SharedMetaMaker();
    if(sptr) return sptr->Meta();
    
    std::cerr << "MetaMaker does not exist!" << std::endl;
    throw std::exception();
  }
  
  const std::vector<size_t>& ImageMetaMaker::RowCompressionFactor() const
  {
    if(_meta_maker) return _meta_maker->RowCompressionFactor();
    auto sptr = SharedMetaMaker();
    if(sptr) return sptr->RowCompressionFactor();
    
    std::cerr << "MetaMaker does not exist!" << std::endl;
    throw std::exception();
  }
  
  const std::vector<size_t>& ImageMetaMaker::ColCompressionFactor() const
  {
    if(_meta_maker) return _meta_maker->ColCompressionFactor();
    auto sptr = SharedMetaMaker();
    if(sptr) return sptr->ColCompressionFactor();
    
    std::cerr << "MetaMaker does not exist!" << std::endl;
    throw std::exception();
  }

}
#endif
