/**
 * \file MCParticleTree.h
 *
 * \ingroup MeatSlicer
 * 
 * \brief Class def header for a class MCParticleTree
 *
 * @author kazuhiro
 */

/** \addtogroup MeatSlicer

    @{*/
#ifndef MCPARTICLETREE_H
#define MCPARTICLETREE_H

#include <iostream>
#include "DataFormat/Vertex.h"
#include "DataFormat/ROI.h"
#include "FMWKInterface.h"
#include "Base/larcv_base.h"
namespace supera {

  class MCNode {
  public:
    enum class SourceType_t {
      kMCTruth,
      kMCTrack,
      kMCShower,
      kUnknown
    };
    
  public:
    MCNode() : origin(0)
	     , pdg(0)
	     , track_id(larcv::kINVALID_SIZE)
	     , start()
	     , end()
	     , source_index(larcv::kINVALID_SIZE)
	     , source_type(SourceType_t::kUnknown)
    {}    
    ~MCNode(){}

    unsigned short origin;
    int    pdg;
    size_t track_id;
    larcv::Vertex start;
    larcv::Vertex end;
    size_t source_index;
    SourceType_t source_type;
  };

  class MCRoot : public MCNode{
  public:
    MCRoot() : MCNode()
	     , daughter_v()
    {}
    MCRoot(const MCNode& node) : MCNode(node)
			       , daughter_v()
    {}
    ~MCRoot(){}
    /// Check if supplied parent track id belongs to this tree
    bool is_daughter(const size_t& parent_id) const;
    /**
       Check if supplied node belongs to this Root via 0) start-start, 1) end-start matching
    */
    bool is_daughter(const larcv::Vertex& start) const;
    /**
       Compute the smallest positive dT among any particle in this tree and the provided node
     */
    double dt(const MCNode& node) const;

    std::vector<supera::MCNode> daughter_v;
    ::larcv::ROI roi;
  };

  /**
     \class MCParticleTree
     User defined class MCParticleTree ... these comments are used to generate
     doxygen documentation!
  */
  class MCParticleTree : public larcv::larcv_base {
    
  public:
    
    /// Default constructor
    MCParticleTree() : larcv::larcv_base("MCParticleTree")
		     , _origin_filter(0)
    {}
    
    /// Default destructor
    ~MCParticleTree(){}

    void configure(const Config_t& cfg)
    {
      set_verbosity((::larcv::msg::Level_t)(cfg.get<unsigned short>("Verbosity",logger().level())));
      _use_g4_primary = cfg.get<bool>("UseG4Primary");
      _dt_max = cfg.get<double>("DTMax");
    }

    void Register(const std::vector<supera::LArMCTruth_t>&  mctruth_v,
		  const std::vector<supera::LArMCTrack_t>&  mctrack_v,
		  const std::vector<supera::LArMCShower_t>& mcshower_v);

    void FilterOrigin(unsigned short flag)
    { _origin_filter = flag; }

    const std::vector<supera::MCRoot>& PrimaryArray() const
    { return _primary_v; }
    
  private:

    std::vector<supera::MCRoot> _primary_v;
    std::vector<int> _used_mctrack_v;
    std::vector<int> _used_mcshower_v;

    unsigned short _origin_filter;
    double _dt_max;
    bool   _use_g4_primary;

    MCNode FillNode(const supera::LArMCTrack_t& mct);

    MCNode FillNode(const supera::LArMCShower_t& mcs);

    size_t FindPrimary(const larcv::Vertex& start,
		       const size_t parent_id,
		       const size_t ancestor_id) const;

    size_t FindPrimary(const size_t parent_id,
		       const size_t ancestor_id) const;
  
    void DefineNeutrinoPrimary(const std::vector<supera::LArMCTruth_t>& mctruth_v);

    void DefinePrimary(const std::vector<supera::LArMCTruth_t>& mctruth_v);

    void DefinePrimary(const std::vector<supera::LArMCTrack_t>& mctrack_v,
		       const std::vector<supera::LArMCShower_t>& mcshower_v);

    void DefineSecondary(const std::vector<supera::LArMCTrack_t>&  mctrack_v,
			 const std::vector<supera::LArMCShower_t>& mcshower_v);
    
    void EstimateSecondary(const std::vector<supera::LArMCTrack_t>&  mctrack_v,
			   const std::vector<supera::LArMCShower_t>& mcshower_v);
    
  };
}
#endif
/** @} */ // end of doxygen group 

