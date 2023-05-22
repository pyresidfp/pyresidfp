#  This file is part of pyresidfp, a SID emulation package for Python.
#
#  Copyright (c) 2018-2023.  Sebastian Klemke <pypi@nerdheim.de>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

from .musical_scale import Tone
from .registers import (
    AttackDecayBits,
    ControlBits,
    ModeVolBits,
    ReadableRegister,
    ResFiltBits,
    SustainReleaseBits,
    WritableRegister,
)
from .sound_interface_device import SoundInterfaceDevice, Voice, _VoiceRegister
from ._version import version, version_tuple, __version__, __version_tuple__

__all__ = [
    "Tone",
    "AttackDecayBits",
    "ControlBits",
    "ModeVolBits",
    "ReadableRegister",
    "ResFiltBits",
    "SustainReleaseBits",
    "WritableRegister",
    "SoundInterfaceDevice",
    "Voice",
]
