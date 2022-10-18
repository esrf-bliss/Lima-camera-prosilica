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
