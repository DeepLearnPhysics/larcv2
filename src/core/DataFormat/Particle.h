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
      : _id         (kINVALID_INDEX)
      , _mcst_index (kINVALID_INDEX)
      , _mct_index  (kINVALID_INDEX)
      , _shape      (shape)
      , _current_type     (-1)
      , _interaction_type (-1)
      , _trackid          (kINVALID_UINT)
      , _pdg              (0)
      , _px               (0.)
      , _py               (0.)
      , _pz               (0.)
      , _dist_travel      (-1)
      , _energy_init      (0.)
      , _energy_deposit   (0.)
      , _process          ("")
      , _num_voxels       (0)
      , _parent_trackid   (kINVALID_UINT)
      , _parent_pdg       (0)
      , _ancestor_trackid (kINVALID_UINT)
      , _ancestor_pdg     (0)
    {}

    /// Default destructor
    ~Particle(){}
    /// particle's ID getter
    inline ParticleIndex_t id      () const { return _id;         }
    // origin/generator info getter
    inline ShapeType_t  shape      () const { return _shape;      }
    inline MCSTIndex_t  mcst_index () const { return _mcst_index; }
    inline MCTIndex_t   mct_index  () const { return _mct_index;  }
    inline short nu_current_type     () const { return _current_type; }
    inline short nu_interaction_type () const { return _interaction_type; }
    // particle's info getter
    inline unsigned int track_id   () const { return _trackid;    }
    inline int          pdg_code   () const { return _pdg;        }
    inline double       px         () const { return _px;         }
    inline double       py         () const { return _py;         }
    inline double       pz         () const { return _pz;         }
    inline double       p          () const { return sqrt(pow(_px,2)+pow(_py,2)+pow(_pz,2)); }
    inline const larcv::Vertex& position() const { return _vtx;   }
    inline double       x          () const { return _vtx.x();    }
    inline double       y          () const { return _vtx.y();    }
    inline double       z          () const { return _vtx.z();    }
    inline double       t          () const { return _vtx.t();    }
    inline const larcv::Vertex& end_position () const { return _end_pt;     }
    inline const larcv::Vertex& first_step   () const { return _first_step; }
    inline const larcv::Vertex& last_step    () const { return _last_step;  }
    inline double       distance_travel () const { return _dist_travel;     }
    inline double       energy_init     () const { return _energy_init;     }
    inline double       energy_deposit  () const { return _energy_deposit;  }
    inline const std::string& creation_process() const { return _process; }
    const BBox2D& boundingbox_2d(ProjectionID_t id) const;
    inline const std::vector<larcv::BBox2D>& boundingbox_2d() const { return _bb2d_v; }
    inline const BBox3D& boundingbox_3d() const { return _bb3d; }
    inline int num_voxels() const { return _num_voxels; }

    // parent info getter
    inline unsigned int parent_track_id () const { return _parent_trackid; }
    inline int          parent_pdg_code () const { return _parent_pdg;     }
    inline const larcv::Vertex& parent_position () const { return _parent_vtx;}
    inline double       parent_x        () const { return _parent_vtx.x(); }
    inline double       parent_y        () const { return _parent_vtx.y(); }
    inline double       parent_z        () const { return _parent_vtx.z(); }
    inline double       parent_t        () const { return _parent_vtx.t(); }

    // ancestor info getter
    inline unsigned int ancestor_track_id () const { return _ancestor_trackid; }
    inline int          ancestor_pdg_code () const { return _ancestor_pdg;     }
    inline const larcv::Vertex& ancestor_position () const { return _ancestor_vtx;}
    inline double       ancestor_x        () const { return _ancestor_vtx.x(); }
    inline double       ancestor_y        () const { return _ancestor_vtx.y(); }
    inline double       ancestor_z        () const { return _ancestor_vtx.z(); }
    inline double       ancestor_t        () const { return _ancestor_vtx.t(); }

    //
    // Setters
    //
    // generator/origin info setter
    inline void id              (InstanceID_t id  )  { _id = id;         }
    inline void mcst_index      (MCSTIndex_t id )    { _mcst_index = id;    }
    inline void mct_index       (MCTIndex_t id )     { _mct_index = id;     }
    inline void shape           (ShapeType_t shape ) { _shape = shape;      }
    inline void nu_current_type (short curr) {_current_type = curr; }
    inline void nu_interaction_type (short itype) {_interaction_type = itype; }
    // particle's info setter
    inline void track_id        (unsigned int id )   { _trackid = id;       }
    inline void pdg_code        (int code)           { _pdg = code;         }
    inline void momentum        (double px, double py, double pz) { _px = px; _py = py; _pz = pz; }
    inline void position        (const larcv::Vertex& vtx) { _vtx = vtx;    }
    inline void position        (double x, double y, double z, double t) { _vtx = Vertex(x,y,z,t); }
    inline void end_position    (const larcv::Vertex& vtx) { _end_pt = vtx; }
    inline void end_position    (double x, double y, double z, double t) { _end_pt = Vertex(x,y,z,t); }
    inline void first_step      (const larcv::Vertex& vtx) { _first_step = vtx; }
    inline void first_step      (double x, double y, double z, double t) { _first_step = Vertex(x,y,z,t); }
    inline void last_step       (const larcv::Vertex& vtx) { _last_step = vtx; }
    inline void last_step       (double x, double y, double z, double t) { _last_step = Vertex(x,y,z,t); }
    inline void distance_travel ( double dist ) { _dist_travel = dist; }
    inline void energy_init     (double e)           { _energy_init = e;    }
    inline void energy_deposit  (double e)           { _energy_deposit = e; }
    inline void creation_process (const std::string& proc) { _process = proc; }
    inline void boundingbox_2d(const std::vector<larcv::BBox2D>& bb_v) { _bb2d_v = bb_v; }
    inline void boundingbox_2d(const BBox2D& bb, ProjectionID_t id) { _bb2d_v.resize(id+1); _bb2d_v[id] = bb; }
    inline void boundingbox_3d(const BBox3D& bb) { _bb3d = bb; }
    inline void num_voxels(int count) { _num_voxels = count; }
    //inline void type_score (const std::vector<float>& score_v) { _type_score_v = score_v; }
    // parent info setter
    inline void parent_track_id (unsigned int id )   { _parent_trackid = id;}
    inline void parent_pdg_code (int code)           { _parent_pdg = code;  }
    inline void parent_position (const larcv::Vertex& vtx) { _parent_vtx = vtx; }
    inline void parent_position (double x, double y, double z, double t) { _parent_vtx = Vertex(x,y,z,t); }
    // ancestor info setter
    inline void ancestor_track_id (unsigned int id )   { _ancestor_trackid = id;}
    inline void ancestor_pdg_code (int code)           { _ancestor_pdg = code;  }
    inline void ancestor_position (const larcv::Vertex& vtx) { _ancestor_vtx = vtx; }
    inline void ancestor_position (double x, double y, double z, double t) { _ancestor_vtx = Vertex(x,y,z,t); }

    std::string dump() const;

  private:

    InstanceID_t   _id; ///< "ID" of this particle
    /// index number in the origin MCShower/MCTrack container array (kINVALID_USHORT if neither)
    MCSTIndex_t  _mcst_index;
    ///< index number in the origin MCTruth container array (kINVALID_USHORT if MCShower/MCTrack)
    MCTIndex_t   _mct_index;
    ShapeType_t  _shape;       ///< shows if it is (e+/e-/gamma) or other particle types
    short _current_type;       ///< if neutrino, shows interaction GENIE current type. else kINVALID_USHORT
    short _interaction_type;   ///< if neutrino, shows interaction GENIE code. else kINVALID_USHORT

    unsigned int _trackid;     ///< Geant4 track id
    int          _pdg;         ///< PDG code
    double       _px,_py,_pz;  ///< (x,y,z) component of particle's initial momentum
    Vertex       _vtx;         ///< (x,y,z,t) of particle's vertex information
    Vertex       _end_pt;      ///< (x,y,z,t) at which particle disappeared from G4WorldVolume
    Vertex       _first_step;  ///< (x,y,z,t) of the first energy deposition point in the detector
    Vertex       _last_step;   ///< (x,y,z,t) of the last energy deposition point in the detector
    double       _dist_travel; ///< filled only if MCTrack origin: distance measured along the trajectory
    double       _energy_init; ///< initial energy of the particle
    double       _energy_deposit; ///< deposited energy of the particle in the detector
    std::string  _process;     ///< string identifier of the particle's creation process from Geant4
    std::vector<larcv::BBox2D> _bb2d_v; ///< bounding box of particle's trajectory in 2D projections. index = ProjectionID_t
    larcv::BBox3D _bb3d; ///< bounding box of particle's trajectory in 3D
    int _num_voxels; ///< Number of voxels in the particle's 3D cluster.

    unsigned int _parent_trackid; ///< Geant4 track id of the parent particle
    int          _parent_pdg;     ///< PDG code of the parent particle
    Vertex       _parent_vtx; ///< (x,y,z,t) of parent's vertex information

    unsigned int _ancestor_trackid; ///< Geant4 track id of the ancestor particle
    int          _ancestor_pdg;     ///< PDG code of the ancestor particle
    Vertex       _ancestor_vtx; ///< (x,y,z,t) of ancestor's vertex information

    //std::vector<float> _type_score_v;

  };

  /**
     \class ParticleSet
     \brief Particle/Interaction collection
  */
  class ParticleSet {
  public:
    ParticleSet() {}
    virtual ~ParticleSet() {}

    void clear() { _part_v.clear(); }

    inline const std::vector<larcv::Particle>& as_vector() const
    { return _part_v; }

    void set(const std::vector<larcv::Particle>& part_v);

    void append(const larcv::Particle& part);

    void emplace_back(larcv::Particle&& part);

    void emplace(std::vector<larcv::Particle>&& part_v);

  private:

    std::vector<larcv::Particle> _part_v; ///< a collection of particles (index maintained)
  };
}
#endif
/** @} */ // end of doxygen group
