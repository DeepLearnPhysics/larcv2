/**
 * \file BatchData.h
 *
 * \ingroup ThreadIO
 *
 * \brief Class def header for a class BatchData
 *
 * @author kazuhiro
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef BATCHDATA_H
#define BATCHDATA_H

#include <iostream>
#include <vector>
#include "ThreadIOTypes.h"

namespace larcv {
  /**
     \class BatchData
     User defined class BatchData ... these comments are used to generate
     doxygen documentation!
  */
  template <class T>
  class BatchData {

  public:

    /// Default constructor
    BatchData()
      : _current_size(0)
      , _state(BatchDataState_t::kBatchStateUnknown)
    {}

    /// Default destructor
    ~BatchData() {}

    const std::vector<T>& data() const;

    inline const std::vector<int>& dim() const { return _dim; }

    size_t data_size(bool calculate=false) const;

    inline size_t current_data_size() const { return _current_size; }

    size_t entry_data_size() const;

    void set_dim(const std::vector<int>& dim);
    void set_entry_data(const std::vector<T>& entry_data);

    void reset();
    void reset_data();

    inline bool is_filled() const
    { return ( _state == BatchDataState_t::kBatchStateFilled ); }

    inline BatchDataState_t state() const
    { return _state; }

  private:
    std::vector<T>   _data;
    std::vector<int> _dim;
    size_t _current_size;
    BatchDataState_t _state;
  };
}
#endif
/** @} */ // end of doxygen group

