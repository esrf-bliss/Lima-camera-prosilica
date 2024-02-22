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

#include "ProsilicaInterface.h"
#include "ProsilicaCamera.h"
#include "ProsilicaDetInfoCtrlObj.h"
#include "ProsilicaBufferCtrlObj.h"
#include "ProsilicaVideoCtrlObj.h"
#include "ProsilicaSyncCtrlObj.h"
#include "ProsilicaBinCtrlObj.h"
#include "ProsilicaRoiCtrlObj.h"

using namespace lima;
using namespace lima::Prosilica;


Interface::Interface(Camera *cam) :
  m_cam(cam)
{
  DEB_CONSTRUCTOR();
  m_det_info = new DetInfoCtrlObj(cam);
  if(m_cam->isMonochrome())
    {
      m_buffer = new BufferCtrlObj(cam);
      m_video = NULL;
    }
  else
    {
      m_video = new VideoCtrlObj(cam);
      cam->_allocBuffer();
      cam->m_video = m_video;
      m_buffer = NULL;
    }
  m_sync = new SyncCtrlObj(cam,m_video ? NULL : m_buffer);
  cam->m_sync = m_sync;

  m_bin = new BinCtrlObj(cam);
  m_roi = new RoiCtrlObj(cam);

  if(m_buffer)
    m_buffer->m_sync = m_sync;
  if(m_video)
    m_video->m_sync = m_sync;
}

Interface::~Interface()
{
  DEB_DESTRUCTOR();
  if(m_video)
    {
      delete m_video;
    }
  else
    delete m_buffer;
  delete m_det_info;
  delete m_sync;
  delete m_bin;
}

void Interface::getCapList(CapList &cap_list) const
{
  cap_list.push_back(HwCap(m_sync));
  cap_list.push_back(HwCap(m_det_info));
  cap_list.push_back(HwCap(m_bin));
  cap_list.push_back(HwCap(m_roi));
  if(m_video)
    { 
      cap_list.push_back(HwCap(m_video));
      cap_list.push_back(HwCap(&(m_video->getHwBufferCtrlObj())));
    }
  else
    cap_list.push_back(HwCap(m_buffer));
}

void Interface::reset(ResetLevel reset_level)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(reset_level);

  m_sync->stopAcq();
  m_cam->reset();
}

void Interface::prepareAcq()
{
  DEB_MEMBER_FUNCT();
  if(m_buffer)
    m_buffer->prepareAcq();
}

void Interface::startAcq()
{
  DEB_MEMBER_FUNCT();

  if(m_buffer)
    m_buffer->getBuffer().setStartTimestamp(Timestamp::now());
  else
    m_video->getBuffer().setStartTimestamp(Timestamp::now());
  m_sync->startAcq();
}

void Interface::stopAcq()
{
  DEB_MEMBER_FUNCT();

  m_sync->stopAcq();
}

void Interface::getStatus(StatusType& status)
{
  m_sync->getStatus(status);
}

int Interface::getNbAcquiredFrames()
{
  DEB_MEMBER_FUNCT();

  int aNbAcquiredFrames;
  if(m_buffer)
    aNbAcquiredFrames = m_buffer->getNbAcquiredFrames();
  else
    aNbAcquiredFrames = m_cam->getNbAcquiredFrames();

  DEB_RETURN() << DEB_VAR1(aNbAcquiredFrames);
  return aNbAcquiredFrames;
}

int Interface::getNbHwAcquiredFrames()
{
  return getNbAcquiredFrames();
}

