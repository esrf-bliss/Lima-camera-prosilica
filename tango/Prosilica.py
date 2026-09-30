############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2026
# European Synchrotron Radiation Facility
# CS40220 38043 Grenoble Cedex 9
# FRANCE
#
# Contact: lima@esrf.fr
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################
#=============================================================================
#
# file :        Prosilica.py
#
# description : Python source for the Prosilica and its commands.
#                The class is derived from Device. It represents the
#                CORBA servant object which will be accessed from the
#                network. All commands which can be executed on the
#                Pilatus are implemented in this file.
#
# project :     TANGO Device Server
#
# copyleft :    European Synchrotron Radiation Facility
#               BP 220, Grenoble 38043
#               FRANCE
#
#=============================================================================
#         (c) - Bliss - ESRF
#=============================================================================
#
from tango import AttrWriteType
from tango.server import Device, attribute, command, device_property

from lima import core
from lima import prosilica as ProsilicaAcq
from lima.server import AttrHelper


class Prosilica(Device):

    core.DEB_CLASS(core.DebModule.DebModApplication, 'LimaCCDs')

    # ------------------------------------------------------------------
    #    Static properties
    # ------------------------------------------------------------------
    _ProsilicaCam = None
    """Reference to Lima prosilica camera binding"""
    _ProsilicaInterface = None
    """Reference to Lima prosilica interface binding"""

    # ------------------------------------------------------------------
    #    Device properties
    # ------------------------------------------------------------------
    cam_ip_address = device_property(dtype=str, doc="Camera ip address")

    # ------------------------------------------------------------------
    #    Device initialization
    # ------------------------------------------------------------------
    @core.DEB_MEMBER_FUNCT
    def init_device(self):
        Device.init_device(self)

    # ------------------------------------------------------------------
    #    Commands
    # ------------------------------------------------------------------
    @command(dtype_in=str, doc_in="Attribute name",
              dtype_out=(str,), doc_out="Authorized String value list")
    @core.DEB_MEMBER_FUNCT
    def getAttrStringValueList(self, attr_name):
        return AttrHelper.get_attr_string_value_list(self, attr_name)

    # ------------------------------------------------------------------
    #    Attributes fully dispatched by naming convention
    # ------------------------------------------------------------------
    _pv_gain_range_fget, _ = AttrHelper.make_fget_fset(
        "pv_gain_range", lambda self: self._ProsilicaCam
    )
    pv_gain_range = attribute(
        dtype=(int,),
        max_dim_x=2,
        access=AttrWriteType.READ,
        unit="N/A",
        doc="Camera min/max gain value (PvApi)",
        fget=_pv_gain_range_fget,
    )

    _gain_fget, _gain_fset = AttrHelper.make_fget_fset(
        "gain", lambda self: self._ProsilicaCam
    )
    gain = attribute(
        dtype=float,
        access=AttrWriteType.READ_WRITE,
        unit="N/A",
        doc="Camera gain",
        fget=_gain_fget,
        fset=_gain_fset,
    )

    _pv_gain_fget, _pv_gain_fset = AttrHelper.make_fget_fset(
        "pv_gain", lambda self: self._ProsilicaCam
    )
    pv_gain = attribute(
        dtype=int,
        access=AttrWriteType.READ_WRITE,
        unit="N/A",
        doc="Camera PvApi gain",
        fget=_pv_gain_fget,
        fset=_pv_gain_fset,
    )


#----------------------------------------------------------------------------
# Plugins
#----------------------------------------------------------------------------

def get_control(cam_ip_address="0", _Prosilica=Prosilica, _Camera=ProsilicaAcq.Camera,
                 _Interface=ProsilicaAcq.Interface, **keys):
    print("cam_ip_address", cam_ip_address)
    camera = _Prosilica._ProsilicaCam
    interface = _Prosilica._ProsilicaInterface

    if camera is None:
        camera = _Camera(cam_ip_address)
        interface = _Interface(camera)
        _Prosilica._ProsilicaCam = camera
        _Prosilica._ProsilicaInterface = interface

    return core.CtControl(interface)


def get_tango_specific_class_n_device():
    return Prosilica.TangoClassClass, Prosilica
