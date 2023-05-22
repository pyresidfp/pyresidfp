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

from enum import IntEnum


class Tone(IntEnum):
    """
    Tones from the musical scale. Copied from *6581 Sound Interface Device (SID)*, MOS technology, Norristown, PA,
    October 1982
    """

    C0 = 0x0112  # Hz:   16.35
    C0s = 0x0123  # Hz:   17.32
    D0 = 0x0134  # Hz:   18.35
    D0s = 0x0146  # Hz:   19.44
    E0 = 0x015A  # Hz:   20.60
    F0 = 0x016E  # Hz:   21.83
    F0s = 0x0184  # Hz:   23.12
    G0 = 0x018B  # Hz:   24.50
    G0s = 0x01B3  # Hz:   25.96
    A0 = 0x01CD  # Hz:   27.50
    A0s = 0x01E9  # Hz:   29.14
    B0 = 0x0206  # Hz:   30.87
    C1 = 0x0225  # Hz:   32.70
    C1s = 0x0245  # Hz:   34.65
    D1 = 0x0268  # Hz:   36.71
    D1s = 0x028C  # Hz:   38.89
    E1 = 0x02B3  # Hz:   41.20
    F1 = 0x02DC  # Hz:   43.65
    F1s = 0x0308  # Hz:   46.25
    G1 = 0x0336  # Hz:   49.00
    G1s = 0x0367  # Hz:   51.91
    A1 = 0x039B  # Hz:   55.00
    A1s = 0x03D2  # Hz:   58.27
    B1 = 0x040C  # Hz:   61.74
    C2 = 0x0449  # Hz:   65.41
    C2s = 0x048B  # Hz:   69.30
    D2 = 0x04D0  # Hz:   73.42
    D2s = 0x0519  # Hz:   77.78
    E2 = 0x0567  # Hz:   82.41
    F2 = 0x05B9  # Hz:   87.31
    F2s = 0x0610  # Hz:   92.50
    G2 = 0x066C  # Hz:   98.00
    G2s = 0x06CE  # Hz:  103.83
    A2 = 0x0735  # Hz:  110.00
    A2s = 0x07A3  # Hz:  116.54
    B2 = 0x0817  # Hz:  123.47
    C3 = 0x0893  # Hz:  130.81
    C3s = 0x0915  # Hz:  138.59
    D3 = 0x099F  # Hz:  146.83
    D3s = 0x0A32  # Hz:  155.56
    E3 = 0x0ACD  # Hz:  164.81
    F3 = 0x0B72  # Hz:  174.61
    F3s = 0x0C20  # Hz:  185.00
    G3 = 0x0C08  # Hz:  196.00
    G3s = 0x0D9C  # Hz:  207.65
    A3 = 0x0E6B  # Hz:  220.00
    A3s = 0x0F46  # Hz:  233.08
    B3 = 0x102F  # Hz:  246.94
    C4 = 0x1125  # Hz:  261.63
    C4s = 0x122A  # Hz:  277.18
    D4 = 0x133F  # Hz:  293.66
    D4s = 0x1464  # Hz:  311.13
    E4 = 0x159A  # Hz:  329.63
    F4 = 0x16E3  # Hz:  349.23
    F4s = 0x183F  # Hz:  370.00
    G4 = 0x1981  # Hz:  392.00
    G4s = 0x1B38  # Hz:  415.30
    A4 = 0x1CD6  # Hz:  440.00
    A4s = 0x1E80  # Hz:  466.16
    B4 = 0x205E  # Hz:  493.88
    C5 = 0x224B  # Hz:  523.25
    C5s = 0x2455  # Hz:  554.37
    D5 = 0x267E  # Hz:  587.33
    D5s = 0x28C8  # Hz:  622.25
    E5 = 0x2B34  # Hz:  659.25
    F5 = 0x2DC6  # Hz:  698.46
    F5s = 0x307F  # Hz:  740.00
    G5 = 0x3361  # Hz:  783.99
    G5s = 0x366F  # Hz:  830.61
    A5 = 0x39AC  # Hz:  880.00
    A5s = 0x3D1A  # Hz:  932.33
    B5 = 0x40BC  # Hz:  987.77
    C6 = 0x4495  # Hz: 1046.50
    C6s = 0x48A9  # Hz: 1108.73
    D6 = 0x4CFC  # Hz: 1174.66
    D6s = 0x518F  # Hz: 1244.51
    E6 = 0x5669  # Hz: 1318.51
    F6 = 0x5B8C  # Hz: 1396.91
    F6s = 0x60FE  # Hz: 1479.98
    G6 = 0x6602  # Hz: 1567.98
    G6s = 0x6CDF  # Hz: 1661.22
    A6 = 0x7358  # Hz: 1760.00
    A6s = 0x7A34  # Hz: 1864.65
    B6 = 0x8178  # Hz: 1975.53
    C7 = 0x892B  # Hz: 2093.00
    C7s = 0x9153  # Hz: 2217.46
    D7 = 0x99F7  # Hz: 2349.32
    D7s = 0xA31F  # Hz: 2489.01
    E7 = 0xACD2  # Hz: 2637.02
    F7 = 0xB719  # Hz: 2793.83
    F7s = 0xC1FC  # Hz: 2959.95
    G7 = 0xC085  # Hz: 3135.96
    G7s = 0x0980  # Hz: 3322.44
    A7 = 0xE6B0  # Hz: 3520.00
    A7s = 0xF467  # Hz: 3729.31
    # out-of-range:
    # B7  = 0x1F2F0 Hz: 3951.06
