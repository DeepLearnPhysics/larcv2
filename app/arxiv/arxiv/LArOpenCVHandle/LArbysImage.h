#ifndef __LARBYSIMAGE_H__
#define __LARBYSIMAGE_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
#include "LArOpenCV/Core/ImageManager.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterManager.h"
#include "DataFormat/Image2D.h"
#include "PreProcessor.h"
#include "LArbysImageMaker.h"
#include "ImageMod/ImageModUtils.h"
#include "DataFormat/EventROI.h"


namespace larcv {

  class LArbysImage : public ProcessBase {

  public:
    
    /// Default constructor
    LArbysImage(const std::string name="LArbysImage");
    
    /// Default destructor
    virtual ~LArbysImage(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    const larocv::ImageClusterManager& Manager() const { return _alg_mgr; }

    const PreProcessor&     PProcessor()     const { return _PreProcessor; }
    const LArbysImageMaker& LArbysImgMaker() const { return _LArbysImageMaker; }
    
  protected:

    const std::vector<larcv::Image2D>& get_image2d(IOManager& mgr, std::string producer);

    void get_rsee(IOManager& mgr, std::string producer,
		  uint& run, uint& subrun, uint& event, uint& entry);
            
    
    bool Reconstruct(const std::vector<larcv::Image2D>& adc_image_v,
		     const std::vector<larcv::Image2D>& track_image_v,
		     const std::vector<larcv::Image2D>& shower_image_v,
		     const std::vector<larcv::Image2D>& thrumu_image_v,
		     const std::vector<larcv::Image2D>& stopmu_image_v,
		     const std::vector<larcv::Image2D>& chstat_image_v);

    virtual void Process();
    
    bool StoreParticles(IOManager& iom,
			const std::vector<larcv::Image2D>& adcimg_v,
			size_t& pidx);

    larocv::ImageClusterManager _alg_mgr;
    larocv::ImageManager _adc_img_mgr;
    larocv::ImageManager _track_img_mgr;
    larocv::ImageManager _shower_img_mgr;
    larocv::ImageManager _thrumu_img_mgr;
    larocv::ImageManager _stopmu_img_mgr;
    larocv::ImageManager _chstat_img_mgr;

    ::fcllite::PSet _image_cluster_cfg;

    bool _debug;
    bool _preprocess;
    bool _write_reco;

    bool _mask_thrumu_pixels;
    bool _mask_stopmu_pixels;
    
    std::string _output_module_name;
    size_t _output_module_offset;
    
    std::string _rse_producer;
    std::string _adc_producer;
    std::string _roi_producer;
    std::string _track_producer;
    std::string _shower_producer;
    ProductType_t _tags_datatype;
    std::string _thrumu_producer;
    std::string _stopmu_producer;
    std::string _channel_producer;
    std::string _output_producer;

    std::string _vertex_algo_name;
    std::string _par_algo_name;
    std::string _3D_algo_name;
    
    larocv::AlgorithmID_t _vertex_algo_id;
    larocv::AlgorithmID_t _par_algo_id;
    larocv::AlgorithmID_t _3D_algo_id;
    
    size_t _vertex_algo_vertex_offset;
    size_t _par_algo_par_offset;
    
    double _process_count;
    double _process_time_image_extraction;
    double _process_time_analyze;
    double _process_time_cluster_storage;

    bool _union_roi;
    
    void Report() const;
    
    PreProcessor     _PreProcessor;
    LArbysImageMaker _LArbysImageMaker;
    
    std::vector<larcv::Image2D> _empty_image_v;
    std::vector<larcv::Image2D> _thrumu_image_v;
    std::vector<larcv::Image2D> _stopmu_image_v;
    ROI _current_roi;

  };

  /**
     \class larcv::LArbysImageFactory
     \brief A concrete factory class for larcv::LArbysImage
  */
  class LArbysImageProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    LArbysImageProcessFactory() { ProcessFactory::get().add_factory("LArbysImage",this); }
    /// dtor
    ~LArbysImageProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new LArbysImage(instance_name); }
  };

}

#endif

