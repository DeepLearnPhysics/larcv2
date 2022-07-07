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

#include <vector>

namespace larcv {
    /**
       \class Flash
       \brief Optical Flash (like recob::OpFlash)
    */
    class Flash {

    public:

        /// Default constructor
        Flash(double time, double timeWidth, double absTime, unsigned int frame,
            std::vector<double> PEPerOpDet,
            bool inBeamFrame=0, int onBeamTime=0, double fastToTotal=1,
            double xCenter=0, double xWidth=0,
            double yCenter=0, double yWidth=0,
            double zCenter=0, double zWidth=0,
            std::vector<double> wireCenters = std::vector<double>(0),
            std::vector<double> wireWidths = std::vector<double>(0));

        /// Default destructor
        virtual ~Flash() {}

        double TotalPE() const;

        /// Getters
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

    private:
        // See also https://internal.dunescience.org/doxygen/OpFlash_8h_source.html
        // Mirroring it to facilitate Larsoft recob::OpFlash -> larcv -> CAF translation

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
}

#endif
/** @} */ // end of doxygen group
