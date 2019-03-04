// /**
//  * \file BatchFillerVOC.h
//  *
//  * \ingroup ThreadIO
//  * 
//  * \brief Class def header for a class BatchFillerVOC
//  *
//  * @author kazuhiro
//  */

// /** \addtogroup ThreadIO

//     @{*/
// #ifndef __BATCHFILLERVOC_H__
// #define __BATCHFILLERVOC_H__

// #include "larcv/core/Processor/ProcessFactory.h"
// #include "BatchFillerTemplate.h"
// #include "larcv/core/DataFormat/EventImage2D.h"

// namespace larcv {

//   /**
//      \class ProcessBase
//      User defined class BatchFillerVOC ... these comments are used to generate
//      doxygen documentation!
//   */
//   class BatchFillerVOC : public BatchFillerTemplate<float> {

//   public:
    
//     /// Default constructor
//     BatchFillerVOC(const std::string name="BatchFillerVOC");
    
//     /// Default destructor
//     ~BatchFillerVOC(){}

//     void configure(const PSet&);

//     void initialize();

//     bool process(IOManager& mgr);

//     void finalize();

    

//   protected:

//     void _batch_begin_();
//     void _batch_end_();

//   private:

//     size_t set_image_size(const EventImage2D* image_data);
//     void assert_dimension(const EventImage2D* image_data) const;
    
//     std::string _image_producer;
//     size_t _rows;
//     size_t _cols;
//     size_t _num_channels;
//     std::vector<float>  _entry_data;


//   };

//   /**
//      \class larcv::BatchFillerImage2DFactory
//      \brief A concrete factory class for larcv::BatchFillerVOC
//   */
//   class BatchFillerVOCProcessFactory : public ProcessFactoryBase {
//   public:
//     /// ctor
//     BatchFillerVOCProcessFactory() { ProcessFactory::get().add_factory("BatchFillerVOC",this); }
//     /// dtor
//     ~BatchFillerVOCProcessFactory() {}
//     /// creation method
//     ProcessBase* create(const std::string instance_name) {
//       return new BatchFillerVOC(instance_name);
//     }
//   };

// }

// #endif
// /** @} */ // end of doxygen group 

