//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2023
// European Synchrotron Radiation Facility
// CS40220 38043 Grenoble Cedex 9
// FRANCE
//
// Contact: lima@esrf.fr
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################

#ifndef PROSILICACAMERA_H
#define PROSILICACAMERA_H
#include "Prosilica.h"
#include "lima/Debug.h"
#include "lima/Constants.h"
#include "lima/HwMaxImageSizeCallback.h"

namespace lima
{
  namespace Prosilica
  {
    class SyncCtrlObj;
    class VideoCtrlObj;
    class Camera : public HwMaxImageSizeCallbackGen
    {
      friend class Interface;
      friend class VideoCtrlObj;
      DEB_CLASS_NAMESPC(DebModCamera,"Camera","Prosilica");
    public:
      Camera(const std::string& ip_addr,bool master = true, bool mono_forced = false);
      ~Camera();
      
      bool isMonochrome() const;
      tPvHandle& getHandle() {return m_handle;}
      void getMaxWidthHeight(tPvUint32& width,tPvUint32& height)
      {width = m_maxwidth, height = m_maxheight;}
      int getNbAcquiredFrames() const {return m_acq_frame_nb;}

      VideoMode getVideoMode() const;
      void 	setVideoMode(VideoMode);
      
      void checkBin(Bin&);
      void setBin(const Bin&);
      void getBin(Bin&);

      void checkRoi(const Roi& set_roi, Roi& hw_roi);
      void setRoi(const Roi&);
      void getRoi(Roi&);
      
      void setGain(double);
      void getGain(double&) const;
      void setPvGain(unsigned long);
      void getPvGain(unsigned long&) const;
      void getPvGainRange(unsigned long&, unsigned long&) const;

      void	getCameraName(std::string& name);
	
      void 	startAcq();
      void	reset();

      bool		m_as_master;

    private:
      void 		_allocBuffer();
      static void 	_newFrameCBK(tPvFrame*);
      void		_newFrame(tPvFrame*);

      bool 		m_cam_connected;
      tPvHandle		m_handle;
      char		m_camera_name[128];
      char		m_sensor_type[64];
      tPvUint32		m_ufirmware_maj, m_ufirmware_min;
      tPvUint32		m_maxwidth, m_maxheight;
      tPvUint32		m_maxgain;
      tPvUint32		m_mingain;
      
      tPvUint32		m_uid;
      tPvFrame		m_frame[2];
      Bin         m_bin;
      Roi         m_roi;
      
      SyncCtrlObj*	m_sync;
      VideoCtrlObj*	m_video;
      VideoMode		m_video_mode;
      int		m_acq_frame_nb;
      bool		m_continue_acq;
      bool              m_mono_forced;
    };
  }
}
#endif
