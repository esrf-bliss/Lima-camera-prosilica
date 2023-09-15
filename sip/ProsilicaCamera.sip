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

namespace Prosilica
{
  class Camera
  {
%TypeHeaderCode
#include <ProsilicaCamera.h>
%End
  public:
    Camera(const std::string& ip_addr,bool=true, bool mono_forced = false);
    ~Camera();
      
    bool isMonochrome() const;
    void getMaxWidthHeight(unsigned long& width /Out/,unsigned long& height /Out/);
    int getNbAcquiredFrames() const;

    void checkBin(Bin& /In,Out/);
    void setBin(const Bin&);
    void getBin(Bin& /Out/);
    void setGain(double);
    void getGain(double& /Out/) const;
    void setPvGain(unsigned long);
    void getPvGain(unsigned long& /Out/) const;
    void getPvGainRange(unsigned long& /Out/, unsigned long& /Out/) const;
    
    VideoMode getVideoMode() const;
    void 	setVideoMode(VideoMode);
      
    void 	startAcq();
    void	reset();
  private:
   Camera(const Prosilica::Camera&);
  };
};
