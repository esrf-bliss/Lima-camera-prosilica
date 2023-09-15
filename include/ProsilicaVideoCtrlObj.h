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

#ifndef PROSILICAVIDEOCTRLOBJ_H
#define PROSILICAVIDEOCTRLOBJ_H
#include "Prosilica.h"
#include "lima/HwVideoCtrlObj.h"

namespace lima
{
  namespace Prosilica
  {
    class Camera;
    class SyncCtrlObj;
    class VideoCtrlObj : public HwVideoCtrlObj
    {
      friend class Interface;
      DEB_CLASS_NAMESPC(DebModCamera,"VideoCtrlObj","Prosilica");
    public:
      VideoCtrlObj(Camera* cam);
      virtual ~VideoCtrlObj();
 
      virtual void getSupportedVideoMode(std::list<VideoMode> &aList) const;
      virtual void setVideoMode(VideoMode);
      virtual void getVideoMode(VideoMode&) const;

      virtual void setLive(bool);
      virtual void getLive(bool&) const;

      virtual void setGain(double);
      virtual void getGain(double&) const;

      virtual void checkBin(Bin& bin);
      virtual void checkRoi(const Roi& set_roi, Roi& hw_roi);

      virtual void setBin(const Bin&){};
      virtual void setRoi(const Roi&){};

    private:
      Camera*	 	m_cam;
      tPvHandle& 	m_handle;
      bool	 	m_live;
      SyncCtrlObj* 	m_sync;
    };
  }
}
#endif
