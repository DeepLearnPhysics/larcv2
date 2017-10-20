/**
 * \file Particle.h
 *
 * \ingroup core_DataFormat
 * 
 * \brief Class def header for a class larcv::Particle
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include <iostream>
#include "Vertex.h"
#include "BBox.h"
#include "DataFormatTypes.h"
namespace larcv {

  /**
     \class Particle
     \brief Particle/Interaction-wise truth information data
  */
  class Particle{
    
  public:
    
    /// Default constructor
    Particle(larcv::ShapeType_t shape=larcv::kShapeUnknown)
      : _index      (kINVALID_INDEX)
      , _shape      (shape)
      , _mcst_index (kINVALID_INDEX)
      , _mct_index  (kINVALID_INDEX)
      , _energy_deposit (0.)
      , _energy_init    (0.)
      , _pdg            (0)
      , _parent_pdg     (0)
      , _trackid        (kINVALID_UINT)
      , _parent_trackid (kINVALID_UINT)
      , _px             (0.)
      , _py             (0.)
      , _pz             (0.)
      , _current_type   (-1)
      , _interaction_type (-1)
      , _process ("")
      , _dist_travel(-1)
    {}
    
    /// Default destructor
    ~Particle(){}

    inline ParticleIndex_t Index         () const { return _index;          }
    inline ShapeType_t     Shape         () const { return _shape;          }
    inline MCSTIndex_t     MCSTIndex     () const { return _mcst_index;     }
    inline MCTIndex_t      MCTIndex      () const { return _mct_index;      }
    inline double          EnergyDeposit () const { return _energy_deposit; }
    inline double          EnergyInit    () const { return _energy_init;    }
    inline int             PdgCode       () const { return _pdg;            }
    inline int             ParentPdgCode () const { return _parent_pdg;     }
    inline unsigned int    TrackID       () const { return _trackid;        }
    inline unsigned int    ParentTrackID () const { return _parent_trackid; }
    inline const larcv::Vertex& Position() const { return _vtx;            }
    inline double         X  () const { return _vtx.X(); }
    inline double         Y  () const { return _vtx.Y(); }
    inline double         Z  () const { return _vtx.Z(); }
    inline double         T  () const { return _vtx.T(); }
    inline double         Px () const { return _px;      }
    inline double         Py () const { return _py;      }
    inline double         Pz () const { return _pz;      }
    inline double         ParentX  () const { return _parent_vtx.X(); }
    inline double         ParentY  () const { return _parent_vtx.Y(); }
    inline double         ParentZ  () const { return _parent_vtx.Z(); }
    inline double         ParentT  () const { return _parent_vtx.T(); }
    inline double         ParentPx () const { return _parent_px;      }
    inline double         ParentPy () const { return _parent_py;      }
    inline double         ParentPz () const { return _parent_pz;      }
    inline const larcv::Vertex& ParentPosition () const { return _parent_vtx;}
    inline short NuCurrentType() const { return _current_type; }
    inline short NuInteractionType() const { return _interaction_type; }
    inline const std::string& CreationProcess() const { return _process; }
    inline const larcv::Vertex& EndPosition() const { return _end_pt; }
    inline const larcv::Vertex& FirstStep() const { return _first_step; }
    inline const larcv::Vertex& LastStep() const { return _last_step; }
    inline double         DistanceTravel() const { return _dist_travel; }
    inline const std::vector<float>& TypeScore() const { return _type_score_v; }

    inline const std::vector<larcv::BBox2D>& BoundingBox2D() const { return _bb2d_v; }
    inline const BBox3D& BoundingBox3D() const { return _bb3d; }
    
    inline void Index         (ParticleIndex_t id  ) { _index = id;         }
    inline void Shape         (ShapeType_t shape ) { _shape = shape;      }
    inline void MCSTIndex     (MCSTIndex_t id )    { _mcst_index = id;    }
    inline void MCTIndex      (MCTIndex_t id )     { _mct_index = id;     }
    inline void EnergyDeposit (double e)           { _energy_deposit = e; }
    inline void EnergyInit    (double e)           { _energy_init = e;    }
    inline void PdgCode       (int code)           { _pdg = code;         }
    inline void ParentPdgCode (int code)           { _parent_pdg = code;  }
    inline void TrackID       (unsigned int id )   { _trackid = id;       }
    inline void ParentTrackID (unsigned int id )   { _parent_trackid = id;}
    inline void Position      (const larcv::Vertex& vtx) { _vtx = vtx;    }
    inline void Position      (double x, double y, double z, double t) { _vtx = Vertex(x,y,z,t); }
    inline void Momentum      (double px, double py, double pz) { _px = px; _py = py; _pz = pz; }
    inline void ParentPosition (const larcv::Vertex& vtx) { _parent_vtx = vtx; }
    inline void ParentPosition (double x, double y, double z, double t) { _parent_vtx = Vertex(x,y,z,t); }
    inline void ParentMomentum (double px, double py, double pz) { _parent_px = px; _parent_py = py; _parent_pz = pz; }
    inline void NuCurrentType (short curr) { curr = -1; _current_type = curr; }
    inline void NuInteractionType (short itype) { itype = -1; _interaction_type = itype; }
    inline void CreationProcess (const std::string& proc) { _process = proc; }
    inline void EndPosition   (const larcv::Vertex& vtx) { _end_pt = vtx; }
    inline void EndPosition   (double x, double y, double z, double t) { _end_pt = Vertex(x,y,z,t); }
    inline void FirstStep     (const larcv::Vertex& vtx) { _first_step = vtx; }
    inline void FirstStep     (double x, double y, double z, double t) { _first_step = Vertex(x,y,z,t); }
    inline void LastStep      (const larcv::Vertex& vtx) { _last_step = vtx; }
    inline void LastStep      (double x, double y, double z, double t) { _last_step = Vertex(x,y,z,t); }
    inline void DistanceTravel ( double dist ) { _dist_travel = dist; }
    inline void TypeScore (const std::vector<float>& score_v) { _type_score_v = score_v; }

    inline void BoundingBox2D(const std::vector<larcv::BBox2D>& bb_v) { _bb2d_v = bb_v; }
    inline void BoundingBox2D(const BBox2D& bb, size_t plane=0) { _bb2d_v.resize(plane+1); _bb2d_v[plane] = bb; }
    inline void BoundingBox3D(const BBox3D& bb) { _bb3d = bb; }
    
    std::string dump() const;
    
  private:

    ParticleIndex_t   _index;
    ShapeType_t  _shape;
    MCSTIndex_t  _mcst_index;
    MCTIndex_t   _mct_index;

    double       _energy_deposit;
    double       _energy_init;
    int          _pdg;
    int          _parent_pdg;
    unsigned int _trackid;
    unsigned int _parent_trackid;
    Vertex       _vtx;
    double       _px,_py,_pz;
    Vertex       _parent_vtx;
    double       _parent_px, _parent_py, _parent_pz;
    short _current_type;
    short _interaction_type;

    std::string _process;
    Vertex      _end_pt;
    std::vector<float> _type_score_v;
    Vertex      _first_step;
    Vertex      _last_step;
    double      _dist_travel;

    std::vector<larcv::BBox2D> _bb2d_v;
    larcv::BBox3D _bb3d;
  };
}
#endif
/** @} */ // end of doxygen group 

