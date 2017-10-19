/**
 * \file Range.h
 *
 * \ingroup Utils
 *
 * \brief Class def header for a class Range
 *
 * @author kazuhiro
 */

/** \addtogroup Utils
    @{*/
#ifndef __LARCVUTIL_RANGE_H__
#define __LARCVUTIL_RANGE_H__

#include <vector>
#include <iostream>
#include <stdexcept>

namespace larcv {

  template <class T>
  class UniqueRangeSet;

  /**
     \class Range
     @brief represents a "Range" w/ notion of ordering.
     A range is defined by a pair of "start" and "end" values. This is stored in std::pair    \n
     attribute larcv::Range::_window. This attribute is protected so that the start/end cannot \n
     be changed w/o a check that start is always less than end. Note the specialization       \n
     requires a template class T to have less operator implemented.   \n
  */
  template <class T>
  class Range {
    // Make it a friend so UniqueRangeSet can access protected guys
    friend class UniqueRangeSet<T>;

  public:
    /// Default ctor
    Range() : _valid(false)
    {}

    /// Enforced ctor. start must be less than end.
    Range(const T& start,
	  const T& end)
      : _window(start,end)
      , _valid(true)
    { if(start>end) throw std::runtime_error("Inserted invalid range: end before start."); }

    /// Default dtor
    ~Range(){}

    /// validity
    bool valid() const { return _valid; }
    /// "start" accessor
    const T& Start() const { return _window.first;  }
    /// "end" accessor
    const T& End()   const { return _window.second; }
    /// Setter
    void Set(const T& s, const T& e)
    { 
      if(s>e) throw std::runtime_error("Inserted invalid range: end before start."); 
      _window.first  = s;
      _window.second = e;
      _valid = true;
    }
    /// Checker (if value is inside range or not)
    inline bool Inside(const T& v) const
    { return _window.first <= v && v <= _window.second; }
    /// Checker (if value is outside range or not)
    inline bool Outside(const T& v) const
    { return !(this->Inside(v)); }

    //
    // Ordering w/ another Range
    //
    inline bool operator< (const Range& rhs) const
    {return ( _window.second < rhs.Start() ); }
    inline bool operator> (const Range& rhs) const
    {return ( _window.first > rhs.End() ); }
    inline bool operator==(const Range& rhs) const
    {return ( _window.first == rhs.Start() && _window.second == rhs.End() ); }
    inline bool operator!=(const Range& rhs) const
    {return !( (*this) == rhs ); }
    
    //
    // Ordering w/ T
    //
    inline bool operator< (const T& rhs) const
    {return (_window.second < rhs); }
    inline bool operator> (const T& rhs) const
    {return (_window.first > rhs); }

    /// Merge two larcv::Range into 1
    inline Range<T> operator+ (const Range<T>& rhs) const
    {
      Range<T> res(*this);
      res.Set(std::min( _window.first,  rhs.Start() ),
	      std::max( _window.second, rhs.End()   ) );
      return res;
    }
    inline Range<T>& operator+= (const Range<T>& rhs)
    {
      _window.first  = std::min( _window.first,  rhs.Start() );
      _window.second = std::max( _window.second, rhs.End()   );
      return (*this);
    }
    inline Range<T>& operator+= (const T& rhs)
    {
      _window.first  = std::min( _window.first,  rhs );
      _window.second = std::max( _window.second, rhs );
      return (*this);
    }

    /*
    void Merge(const Range& a) {
      _window.first  = std::min( _window.first,  a.Start() );
      _window.second = std::max( _window.second, a.End()   );
    }
      */
  protected:
    /// Protected to avoid user's illegal modification on first/second (sorry users!)
    std::pair<T,T> _window;
    /// For validity
    bool _valid;
  };
}

namespace std {
  // Implement pointer comparison in case it's useful
  template <class T>
  /**
     \class less
     Implementation of std::less for larcv::Range pointers
   */
  class less<larcv::Range<T>*>
  {
  public:
    bool operator()( const larcv::Range<T>* lhs, const larcv::Range<T>* rhs )
    { return (*lhs) < (*rhs); }
  };
}

#endif
/** @} */ // end of doxygen group
