/**
 * \file CRTHit.h
 *
 * \ingroup core_DataFormat
 *
 * \brief Class def header for a class larcv::CRTHit
 *
 * @author Temigo
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef __LARCV_CRTHIT_H__
#define __LARCV_CRTHIT_H__

#include "DataFormatTypes.h"
#include <vector>
#include <map>
#include <cstdint>

namespace larcv {
    /**
       \class CRTHit
       \brief CRT Hit information (like sbn::crt::CRTHit)
			 See https://github.com/SBNSoftware/sbnobj/blob/develop/sbnobj/Common/CRT/CRTHit.hh
    */
	  class CRTHit {

		public:
			/// Default constructor
			CRTHit() : _id(kINVALID_INDEX) {};

			/// Default destructor
			virtual ~CRTHit() {}

			/// Getters
			inline InstanceID_t id () const { return _id; }
			inline const std::vector<uint8_t>& feb_id () const { return _feb_id; }
			inline const std::map<uint8_t, std::vector<std::pair<int, float>>>& pesmap () const { return _pesmap; }
			inline float peshit () const { return _peshit; }
			inline uint64_t ts0_s () const { return _ts0_s; }
			inline double ts0_s_corr () const { return _ts0_s_corr; }
			inline double ts0_ns () const { return _ts0_ns; }
			inline double ts0_ns_corr () const { return _ts0_ns_corr; }
			inline double ts1_ns () const { return _ts1_ns; }
			inline int plane () const { return _plane; }
			inline float x_pos () const { return _x_pos; }
			inline float x_err () const { return _x_err; }
			inline float y_pos () const { return _y_pos; }
			inline float y_err () const { return _y_err; }
			inline float z_pos () const { return _z_pos; }
			inline float z_err () const { return _z_err; }
			inline const std::string& tagger () const { return _tagger; }

			/// Setters
			inline void id (InstanceID_t id) { _id = id; }
			inline void feb_id (const std::vector<uint8_t>& f) { _feb_id = f; }
			inline void pesmap(const std::map<uint8_t, std::vector<std::pair<int, float>>>& p) { _pesmap = p; }
			inline void peshit (float p) { _peshit = p; }
			inline void ts0_s (uint64_t t) { _ts0_s = t; }
			inline void ts0_s_corr (double corr) { _ts0_s_corr = corr; }
			inline void ts0_ns (double t) { _ts0_ns = t; }
			inline void ts0_ns_corr (double corr) { _ts0_ns_corr = corr; }
			inline void ts1_ns (double t) { _ts1_ns = t; }
			inline void plane (int p) { _plane = p; }
			inline void x_pos (float x) { _x_pos = x; }
			inline void x_err (float err) { _x_err = err; }
			inline void y_pos (float y) { _y_pos = y; }
			inline void y_err (float err) { _y_err = err; }
			inline void z_pos (float z) { _z_pos = z; }
			inline void z_err (float err) { _z_err = err; }
			inline void tagger (const std::string& t) { _tagger = t; }

		private:
			InstanceID_t _id; ///< "ID" of this CRTHit in CRTHit collection
			std::vector<uint8_t> _feb_id; ///< FEB address
			std::map<uint8_t, std::vector<std::pair<int, float>>> _pesmap; ///< Saves signal hit information (FEB, local-channel and PE) .)
			float _peshit; ///< Total photo-electron (PE) in a crt hit.)

			uint64_t _ts0_s; ///< Second-only part of timestamp T0.
			double _ts0_s_corr; ///< [Honestly, not sure at this point, it was there since long time (BB)])]

			double _ts0_ns; ///< Timestamp T0 (from White Rabbit), in UTC absolute time scale in nanoseconds from the Epoch.)
			double _ts0_ns_corr; ///< [Honestly, not sure at this point, it was there since long time (BB)])]
			double _ts1_ns; ///< timestamp T1 ([signal time w.r.t. Trigger time]), in UTC absolute time scale in nanoseconds from the Epoch.])

			int _plane; ///< Name of the CRT wall (in the form of numbers)

			float _x_pos; ///< position in x-direction (cm)
			float _x_err; ///< position uncertainty in x-direction (cm)
			float _y_pos; ///< position in y-direction (cm)
			float _y_err; ///< position uncertainty in y-direction (cm)
			float _z_pos; ///< position in z-direction (cm)
			float _z_err; ///< position uncertainty in z-direction (cm)

			std::string _tagger; ///< Name of the CRT wall (in the form of strings)
		};

    /**
       \class CRTHitSet
       \brief CRTHit collection
    */
    class CRTHitSet {
    public:
      CRTHitSet() {}
      virtual ~CRTHitSet() {}

      void clear() { _crthit_v.clear(); }

      inline const std::vector<larcv::CRTHit>& as_vector() const
      { return _crthit_v; }

      void set(const std::vector<larcv::CRTHit>& crthit_v);

      void append(const larcv::CRTHit& crthit);

      void emplace_back(larcv::CRTHit&& crthit);

      void emplace(std::vector<larcv::CRTHit>&& crthit_v);

    private:

      std::vector<larcv::CRTHit> _crthit_v; ///< a collection of CRTHits (index maintained)
    };
}

#endif
