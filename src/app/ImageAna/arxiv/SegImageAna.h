/**
 * \file SegImageAna.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class SegImageAna
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SEGIMAGEANA_H__
#define __SEGIMAGEANA_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class SegImageAna ... these comments are used to generate
     doxygen documentation!
  */
  class SegImageAna : public ProcessBase {

  public:

    /// Default constructor
    SegImageAna(const std::string name = "SegImageAna");

    /// Default destructor
    ~SegImageAna() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _image_producer;   ///< Input Image2D producer name
    std::string _label_producer;   ///< Segmentation ground-truth (label) Image2D producer name
    std::string _segment_producer; ///< Segmentation prediction (by CNN) Image2D producer name
    std::string _roi_producer;     ///< Some MCTruth info might be interesting

    size_t _image_channel;         ///< Which channel to use from input Image2D (assuming 1 channel)
    size_t _label_channel;         ///< Which channel to use from input label Image2D (assuming 1 channel)
    std::vector<size_t> _roitype_to_class; ///< Attribute to convert ROIType_t to class type defined by caffe
    size_t _ntypes;                ///< Number of unique classes used by caffe

    TTree* _tree;        ///< Output analysis tree
    double _pi_thresh;   ///< Pixel Intensity (PI) threshold value to filter out pixels we do not care
    int _npx_thresh;     ///< # pixels above PI threshold
    int _npx_total;      ///< # pixels total
    int _npx_correct;    ///< # pixels where prediction got it right
    int _roi_type;       ///< # enum for ROIType

    double _prob_correct; ///< Probability of getting the right pixel value in the whole image
    std::vector<int> _npx_total_v;       ///< # of total pixel for a specific type (based on ground truth)
    std::vector<int> _npx_predicted_v;   ///< # of predicted pixel for a specific type
    std::vector<int> _npx_correct_v;     ///< # of correctly labeled pixel for a specific type 
    std::vector<double> _prob_correct_v; ///< Probability of getting the right pixel value in the whole image for a specific type

  };

  /**
     \class larcv::SegImageAnaFactory
     \brief A concrete factory class for larcv::SegImageAna
  */
  class SegImageAnaProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SegImageAnaProcessFactory() { ProcessFactory::get().add_factory("SegImageAna", this); }
    /// dtor
    ~SegImageAnaProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SegImageAna(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

