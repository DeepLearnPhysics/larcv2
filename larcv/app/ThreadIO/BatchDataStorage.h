/**
 * \file BatchDataStorage.h
 *
 * \ingroup ThreadIO
 * 
 * \brief Class def header for a class BatchDataStorage
 *
 * @author kazuhiro
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef BATCHDATASTORAGE_H
#define BATCHDATASTORAGE_H

#include <iostream>
#include "BatchData.h"

namespace larcv {
  /**
     \class BatchDataStorage
     User defined class BatchDataStorage ... these comments are used to generate
     doxygen documentation!
  */
  template <class T>
  class BatchDataStorage {
    
  public:
    /// Default constructor
    BatchDataStorage(size_t num_batch=1);

    /// Default destructor
    ~BatchDataStorage(){}

    void reset(size_t num_batch);

    inline size_t num_batch() const { return _data_v.size(); }
    
    inline BatchDataState_t batch_state(size_t batch_id) const
    { return get_batch(batch_id).state(); }

    int  is_any_batch_ready () const;
    bool is_batch_ready (size_t batch_id) const;

    const BatchData<T>& get_any_batch  () const;
    const BatchData<T>& get_batch      (size_t batch_id) const;

    BatchData<T>& get_batch_writeable(size_t batch_id);

    void set_entry_data  (const std::vector<T>& source, size_t batch_id);

    void reset_batch(size_t batch_id, std::vector<int> dim);

    void reset_batch(size_t batch_id);

    void reset_batch();

  private:
    std::vector<larcv::BatchData<T> > _data_v;
  };
  
}

#endif
/** @} */ // end of doxygen group 

