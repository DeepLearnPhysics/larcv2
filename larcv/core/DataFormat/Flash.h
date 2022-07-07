/**
 * \file Flash.h
 *
 * \ingroup core_DataFormat
 *
 * \brief Class def header for a class larcv::Flash
 *
 * @author Temigo
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef __LARCV_FLASH_H__
#define __LARCV_FLASH_H__

#include "DataFormatTypes.h"
#include <vector>

namespace larcv {
    /**
       \class Flash
       \brief Optical Flash (like recob::OpFlash)
    */
    class Flash {

    public:

        /// Default constructor
        Flash(double time=0, double timeWidth=0, double absTime=0, unsigned int frame=0,
            std::vector<double> PEPerOpDet=std::vector<double>(0),
            bool inBeamFrame=0, int onBeamTime=0, double fastToTotal=1,
            double xCenter=0, double xWidth=0,
            double yCenter=0, double yWidth=0,
            double zCenter=0, double zWidth=0,
            std::vector<double> wireCenters = std::vector<double>(0),
            std::vector<double> wireWidths = std::vector<double>(0),
            InstanceID_t id = kINVALID_INDEX);

        /// Default destructor
        virtual ~Flash() {}

        double TotalPE() const;

        /// Getters
        inline InstanceID_t id         () const { return _id;         }
        inline double time () const { return _time; }
        inline double timeWidth () const { return _timeWidth; }
        inline double absTime () const { return _absTime; }
        inline unsigned int frame () const { return _frame; }
        inline std::vector<double> PEPerOpDet () const { return _PEPerOpDet; }
        inline std::vector<double> wireCenters () const { return _wireCenters; }
        inline std::vector<double> wireWidths () const { return _wireWidths; }
        inline double xCenter () const { return _xCenter; }
        inline double xWidth () const { return _xWidth; }
        inline double yCenter () const { return _yCenter; }
        inline double yWidth () const { return _yWidth; }
        inline double zCenter () const { return _zCenter; }
        inline double zWidth () const { return _zWidth; }
        inline double fastToTotal () const { return _fastToTotal; }
        inline bool inBeamFrame () const { return _inBeamFrame; }
        inline int onBeamTime () const { return _onBeamTime; }

        /// Setters
        inline void id (InstanceID_t id  )  { _id = id;         }
        inline void time (double time) { _time = time; }
        inline void timeWidth (double timeWidth) { _timeWidth = timeWidth; }
        inline void absTime (double absTime) { _absTime = absTime; }
        inline void frame (unsigned int frame) { _frame = frame; }
        inline void PEPerOpDet(std::vector<double> PEPerOpDet) { _PEPerOpDet = PEPerOpDet; }
        inline void wireCenters(std::vector<double> wireCenters) { _wireCenters = wireCenters; }
        inline void wireWidths(std::vector<double> wireWidths) { _wireWidths = wireWidths; }
        inline void xCenter (double xCenter) { _xCenter = xCenter; }
        inline void xWidth (double xWidth) { _xWidth = xWidth; }
        inline void yCenter (double yCenter) { _yCenter = yCenter; }
        inline void yWidth (double yWidth) { _yWidth = yWidth; }
        inline void zCenter (double zCenter) { _zCenter = zCenter; }
        inline void zWidth (double zWidth) { _zWidth = zWidth; }
        inline void fastToTotal (double fastToTotal) { _fastToTotal = fastToTotal; }
        inline void inBeamFrame (bool inBeamFrame) { _inBeamFrame = inBeamFrame; }
        inline void onBeamTime (int onBeamTime) { _onBeamTime = onBeamTime; }

    private:
        // See also https://internal.dunescience.org/doxygen/OpFlash_8h_source.html
        // Mirroring it to facilitate Larsoft recob::OpFlash -> larcv -> CAF translation

        InstanceID_t   _id; ///< "ID" of this flash in FlashSet collection
        double _time; ///< Time on @ref DetectorClocksHardwareTrigger "trigger time scale" [us]
        double _timeWidth; ///< Width of the flash in time [us]
        double _absTime; ///< Time by PMT readout clock
        unsigned int _frame; ///< Frame number
        std::vector<double> _PEPerOpDet; ///< Number of PE on each PMT
        std::vector<double> _wireCenters; ///< Geometric center in each view
        std::vector<double> _wireWidths; ///< Geometric width in each view
        double _xCenter;
        double _xWidth;
        double _yCenter;
        double _yWidth;
        double _zCenter;
        double _zWidth;
        double _fastToTotal; ///< Fast to total light ratio
        bool _inBeamFrame; ///< Is this in the beam frame?
        int _onBeamTime; ///< Is this in time with beam?
    };

    /**
       \class FlashSet
       \brief Flash/Interaction collection
    */
    class FlashSet {
    public:
      FlashSet() {}
      virtual ~FlashSet() {}

      void clear() { _part_v.clear(); }

      inline const std::vector<larcv::Flash>& as_vector() const
      { return _part_v; }

      void set(const std::vector<larcv::Flash>& part_v);

      void append(const larcv::Flash& part);

      void emplace_back(larcv::Flash&& part);

      void emplace(std::vector<larcv::Flash>&& part_v);

    private:

      std::vector<larcv::Flash> _part_v; ///< a collection of flashes (index maintained)
    };
}

#endif
/** @} */ // end of doxygen group
