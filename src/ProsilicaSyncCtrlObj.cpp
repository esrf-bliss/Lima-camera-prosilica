#include <sstream>
#include "ProsilicaSyncCtrlObj.h"
#include "ProsilicaBufferCtrlObj.h"
#include "ProsilicaCamera.h"

using namespace lima;
using namespace lima::Prosilica;

SyncCtrlObj::SyncCtrlObj(Camera *cam,BufferCtrlObj *buffer) :
  m_cam(cam),
  m_handle(cam->getHandle()),
  m_trig_mode(IntTrig),
  m_buffer(buffer),
  m_nb_frames(1),
  m_started(false)
{
  DEB_CONSTRUCTOR();
  m_access_mode = cam->m_as_master ? 
    HwSyncCtrlObj::Master : HwSyncCtrlObj::Monitor;
  
  tPvErr error = PvAttrRangeFloat32(m_handle, "FrameRate", &m_minframerate, &m_maxframerate);
  if(error)
    throw LIMA_HW_EXC(Error,"Can't get  FramRate range");
  DEB_TRACE() << "Frame Rate Range :" << m_minframerate << " - " << m_maxframerate << " Hz";
  
  tPvUint32 min_exp, max_exp;
  error = PvAttrRangeUint32(m_handle, "ExposureValue", &min_exp, &max_exp);
  if(error)
    throw LIMA_HW_EXC(Error,"Can't get  Exposure range");
  DEB_TRACE() << "Exposure Range :" << min_exp << " - " << max_exp << " usec.";
  m_minexposure = min_exp/1e6;
  m_maxexposure = max_exp/1e6;
  
  m_latency = 1/m_maxframerate - m_minexposure;
  
  error = PvAttrFloat32Set(m_handle, "FrameRate", m_maxframerate);
  if(error)
    throw LIMA_HW_EXC(Error,"Can't set FramRate to max");
}

SyncCtrlObj::~SyncCtrlObj()
{
  DEB_DESTRUCTOR();
}

bool SyncCtrlObj::checkTrigMode(TrigMode trig_mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(trig_mode);

  switch(trig_mode)
    {
    case IntTrig:
    case IntTrigMult:
    case ExtTrigMult:
      return true;
    default:
      return false;
    }
}

void SyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(trig_mode);

  tPvErr error;
  if(checkTrigMode(trig_mode))
    {
      switch(trig_mode)
	{
	case ExtTrigMult:
	  error = PvAttrEnumSet(m_handle, "FrameStartTriggerMode", "SyncIn1");
	  if(error)
	    throw LIMA_HW_EXC(Error,"Can't set trigger input");
	  error = PvAttrEnumSet(m_handle, "FrameStartTriggerEvent", "EdgeRising");
	  if(error)
	    throw LIMA_HW_EXC(Error,"Can't change Trigger start to a rising edge");
	  break;
	case IntTrig:
	  error = PvAttrEnumSet(m_handle, "FrameStartTriggerMode", "FixedRate");
	  if(error)
	    {
	      std::stringstream message;
	      message << "could not set trigger mode to FixedRate " << error;
	      throw LIMA_HW_EXC(Error,message.str().c_str());
	    }
	  break;
	case IntTrigMult:
	  error = PvAttrEnumSet(m_handle, "FrameStartTriggerMode", "Software");
	  if(error)
	    {
	      std::stringstream message;
	      message << "could not set trigger mode to FixedRate " << error;
	      throw LIMA_HW_EXC(Error,message.str().c_str());
	    }
	  break;
	}
      m_trig_mode = trig_mode;
    }
  else
    throw LIMA_HW_EXC(NotSupported,"Trigger type not supported");
}

void SyncCtrlObj::getTrigMode(TrigMode &trig_mode)
{
  trig_mode = m_trig_mode;
}

void SyncCtrlObj::setExpTime(double exp_time)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(exp_time);

  tPvErr error = PvAttrEnumSet(m_handle, "ExposureMode", "Manual");
  if(error != ePvErrSuccess)
    throw LIMA_HW_EXC(Error,"Can't set manual exposure");

  tPvUint32 exposure_value = tPvUint32(exp_time * 1e6);
  error = PvAttrUint32Set(m_handle,"ExposureValue",exposure_value);
  if(error != ePvErrSuccess)
    throw LIMA_HW_EXC(Error,"Can't set exposure time failed");

  tPvFloat32 frame_rate = 1/( exp_time + m_latency);
  DEB_TRACE() << frame_rate;
  if (frame_rate < m_minframerate) frame_rate = m_minframerate;
  else if (frame_rate > m_maxframerate) frame_rate = m_maxframerate;
  
  error = PvAttrFloat32Set(m_handle, "FrameRate", frame_rate);
  if(error)
    throw LIMA_HW_EXC(Error,"Can't set FramRate");
  m_exposure = exp_time;
}

