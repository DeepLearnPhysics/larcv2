/**
 * \file UniqueRangeSet.h
 *
 * \ingroup Utils
 *
 * \brief Class def header for a class larcv::UniqueRangeSet
 *
 * @author kazuhiro
 */

/** \addtogroup Utils
    @{*/

#ifndef __LARCVUTIL_UNIQUERANGESET_H__
#define __LARCVUTIL_UNIQUERANGESET_H__

#include <set>
#include <iostream>
#include <exception>
#include "Range.h"

namespace larcv {

  /**
     \class UniqueRangeSet
     @brief std::set of larcv::Range, which does not allow any overlap in contained element.
     std::set<Range> w/ modified insert/emplace function. Original std::set does not allow    \n
     modification of element. I assume what we're interested in is "find if the range already \n
     exists, and merge if it exists". The insert function does that by recursively looking up \n
     overlapping elements w.r.t. input argument of insert function.                           \n
     
     One important function worth noting is larcv::UniqueRangeSet::Exclusive which takes two   \n
     input arguments, "start" and "end", and returns larcv::UniqueRangeSet of all exclusive    \n
     regions between "start" and "end". By definition, merging this return with the original  \n
     instance will result in 1 huge larcv::Range.
  */
  template <class T>
  class UniqueRangeSet : public std::set<larcv::Range<T> > {
  public:
    /// default ctor
    UniqueRangeSet(){}
    /// default dtor
    ~UniqueRangeSet(){}

    /// Merge two UniqueRangeSet<T>
    inline UniqueRangeSet& operator+= (const UniqueRangeSet& rhs)
    {
      for(auto const& r : rhs) emplace(r._window.first,r._window.second);
      return (*this);
    }
    inline UniqueRangeSet operator+ (const UniqueRangeSet& rhs)
    {
      UniqueRangeSet res((*this));
      res += rhs;
      return res;
    }
    /*
    void Merge(const UniqueRangeSet<T>& in)
    { for(auto const& r : in) emplace(r._window.first,r._window.second); }
    */

    /// Very first "start" of all contained range
    const T& Start() const
    { 
      if(!(this->size())) throw std::runtime_error("Nothing in the set!");
      return (*(this->begin()))._window.first;
    }

    /// Very first "start" of all contained range
    const T& End() const
    { 
      if(!(this->size())) throw std::runtime_error("Nothing in the set!");
      return (*(this->rbegin()))._window.second;
    }

    /*
       It takes two input arguments, "start" and "end", and returns larcv::UniqueRangeSet \n
       of all exclusive regions between "start" and "end". By definition, merging this   \n
       return with the original instance will result in 1 huge larcv::Range.
    */
    UniqueRangeSet<T> Exclusive(const T start, const T end) const
    {
      UniqueRangeSet<T> res;

      auto start_iter = std::lower_bound(this->begin(),this->end(),start);
      auto end_iter   = std::lower_bound(this->begin(),this->end(),end);
      
      // Anything to add to the head?
      if(start < (*start_iter)._window.first) res.emplace(start,(*start_iter)._window.first);

      auto iter = start_iter;
      T  tmp_end=end;
      while(iter != this->end()) {
	if(iter != start_iter)
	  res.emplace(tmp_end,(*iter)._window.first);
	tmp_end   = (*iter)._window.second;
	if(iter == end_iter) break;
	++iter;
      }

      // Anything to add to the tail?
      if(tmp_end < end) 
	res.emplace(tmp_end,end);

      return res;
    }

    /// Modified emplace that merges overlapping range. Return = # merged range.
    size_t emplace(const T& start,const T& end) {

      auto res = std::set<larcv::Range<T> >::emplace(start,end);
      if(res.second) return 0;
      
      auto& iter = res.first;
      auto tmp_a = Range<T>(start,end);
      size_t ctr=0;
      while(iter != this->end()) {
	tmp_a += (*iter);
	this->erase(iter);
	iter = this->find(tmp_a);
	++ctr;
      }
      this->insert(tmp_a);
      return ctr;
    }

    /// Modified insert that merges overlapping range. Return = # merged range.
    size_t insert(const Range<T>& a)
    {return emplace(a._window.first,a._window.second);}
    
  };
}

#endif
/** @} */ // end of doxygen group
