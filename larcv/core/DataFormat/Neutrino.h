/**
 * \file Neutrino.h
 *
 * \ingroup core_DataFormat
 *
 * \brief Class def header for a class larcv::Neutrino
 *
 * @author Temigo
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef __NEUTRINO_H__
#define __NEUTRINO_H__

#include <iostream>
#include "Vertex.h"
#include "Voxel3DMeta.h"
#include "DataFormatTypes.h"
namespace larcv {

  /**
     \class Neutrino
     \brief Neutrino/Interaction-wise truth information data from GENIE
  */
  class Neutrino{

  public:

    /// Default constructor
    Neutrino()
      : _id               (kINVALID_INDEX) 
      , _interaction_id            (kINVALID_INDEX)  
      , _mcst_index (kINVALID_INDEX)
      , _mct_index  (kINVALID_INDEX)
			, _trackid       (kINVALID_UINT)
			, _lepton_trackid   (kINVALID_UINT)
			, _current_type     (-1)
			, _interaction_mode (-1)
			, _interaction_type (-1)
			, _target           (-1)
			, _nucleon          (-1)
			, _quark            (-1)
			, _w                (0.)
			, _x                (0.)
			, _y                (0.)
			, _qsqr             (0.)
			, _q0               (0.)
			, _q3               (0.)
			//, _pt               (0.)
			, _theta            (0.)
      , _pdg              (0)
      , _lepton_pdg       (0)
      , _px               (0.)
      , _py               (0.)
      , _pz               (0.)
      , _lepton_p         (0.)
      , _dist_travel      (-1)
      , _energy_init      (0.)
      , _energy_deposit   (0.)
      , _process          ("")
      , _num_voxels       (0)
    {}

    /// Default destructor
    ~Neutrino(){}
    /// particle's ID getter
    inline InstanceID_t id               () const { return _id;         } 
    inline InstanceID_t interaction_id        () const { return _interaction_id;         }
    
    // origin/generator info getter
    inline MCSTIndex_t  mcst_index () const { return _mcst_index; }
    inline MCTIndex_t   mct_index  () const { return _mct_index;  }
    inline short  current_type     () const { return _current_type; }
    inline short  interaction_type () const { return _interaction_type; }
		inline short  interaction_mode () const { return _interaction_mode; }
		inline int    target           () const { return _target; }
		inline int    nucleon          () const { return _nucleon; }
		inline int    quark            () const { return _quark; }
		inline double momentum_transfer () const { return _qsqr; }
		inline double momentum_transfer_mag () const { return _q3; }
		inline double energy_transfer () const { return _q0; }
		inline double bjorken_x         () const { return _x; }
		inline double hadronic_invariant_mass () const { return _w; }
		inline double inelasticity     () const { return _y; }
		inline double theta            () const { return _theta; }

    // particle's info getter
    inline unsigned int nu_track_id   () const { return _trackid;    }
    inline unsigned int lepton_track_id   () const { return _lepton_trackid;    }
    inline int          pdg_code   () const { return _pdg;        }
    inline int          lepton_pdg_code   () const { return _lepton_pdg;        }  
    inline double       px         () const { return _px;         }
    inline double       py         () const { return _py;         }
    inline double       pz         () const { return _pz;         }
    inline double       lepton_p   () const { return _lepton_p;     }
    //inline double       pt         () const { return _pt;         }
    inline double       p          () const { return sqrt(pow(_px,2)+pow(_py,2)+pow(_pz,2)); }
    inline const larcv::Vertex& position() const { return _vtx;   }
    inline double       x          () const { return _vtx.x();    }
    inline double       y          () const { return _vtx.y();    }
    inline double       z          () const { return _vtx.z();    }
    inline double       t          () const { return _vtx.t();    }
    inline double       distance_travel () const { return _dist_travel;     }
    inline double       energy_init     () const { return _energy_init;     }
    inline double       energy_deposit  () const { return _energy_deposit;  }
    inline const std::string& creation_process() const { return _process; }
    inline int num_voxels() const { return _num_voxels; }

		inline std::vector<double> traj_x  () const { return _traj_x; }
		inline std::vector<double> traj_y  () const { return _traj_y; }
		inline std::vector<double> traj_z  () const { return _traj_z; }
		inline std::vector<double> traj_t  () const { return _traj_t; }
		inline std::vector<double> traj_px () const { return _traj_px; }
		inline std::vector<double> traj_py () const { return _traj_py; }
		inline std::vector<double> traj_pz () const { return _traj_pz; }
		inline std::vector<double> traj_e  () const { return _traj_e; }

    //
    // Setters
    //
    // generator/origin info setter
    inline void id        (InstanceID_t id  )  { _id = id;         }
    inline void interaction_id     (InstanceID_t interaction_id )  { _interaction_id = interaction_id;       }
    inline void mcst_index      (MCSTIndex_t id )    { _mcst_index = id;    }
    inline void mct_index       (MCTIndex_t id )     { _mct_index = id;     }
    inline void current_type (short curr) {_current_type = curr; }
    inline void interaction_type (short itype) {_interaction_type = itype; }
		inline void interaction_mode (short imode) { _interaction_mode = imode; }
		inline void target       (int target)     { _target = target; }
		inline void nucleon      (int nucleon)     { _nucleon = nucleon; }
		inline void quark        (int quark)       { _quark = quark; }
		inline void momentum_transfer (double q2)  { _qsqr = q2; }
		inline void momentum_transfer_mag (double q3)  { _q3 = q3; }
		inline void energy_transfer (double q0)  { _q0 = q0; }
		inline void bjorken_x    (double x)        { _x = x; }
		inline void hadronic_invariant_mass (double w) { _w = w; }
		inline void inelasticity (double y)        { _y = y; }
		inline void theta (double theta)           { _theta = theta; }

		// particle's info setter
    inline void nu_track_id        (unsigned int id )   { _trackid = id;       }
    inline void lepton_track_id (unsigned int id )   { _lepton_trackid = id;       }
    inline void pdg_code        (int code)           { _pdg = code;         }
    inline void lepton_pdg_code (int code)           { _lepton_pdg = code;         }
    inline void momentum        (double px, double py, double pz) { _px = px; _py = py; _pz = pz; }
    inline void lepton_p        (double lepton_p) {_lepton_p = lepton_p; }
    inline void position        (const larcv::Vertex& vtx) { _vtx = vtx;    }
    inline void position        (double x, double y, double z, double t) { _vtx = Vertex(x,y,z,t); }
    inline void distance_travel (double dist ) { _dist_travel = dist; }
    inline void energy_init     (double e)           { _energy_init = e;    }
    inline void energy_deposit  (double e)           { _energy_deposit = e; }
    inline void creation_process (const std::string& proc) { _process = proc; }
    inline void num_voxels(int count) { _num_voxels = count; }
		inline void add_trajectory_point(double x, double y, double z, double t, double px, double py, double pz, double e) {
			_traj_x.push_back(x);
			_traj_y.push_back(y);
			_traj_z.push_back(z);
			_traj_t.push_back(t);
			_traj_px.push_back(px);
			_traj_py.push_back(py);
			_traj_pz.push_back(pz);
			_traj_e.push_back(e);
		}
    std::string dump() const;

  private:
    InstanceID_t   _id; ///< "ID" of interaction, unique inside the file
    InstanceID_t   _interaction_id; ///Original generator ID
    /// index number in the origin MCShower/MCTrack container array (kINVALID_USHORT if neither)
    MCSTIndex_t  _mcst_index;
    ///< index number in the origin MCTruth container array (kINVALID_USHORT if MCShower/MCTrack)
    MCTIndex_t   _mct_index;
    unsigned int _trackid;     ///< Geant4 track id
		unsigned int _lepton_trackid;
    short _current_type;       ///< if neutrino, shows interaction GENIE current type. else kINVALID_USHORT
		short _interaction_mode;   ///< if neutrino, shows interaction GENIE mode (QE / 1-pi / DIS / ...)
    short _interaction_type;   ///< if neutrino, shows interaction GENIE code. else kINVALID_USHORT
		int _target;
		int _nucleon;           ///< if neutrino, shows nucleon hit 2212 (proton) or 2112 (neutron)
		int _quark;              ///< if neutrino and DIS event, shows quark PDG code
		double _w;
		double _x;         ///< if neutrino, Bjorken variable x
		double _y;
		double _qsqr; ///< if neutrino, momentum transfer Q^2 [MeV]
		double _q0; ///< Energy transfer [MeV]
		double _q3; ///< Maginitude of momentum transfer [MeV]
		double _theta;
		//double _pt;

    int          _pdg;         ///< PDG code
    int          _lepton_pdg;         ///< PDG code of the outgoing lepton
    double       _px,_py,_pz;  ///< (x,y,z) component of particle's initial momentum
    double       _lepton_p;  // outgoing lepton momentum
    Vertex       _vtx;         ///< (x,y,z,t) of particle's vertex information
    double       _dist_travel; ///< filled only if MCTrack origin: distance measured along the trajectory
    double       _energy_init; ///< initial energy of the particle
    double       _energy_deposit; ///< deposited energy of the particle in the detector
    std::string  _process;     ///< string identifier of the particle's creation process from Geant4
    int _num_voxels; ///< Number of voxels in the particle's 3D cluster.

    std::vector<larcv::InstanceID_t> _children_id; ///< "ID" of the children particles in ParticleSet collection
		std::vector<double> _traj_x;
		std::vector<double> _traj_y;
		std::vector<double> _traj_z;
		std::vector<double> _traj_t;
		std::vector<double> _traj_px;
		std::vector<double> _traj_py;
		std::vector<double> _traj_pz;
		std::vector<double> _traj_e;
  };

  /**
     \class NeutrinoSet
     \brief Neutrino/Interaction collection
  */
  class NeutrinoSet {
  public:
		NeutrinoSet() {}
    virtual ~NeutrinoSet() {}

    void clear() { _part_v.clear(); }

    inline const std::vector<larcv::Neutrino>& as_vector() const
    { return _part_v; }

    void set(const std::vector<larcv::Neutrino>& part_v);

    void append(const larcv::Neutrino& part);

    void emplace_back(larcv::Neutrino&& part);

    void emplace(std::vector<larcv::Neutrino>&& part_v);


  private:

    std::vector<larcv::Neutrino> _part_v; ///< a collection of particles (index maintained)
  };
}
#endif
/** @} */ // end of doxygen group