void SyncCtrlObj::getExpTime(double &exp_time)
{
  DEB_MEMBER_FUNCT();

  tPvUint32 exposure_value;
  PvAttrUint32Get(m_handle, "ExposureValue", &exposure_value);
  exp_time = exposure_value / 1e6;

  // update frame rate using expo_time + latency
  
  DEB_RETURN() << DEB_VAR1(exp_time);
}

void SyncCtrlObj::setLatTime(double  lat_time)
{
  m_latency = lat_time;
}

void SyncCtrlObj::getLatTime(double& lat_time)
{
  lat_time = m_latency;		// Don't know
}

void SyncCtrlObj::setNbFrames(int  nb_frames)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(nb_frames);

  m_nb_frames = nb_frames;
}

void SyncCtrlObj::getNbFrames(int& nb_frames)
{
  nb_frames = m_nb_frames;
}

void SyncCtrlObj::setNbHwFrames(int  nb_frames)
{
  setNbFrames(nb_frames);
}

void SyncCtrlObj::getNbHwFrames(int& nb_frames)
{
  getNbFrames(nb_frames);
}

void SyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
  // period = exposure + latency
  // framerate = 1 / (exposure + latency)
  // min_latency = (1 / max_framerate) -  min_exposure
  // max_latency = (1 / min_framerate) -  min_exposure
  valid_ranges.min_exp_time = m_minexposure;
  valid_ranges.max_exp_time = m_maxexposure;
  valid_ranges.min_lat_time = 1/m_maxframerate - m_minexposure;
  valid_ranges.max_lat_time = 1/m_minframerate - m_minexposure;
}

void SyncCtrlObj::startAcq()
{
  DEB_MEMBER_FUNCT();
  
  tPvErr error;
  if(!m_started)
    {
      error = PvCaptureStart(m_handle);
      if(error)
	throw LIMA_HW_EXC(Error,"Can't start acquisition capture");

      if(m_buffer)
	m_buffer->startAcq();
      else
	m_cam->startAcq();
      
      if(m_cam->m_as_master)
	{
	  error = PvCommandRun(m_handle, "AcquisitionStart");
	  if(error)
	    throw LIMA_HW_EXC(Error,"Can't start acquisition");
	}  
    }
  if (m_trig_mode == IntTrigMult)
    {
      error = PvCommandRun(m_handle, "FrameStartTriggerSoftware");
      if(error)
	throw LIMA_HW_EXC(Error,"Can't start software trigger");
    }
  m_started = true;
}

void SyncCtrlObj::stopAcq(bool clearQueue)
{
  DEB_MEMBER_FUNCT();
  if(m_started)
    {
      DEB_TRACE() << "Try to stop Acq";
      tPvErr error = PvCommandRun(m_handle,"AcquisitionStop");
      if(error)
	{
	  DEB_ERROR() << "Failed to stop acquisition";
	  throw LIMA_HW_EXC(Error,"Failed to stop acquisition");
	}

      DEB_TRACE() << "Try to stop Capture";
      error = PvCaptureEnd(m_handle);
      if(error)
	{
	  DEB_ERROR() << "Failed to stop acquisition";
	  throw LIMA_HW_EXC(Error,"Failed to stop acquisition");
	}

      if(clearQueue)
	{
	  DEB_TRACE() << "Try to clear queue";
	  error = PvCaptureQueueClear(m_handle);
	  if(error)
	    {
	      DEB_ERROR() << "Failed to stop acquisition";
	      throw LIMA_HW_EXC(Error,"Failed to stop acquisition");
	    }
	}
    }
  m_started = false;
}

void SyncCtrlObj::getStatus(HwInterface::StatusType& status)
{
  DEB_MEMBER_FUNCT();
  if(m_started)
    {
      tPvErr error = ePvErrSuccess;
      if(m_buffer)
	{
	  bool exposing;
	  m_buffer->getStatus(error,exposing);
	  if(error)
	    {
	      status.acq = AcqFault;
	      status.det = DetFault;
	    }
	  else
	    {
	      status.acq = AcqRunning;
	      if (m_trig_mode == IntTrigMult)
		status.det = exposing ? DetIdle : DetExposure;
	      else
		status.det = exposing ? DetExposure : DetIdle;
	    }
	}
      else			// video mode, don't need to be precise
	{
	  status.acq = AcqRunning;
	  status.det = DetExposure;
	}
    }
  else
    {
      status.acq = AcqReady;
      status.det = DetIdle;
    }
  DEB_RETURN() << DEB_VAR1(status);
}
