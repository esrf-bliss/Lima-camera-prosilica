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

#include "ProsilicaBufferCtrlObj.h"
#include "ProsilicaSyncCtrlObj.h"
#include "ProsilicaCamera.h"

using namespace lima;
using namespace lima::Prosilica;

BufferCtrlObj::BufferCtrlObj(Camera *cam) :
  m_handle(cam->getHandle()),
  m_status(ePvErrSuccess),
  m_exposing(false)
{
  DEB_CONSTRUCTOR();

  //IMPORTANT: Initialize camera structure. See tPvFrame in PvApi.h for more info.
  memset(&(m_frame[0]),0,sizeof(tPvFrame));
  memset(&(m_frame[1]),0,sizeof(tPvFrame));
  m_frame[0].Context[0] = this;
  m_frame[1].Context[0] = this;
}
void BufferCtrlObj::prepareAcq()
{
  DEB_MEMBER_FUNCT();
  FrameDim dim;
  getFrameDim(dim);
  m_frame[0].ImageBufferSize = m_frame[1].ImageBufferSize = dim.getMemSize();
  
  m_acq_frame_nb = -1;
  tPvFrame& frame0 = m_frame[0];
  frame0.ImageBuffer = (char*) m_buffer_cb_mgr.getFrameBufferPtr(0);
  
  int requested_nb_frames;
  m_sync->getNbFrames(requested_nb_frames);
  if(!requested_nb_frames || requested_nb_frames > 1)
  {
    tPvFrame& frame1 = m_frame[1];
    frame1.ImageBuffer = (char*) m_buffer_cb_mgr.getFrameBufferPtr(1);
  }
  unsigned long FrameSize = 0;
  if((PvAttrUint32Get(m_handle,"TotalBytesPerFrame",&FrameSize)) == ePvErrSuccess)
    {
      DEB_TRACE() << "Camera TotalBytesPerFrame: "<< FrameSize;
      DEB_TRACE() << "Lima Frame size: " << dim.getMemSize();
      DEB_TRACE() << "Lima buffer 0 ptr address: " << frame0.ImageBuffer;
      DEB_TRACE() << "m_frame[0] ptr address: " << &frame0;
    }
}

void BufferCtrlObj::startAcq()
{
  DEB_MEMBER_FUNCT();

  m_exposing = true;
  tPvFrame& frame = m_frame[0];
  
  m_status = PvCaptureQueueFrame(m_handle,&frame,_newFrame);
}

void BufferCtrlObj::_newFrame(tPvFrame* aFrame)
{
  DEB_STATIC_FUNCT();
  BufferCtrlObj *bufferPt = (BufferCtrlObj*)aFrame->Context[0];

  int requested_nb_frames;
  bufferPt->m_sync->getNbFrames(requested_nb_frames);

  bufferPt->m_exposing = false;
  if(bufferPt->m_status || aFrame->Status != ePvErrSuccess) // error
    {
      // it's not really an error,continue
      if(aFrame->Status == ePvErrDataMissing)
	{
	  DEB_WARNING() << DEB_VAR1(aFrame->Status);
          PvCaptureQueueFrame(bufferPt->m_handle,aFrame,_newFrame);
	  return;
	}
      else if(aFrame->Status == ePvErrCancelled) // we stopped the acqusition so not an error
	return;
      else 
	{
	  if(!bufferPt->m_status) // Keep error status
	    bufferPt->m_status = aFrame->Status;

	  if(aFrame->Status)
	    DEB_ERROR() << DEB_VAR1(aFrame->Status);
	    
	  return;
	}
    }
  
  ++bufferPt->m_acq_frame_nb;

  int next_frame_nb = bufferPt->m_acq_frame_nb  + 1;

  tPvFrame* next_frame = &(bufferPt->m_frame[next_frame_nb % 2]);

  bool stopAcq = false;
  if(!requested_nb_frames || 
     bufferPt->m_acq_frame_nb < (requested_nb_frames - 1))
    {
      int buffer_nb, concat_frame_nb;
      bufferPt->m_buffer_cb_mgr.acqFrameNb2BufferNb(
                            next_frame_nb,
						    buffer_nb,
						    concat_frame_nb);
      next_frame->ImageBuffer = (char*)bufferPt->m_buffer_cb_mgr.getBufferPtr(
                                      buffer_nb,
									  concat_frame_nb);
      bufferPt->m_exposing = true;
      bufferPt->m_status = PvCaptureQueueFrame(bufferPt->m_handle,
                                               next_frame,
                                               _newFrame);
    }
  else
    stopAcq = true;
  
  HwFrameInfoType frame_info;
  frame_info.acq_frame_nb = bufferPt->m_acq_frame_nb;
  bufferPt->m_buffer_cb_mgr.newFrameReady(frame_info);
  
  if(stopAcq)
    bufferPt->m_sync->stopAcq(false);
}
