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

from enum import IntEnum, IntFlag


class WritableRegister(IntEnum):
    """Writable registers on the MOS 6581"""

    Voice1_Freq_Lo = 0x00
    Voice1_Freq_Hi = 0x01
    Voice1_Pw_Lo = 0x02
    Voice1_Pw_Hi = 0x03
    Voice1_Control_Reg = 0x04
    Voice1_Attack_Decay = 0x05
    Voice1_Sustain_Release = 0x06

    Voice2_Freq_Lo = 0x07
    Voice2_Freq_Hi = 0x08
    Voice2_Pw_Lo = 0x09
    Voice2_Pw_Hi = 0x0A
    Voice2_Control_Reg = 0x0B
    Voice2_Attack_Decay = 0x0C
    Voice2_Sustain_Release = 0x0D

    Voice3_Freq_Lo = 0x0E
    Voice3_Freq_Hi = 0x0F
    Voice3_Pw_Lo = 0x10
    Voice3_Pw_Hi = 0x11
    Voice3_Control_Reg = 0x12
    Voice3_Attack_Decay = 0x13
    Voice3_Sustain_Release = 0x14

    Filter_Fc_Lo = 0x15
    Filter_Fc_Hi = 0x16
    Filter_Res_Filt = 0x17
    Filter_Mode_Vol = 0x18


class ReadableRegister(IntEnum):
    """Readable registers on the MOS 6581"""

    Misc_Potx = 0x19
    Misc_Poty = 0x1A
    Misc_Osc3_Random = 0x1B
    Misc_Env3 = 0x1C


class ControlBits(IntFlag):
    """Bits in control registers"""

    GATE = 2**0
    SYNC = 2**1
    RING_MOD = 2**2
    TEST = 2**3
    TRIANGLE = 2**4
    SAWTOOTH = 2**5
    PULSE = 2**6
    NOISE = 2**7


class AttackDecayBits(IntFlag):
    """Bits in Attack/Decay registers"""

    DCY0 = 2**0
    DCY1 = 2**1
    DCY2 = 2**2
    DCY3 = 2**3
    ATK0 = 2**4
    ATK1 = 2**5
    ATK2 = 2**6
    ATK3 = 2**7


class SustainReleaseBits(IntFlag):
    """Bits in Sustain/Release registers"""

    RIS0 = 2**0
    RIS1 = 2**1
    RIS2 = 2**2
    RIS3 = 2**3
    STN0 = 2**4
    STN1 = 2**5
    STN2 = 2**6
    STN3 = 2**7


class ResFiltBits(IntFlag):
    """Bits in Resonance/Filter register"""

    Filt1 = 2**0
    Filt2 = 2**1
    Filt3 = 2**2
    FiltEX = 2**3
    RES0 = 2**4
    RES1 = 2**5
    RES2 = 2**6
    RES3 = 2**7


class ModeVolBits(IntFlag):
    """Bits in Mode/Volume register"""

    VOL0 = 2**0
    VOL1 = 2**1
    VOL2 = 2**2
    VOL3 = 2**3
    LP = 2**4
    BP = 2**5
    HP = 2**6
    THREE_OFF = 2**7
