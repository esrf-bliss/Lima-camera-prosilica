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

#include "ProsilicaVideoCtrlObj.h"
#include "ProsilicaCamera.h"
#include "ProsilicaSyncCtrlObj.h"

using namespace lima;
using namespace lima::Prosilica;

VideoCtrlObj::VideoCtrlObj(Camera *cam) :
  m_cam(cam),
  m_handle(cam->getHandle()),
  m_live(false)
{
}

VideoCtrlObj::~VideoCtrlObj()
{
}

void VideoCtrlObj::getSupportedVideoMode(std::list<VideoMode> &aList) const
{
  aList.push_back(Y8);
  if(!m_cam->isMonochrome())
    {
      aList.push_back(BAYER_RG8);
      aList.push_back(BAYER_RG16);
      aList.push_back(RGB24);
      aList.push_back(BGR24);
    }
  else
    aList.push_back(Y16);
}

void VideoCtrlObj::getVideoMode(VideoMode &aMode) const
{
  aMode = m_cam->getVideoMode();
}

void VideoCtrlObj::setVideoMode(VideoMode aMode)
{
  m_cam->setVideoMode(aMode);
}

void VideoCtrlObj::setLive(bool flag)
{
  m_live = flag;
  if(flag)
    {
      m_cam->_allocBuffer();
      m_sync->startAcq();
    }
  else
    m_sync->stopAcq();
}

void VideoCtrlObj::getLive(bool &flag) const
{
  flag = m_live;
}

void VideoCtrlObj::setGain(double aGain)
{
  m_cam->setGain(aGain);
}

void VideoCtrlObj::getGain(double &aGain) const
{
  m_cam->getGain(aGain);
}

void VideoCtrlObj::checkBin(Bin& bin)
{
  bin = Bin(1,1);		// Do not manage Hw Bin
}

void VideoCtrlObj::checkRoi(const Roi&, Roi& hw_roi)
{
  tPvUint32 width, height;
  tPvErr error = PvAttrUint32Get(m_handle,"Width",&width);
  error = PvAttrUint32Get(m_handle,"Height",&height);

  hw_roi = Roi(0,0,width,height); // Do not manage Hw Roi
}
