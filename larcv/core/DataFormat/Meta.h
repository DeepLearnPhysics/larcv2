/**
 * \file Meta.h
 *
 * \ingroup DataFormat
 * 
 * \brief Class def header for Meta
 *
 * @author Kazu - Nevis 2013
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef LARCV_META_H
#define LARCV_META_H

#include <map>
#include <vector>
#include <string>

namespace larcv {
  /**
     \class Meta
     A class to hold meta data that can be generated on the fly.
     What we mean by "on the fly"? It means you can define variable name
     and store it in the data w/o defining a dedicated data container class.
     
     The supported variable type for storage includes double, int, std::string, bool, and vector of those types.
     The variable is stored as a pair of a specific std::string key and value, both provided by a user.
     The string is then used to retrieve a variable later.
     Note that each retrieval requires std::string comparison, hence is somewhat expensive (i.e. takes time).
     But there are use-cases where speed may not be too important.
     
  */
  class Meta {
    
  public:
    
    /** Method to dump all key & value contents in the storage map on the screen
	This is useful to check what's in the data interactively.
    */
    void dump() const;
    
    /// Default constructor
    Meta() {clear_data();};
    /// Default destructor
    virtual ~Meta(){};
    
    //
    // Read-access
    //
    /// checker for an existence of a double variable key
    inline bool exist_double (const std::string& key) const { return _d_map.find(key)!=_d_map.end(); };
    /// checker for an existence of a string variable key
    inline bool exist_string (const std::string& key) const { return _s_map.find(key)!=_s_map.end(); };
    /// checker for an existence of a int variable key
    inline bool exist_int    (const std::string& key) const { return _i_map.find(key)!=_i_map.end(); };
    /// checker for an existence of a double array
    inline bool exist_darray (const std::string& key) const { return _darray_map.find(key)!=_darray_map.end(); };
    /// checker for an existence of a string array
    inline bool exist_sarray (const std::string& key) const { return _sarray_map.find(key)!=_sarray_map.end(); };
    /// checker for an existence of a int array
    inline bool exist_iarray (const std::string& key) const { return _iarray_map.find(key)!=_iarray_map.end(); };
    
    /// getter for a single double variable
    double      get_double (const std::string& key) const;  
    /// getter for a single int variable
    int         get_int    (const std::string& key) const;  
    /// getter for a single std::string variable
    const std::string& get_string (const std::string& key) const;  
    
    /// getter for a double array
    const std::vector<double>&      get_darray (const std::string& key) const;  
    /// getter for an int array
    const std::vector<int>&         get_iarray (const std::string& key) const; 
    /// getter for a std::string array
    const std::vector<std::string>& get_sarray (const std::string& key) const;

    //
    // Write-access
    //
    /// setter for a single double variable
    inline void store(const std::string& key, double value)      {_d_map[key]=value;};
    /// setter for a single int variable
    inline void store(const std::string& key, int value)         {_i_map[key]=value;};
    /// setter for a single std::string variable
    inline void store(const std::string& key, std::string value) {_s_map[key]=value;};
    
    /// method to force-store std::vector<double>
    inline void store(const std::string& key, const std::vector<double>& value) { _darray_map[key] = value; }
    /// method to force-store std::vector<int>
    inline void store(const std::string& key, const std::vector<int>& value) { _iarray_map[key] = value; }
    /// method to force-store std::vector<std::string>
    inline void store(const std::string& key, const std::vector<std::string>& value) { _sarray_map[key] = value; }

    /// method to force-store std::vector<double>
    inline void emplace(const std::string& key, std::vector<double>&& value) { _darray_map[key] = std::move(value); }
    /// method to force-store std::vector<int>
    inline void emplace(const std::string& key, std::vector<int>&& value) { _iarray_map[key] = std::move(value); }
    /// method to force-store std::vector<std::string>
    inline void emplace(const std::string& key, std::vector<std::string>&& value) { _sarray_map[key] = std::move(value); }

    /// method to get hands on writeable std::vector<double> reference
    std::vector<double>& writeable_darray(const std::string& key);
    /// method to get hands on writeable std::vector<int> reference
    std::vector<int>&    writeable_iarray(const std::string& key);
    /// method to get hands on writeable std::vector<string> reference
    std::vector<std::string>& writeable_sarray(const std::string& key);
    
    /// Clear method override
    void clear_data();
    
  protected:
    
    std::map<std::string,double>       _d_map; ///< var. holder map of double
    std::map<std::string,int>          _i_map; ///< var. holder map of int
    std::map<std::string,std::string>  _s_map; ///< var. holder map of std::string
    std::map<std::string,std::vector<double> >      _darray_map; ///< var. holder map of double array
    std::map<std::string,std::vector<int> >         _iarray_map; ///< var. holder map of int array
    std::map<std::string,std::vector<std::string> > _sarray_map; ///< var. holder map of std::string array
    
  };
  
}
  
#endif
/** @} */ // end of doxygen group 
