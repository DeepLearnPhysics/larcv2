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

    inline ParticleIndex_t index           () const { return _index;          }
    inline ShapeType_t     shape           () const { return _shape;          }
    inline MCSTIndex_t     mcst_index      () const { return _mcst_index;     }
    inline MCTIndex_t      mct_index       () const { return _mct_index;      }
    inline double          energy_deposit  () const { return _energy_deposit; }
    inline double          energy_init     () const { return _energy_init;    }
    inline int             pdg_code        () const { return _pdg;            }
    inline int             parent_pdg_code () const { return _parent_pdg;     }
    inline unsigned int    track_id        () const { return _trackid;        }
    inline unsigned int    parent_track_id () const { return _parent_trackid; }
    inline const larcv::Vertex& Position() const { return _vtx;            }
    inline double         x  () const { return _vtx.x(); }
    inline double         y  () const { return _vtx.y(); }
    inline double         z  () const { return _vtx.z(); }
    inline double         t  () const { return _vtx.t(); }
    inline double         px () const { return _px;      }
    inline double         py () const { return _py;      }
    inline double         pz () const { return _pz;      }
    inline double         parent_x  () const { return _parent_vtx.x(); }
    inline double         parent_y  () const { return _parent_vtx.y(); }
    inline double         parent_z  () const { return _parent_vtx.z(); }
    inline double         parent_t  () const { return _parent_vtx.t(); }
    inline double         parent_px () const { return _parent_px;      }
    inline double         parent_py () const { return _parent_py;      }
    inline double         parent_pz () const { return _parent_pz;      }
    inline const larcv::Vertex& parent_position () const { return _parent_vtx;}
    inline short nu_current_type() const { return _current_type; }
    inline short nu_interaction_type() const { return _interaction_type; }
    inline const std::string& creation_process() const { return _process; }
    inline const larcv::Vertex& end_position() const { return _end_pt; }
    inline const larcv::Vertex& first_step() const { return _first_step; }
    inline const larcv::Vertex& last_step() const { return _last_step; }
    inline double         distance_travel() const { return _dist_travel; }
    inline const std::vector<float>& type_score() const { return _type_score_v; }

    const BBox2D& boundingbox_2d(ProjectionID_t id) const;
    inline const std::vector<larcv::BBox2D>& boundingbox_2d() const { return _bb2d_v; }
    inline const BBox3D& boundingbox_3d() const { return _bb3d; }
    
    inline void index         (ParticleIndex_t id  ) { _index = id;         }
    inline void shape         (ShapeType_t shape ) { _shape = shape;      }
    inline void mcst_index     (MCSTIndex_t id )    { _mcst_index = id;    }
    inline void mct_index      (MCTIndex_t id )     { _mct_index = id;     }
    inline void energy_deposit (double e)           { _energy_deposit = e; }
    inline void energy_init    (double e)           { _energy_init = e;    }
    inline void pdg_code       (int code)           { _pdg = code;         }
    inline void parent_pdg_code (int code)           { _parent_pdg = code;  }
    inline void track_id       (unsigned int id )   { _trackid = id;       }
    inline void parent_track_id (unsigned int id )   { _parent_trackid = id;}
    inline void position      (const larcv::Vertex& vtx) { _vtx = vtx;    }
    inline void position      (double x, double y, double z, double t) { _vtx = Vertex(x,y,z,t); }
    inline void momentum      (double px, double py, double pz) { _px = px; _py = py; _pz = pz; }
    inline void parent_position (const larcv::Vertex& vtx) { _parent_vtx = vtx; }
    inline void parent_position (double x, double y, double z, double t) { _parent_vtx = Vertex(x,y,z,t); }
    inline void parent_momentum (double px, double py, double pz) { _parent_px = px; _parent_py = py; _parent_pz = pz; }
    inline void nu_current_type (short curr) { curr = -1; _current_type = curr; }
    inline void nu_interaction_type (short itype) { itype = -1; _interaction_type = itype; }
    inline void creation_process (const std::string& proc) { _process = proc; }
    inline void end_position   (const larcv::Vertex& vtx) { _end_pt = vtx; }
    inline void end_position   (double x, double y, double z, double t) { _end_pt = Vertex(x,y,z,t); }
    inline void first_step     (const larcv::Vertex& vtx) { _first_step = vtx; }
    inline void first_step     (double x, double y, double z, double t) { _first_step = Vertex(x,y,z,t); }
    inline void last_step      (const larcv::Vertex& vtx) { _last_step = vtx; }
    inline void last_step      (double x, double y, double z, double t) { _last_step = Vertex(x,y,z,t); }
    inline void distance_travel ( double dist ) { _dist_travel = dist; }
    inline void type_score (const std::vector<float>& score_v) { _type_score_v = score_v; }

    inline void boundingbox_2d(const std::vector<larcv::BBox2D>& bb_v) { _bb2d_v = bb_v; }
    inline void boundingbox_2d(const BBox2D& bb, ProjectionID_t id=0) { _bb2d_v.resize(id+1); _bb2d_v[id] = bb; }
    inline void boundingbox_3d(const BBox3D& bb) { _bb3d = bb; }
    
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

