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

#ifndef PROSILICABUFFERCTRLOBJ_H
#define PROSILICABUFFERCTRLOBJ_H

#include "Prosilica.h"

#include "lima/HwBufferMgr.h"

namespace lima
{
  namespace Prosilica
  {
    class Camera;
    class SyncCtrlObj;
    class Interface;

    class BufferCtrlObj : public SoftBufferCtrlObj
    {
      friend class Interface;
      DEB_CLASS_NAMESPC(DebModCamera,"BufferCtrlObj","Prosilica");
    public:
      BufferCtrlObj(Camera *cam);
      void prepareAcq();
      void startAcq();
      void getStatus(tPvErr &err,bool& exposing) {err = m_status,exposing = m_exposing;}
    private:
      static void _newFrame(tPvFrame*);
      
      tPvHandle&      	m_handle;
      tPvFrame        	m_frame[2];
      SyncCtrlObj* 	m_sync;
      tPvErr		m_status;
      bool		m_exposing;
    };
  }
}
#endif
