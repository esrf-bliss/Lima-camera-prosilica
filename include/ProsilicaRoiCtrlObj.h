//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2024
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

#ifndef PROSILICAROICTRLOBJ_H
#define PROSILICAROICTRLOBJ_H

#include <prosilica_export.h>

#include "lima/HwRoiCtrlObj.h"
#include "lima/HwInterface.h"

namespace lima
{
  namespace Prosilica
  {
    class Camera;
    class SyncCtrlObj;

    class RoiCtrlObj : public HwRoiCtrlObj
    {
      DEB_CLASS_NAMESPC(DebModCamera,"RoiCtrlObj","Prosilica");
    public:
      RoiCtrlObj(Camera* cam, SyncCtrlObj* sync);
      virtual ~RoiCtrlObj();

      virtual void setRoi(const Roi& set_roi);
      virtual void getRoi(Roi& hw_roi);
      virtual void checkRoi(const Roi& set_roi, Roi& hw_roi);
    private:
      Camera*	m_cam;
      SyncCtrlObj* m_sync;
    };
  } // namespace Prosilica
} // namespace lima

#endif // ProsilicaROICTRLOBJ_H
