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

#include <sstream>
#include "ProsilicaRoiCtrlObj.h"
#include "ProsilicaSyncCtrlObj.h"
#include "ProsilicaCamera.h"

using namespace lima;
using namespace lima::Prosilica;

RoiCtrlObj::RoiCtrlObj(Camera* cam, SyncCtrlObj* sync) :
  m_cam(cam),
  m_sync(sync)
{
  DEB_CONSTRUCTOR();
}

RoiCtrlObj::~RoiCtrlObj()
{
}
void RoiCtrlObj::checkRoi(const Roi& set_roi, Roi& hw_roi)
{
  DEB_MEMBER_FUNCT();
  m_cam->checkRoi(set_roi, hw_roi);
}

void RoiCtrlObj::setRoi(const Roi& roi)
{
  DEB_MEMBER_FUNCT();
  Roi real_roi;
  checkRoi(roi,real_roi);
  m_cam->setRoi(real_roi);

  // force update of the timing ranges, to allow change on the frame rate
  // A hw Roi can change the max. frame-rate
  m_sync->updateValidRanges();
}

void RoiCtrlObj::getRoi(Roi& roi)
{
  DEB_MEMBER_FUNCT();
  m_cam->getRoi(roi);
}
