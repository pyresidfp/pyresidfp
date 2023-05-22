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

import datetime
import logging
import typing as t
from enum import Enum

from ._pyresidfp import ChipModel, SID, SamplingMethod
from .musical_scale import Tone
from .registers import ReadableRegister, WritableRegister


class Voice(Enum):
    """The three voices of an emulated MOS 6581 / MOS 8580"""

    ONE = 0
    TWO = 1
    THREE = 2


class _VoiceRegister(Enum):
    """Voice-specific writable registers"""

    Freq_Lo = WritableRegister.Voice1_Freq_Lo
    Freq_Hi = WritableRegister.Voice1_Freq_Hi
    Pw_Lo = WritableRegister.Voice1_Pw_Lo
    Pw_Hi = WritableRegister.Voice1_Pw_Hi
    Control_Reg = WritableRegister.Voice1_Control_Reg
    Attack_Decay = WritableRegister.Voice1_Attack_Decay
    Sustain_Release = WritableRegister.Voice1_Sustain_Release

    def voice_register(self, voice: Voice) -> WritableRegister:
        """
        Gets the writable register for the given voice.

        Args:
            voice (Voice): Voice to get register for

        Returns:
            WritableRegister according to register type and voice
        """
        return WritableRegister(self.value + 7 * voice.value)


class _Metaclass(type):
    def __init__(
        cls, name: str, bases: t.Tuple[type, ...], attrs: t.Dict[str, t.Any]
    ) -> None:
        super(_Metaclass, cls).__init__(name, bases, attrs)

        for writable_register in WritableRegister:
            setter = type(cls)._create_setter(writable_register)
            setattr(
                cls,
                writable_register.name,
                property(
                    fget=None,
                    fset=setter,
                    doc="int: Writable {0:s} 8-bit register".format(
                        writable_register.name
                    ),
                ),
            )

        for readable_register in ReadableRegister:
            getter = type(cls)._create_getter(readable_register)
            setattr(
                cls,
                readable_register.name,
                property(
                    fget=getter,
                    doc="int: Readable {0:s} 8-bit register".format(
                        readable_register.name
                    ),
                ),
            )

    @staticmethod
    def _create_setter(register: WritableRegister) -> t.Callable[[t.Any, int], None]:
        def setter(self, value: int) -> None:
            self.write_register(register, value)

        return setter

    @staticmethod
    def _create_getter(register: ReadableRegister) -> t.Callable[[t.Any], int]:
        def getter(self) -> int:
            return self.read_register(register)

        return getter


class SoundInterfaceDevice(metaclass=_Metaclass):
    """Emulation for MOS 6581 / MOS 8580 chips."""

    PAL_CLOCK_FREQUENCY: float = 985248.0
    NTSC_CLOCK_FREQUENCY: float = 1022730.0
    DEFAULT_CLOCK_FREQUENCY: float = PAL_CLOCK_FREQUENCY
    DEFAULT_SAMPLING_RATE: float = 48000.0
    DEFAULT_SAMPLING_METHOD: SamplingMethod = SamplingMethod.RESAMPLE
    DEFAULT_CHIP_MODEL: ChipModel = ChipModel.MOS6581

    def __init__(
        self,
        model: t.Optional[ChipModel] = None,
        sampling_method: t.Optional[SamplingMethod] = None,
        clock_frequency: t.Optional[float] = None,
        sampling_frequency: t.Optional[float] = None,
    ) -> None:
        """Creates a new instance."""
        self._log = logging.getLogger(__name__)
        chip_model = model or type(self).DEFAULT_CHIP_MODEL
        sampling_method = sampling_method or type(self).DEFAULT_SAMPLING_METHOD
        clock_frequency = clock_frequency or type(self).DEFAULT_CLOCK_FREQUENCY
        sampling_frequency = sampling_frequency or type(self).DEFAULT_SAMPLING_RATE
        assert 0 < sampling_frequency <= clock_frequency
        assert 125.0 * clock_frequency / sampling_frequency < 16384.0
        self._sid = SID(
            chip_model, sampling_method, clock_frequency, sampling_frequency
        )

    @property
    def chip_model(self) -> ChipModel:
        """ChipModel: Chip model to emulate"""
        return self._sid.chip_model

    @chip_model.setter
    def chip_model(self, value: ChipModel) -> None:
        self._sid.chip_model = value

    @property
    def sampling_method(self) -> SamplingMethod:
        """SamplingMethod: Method to use for sampling"""
        return self._sid.sampling_method

    @sampling_method.setter
    def sampling_method(self, value: SamplingMethod) -> None:
        self._sid.sampling_method = value

    @property
    def clock_frequency(self) -> float:
        """float: System clock frequency"""
        return self._sid.clock_frequency

    @clock_frequency.setter
    def clock_frequency(self, value: float) -> None:
        self._sid.clock_frequency = value

    @property
    def sampling_frequency(self) -> float:
        """float: Sampling frequency"""
        return self._sid.sampling_frequency

    @sampling_frequency.setter
    def sampling_frequency(self, value: float) -> None:
        self._sid.sampling_frequency = value

    def reset(self):
        """Resets the emulation."""
        self._sid.reset()

    def pulse_width(self, voice: Voice, pulse_width: int) -> None:
        """
        Sets the pulse width on a voice.

        Args:
            voice (Voice): Voice to program
            pulse_width (int): 12-bit value to set
        """
        lo, hi = SoundInterfaceDevice._split_pulse_width(pulse_width)

        self._write_voice_register(voice, _VoiceRegister.Pw_Lo, lo)
        self._write_voice_register(voice, _VoiceRegister.Pw_Hi, hi)

    def tone(self, voice: Voice, tone: Tone) -> None:
        """
        Programs the oscillator of a voice to emit a tone from the musical scale.

        Args:
            voice (Voice): Voice to program
            tone (Tone): Tone to program
        """
        oscillator_value = tone.value

        self.oscillator(voice, oscillator_value)

    def oscillator(self, voice: Voice, value: int) -> None:
        """
        Sets the oscillator of a voice to a given 16-bit value.

        Args:
            voice (Voice): Voice to program
            value (int): 16-bit value to program
        """
        lo, hi = SoundInterfaceDevice._split_16_bit(value)

        self._write_voice_register(voice, _VoiceRegister.Freq_Lo, lo)
        self._write_voice_register(voice, _VoiceRegister.Freq_Hi, hi)

    def control(self, voice: Voice, value: int) -> None:
        """
        Sets the control register of a voice.

        Args:
            voice (Voice): Voice to program
            value (int): 8-bit control word to set
        """
        self._write_voice_register(voice, _VoiceRegister.Control_Reg, value)

    def attack_decay(self, voice: Voice, value: int) -> None:
        """
        Sets the attack/decay value of a voice.

        Args:
            voice (Voice): Voice to program
            value (int): 8-bit value to set
        """
        self._write_voice_register(voice, _VoiceRegister.Attack_Decay, value)

    def sustain_release(self, voice: Voice, value: int) -> None:
        """
        Sets the sustain/release value of a voice.

        Args:
            voice (Voice): Voice to program
            value (int): 8-bit value to set
        """
        self._write_voice_register(voice, _VoiceRegister.Sustain_Release, value)

    def mute(self, voice: Voice):
        self._sid.mute(voice.value, True)

    def unmute(self, voice: Voice):
        self._sid.mute(voice.value, False)

    def filter_cutoff(self, value: int) -> None:
        """
        Sets the filter cutoff value.

        Args:
            value (int): 11-bit value to set
        """
        lo, hi = SoundInterfaceDevice._split_filter_cutoff(value)

        self.write_register(WritableRegister.Filter_Fc_Lo, lo)
        self.write_register(WritableRegister.Filter_Fc_Hi, hi)

    def clock(self, duration: datetime.timedelta) -> list:
        """
        Advances system clock for the given duration and samples output.

        Args:
            duration (datetime.timedelta): Duration to emulate

        Returns:
            list of int containing the sampled output in -32768 to 32767 range
        """
        num_cycles = int(duration.total_seconds() * self.clock_frequency)
        num_samples = int(duration.total_seconds() * self.sampling_frequency)

        self._log.debug(
            "Clock for %f cycles (%f samples estimated) for an interval of %s",
            num_cycles,
            num_samples,
            duration,
        )

        # native sample format is signed 16-bit integers in host endianness
        result = self._sid.clock(num_cycles)

        self._log.debug("Retrieved %d samples", len(result))

        return result

    def write_register(self, register: WritableRegister, value: int) -> None:
        """
        Writes an 8-bit value to a writable register.

        Args:
            register (WritableRegister): Register to write to
            value (int): 8-bit value to write
        """
        assert 0 <= value <= (2**8 - 1)

        self._log.debug("Writing value %d to register %s", value, register)

        self._sid.write(register, value)

    def read_register(self, register: ReadableRegister) -> int:
        """
        Reads the current register value from a readable register.

        Args:
            register (ReadableRegister): Register to read from

        Returns:
            int value in the 0 to 255 range
        """
        self._log.debug("Reading register %s", register)

        result = self._sid.read(register)

        self._log.debug("Read value %d from register %s", result, register)

        return result

    def _write_voice_register(
        self, voice: Voice, register: _VoiceRegister, value: int
    ) -> None:
        writable_register = register.voice_register(voice)

        self.write_register(writable_register, value)

    @staticmethod
    def _split_filter_cutoff(filter_cutoff: int) -> tuple:
        assert 0 <= filter_cutoff <= (2**11 - 1)

        lo = filter_cutoff & 0x07
        hi = (filter_cutoff >> 3) & 0xFF

        return lo, hi

    @staticmethod
    def _split_pulse_width(pulse_width: int) -> tuple:
        assert 0 <= pulse_width <= (2**12 - 1)

        lo = pulse_width & 0xFF
        hi = (pulse_width >> 8) & 0x0F

        return lo, hi

    @staticmethod
    def _split_16_bit(word: int) -> tuple:
        assert 0 <= word <= (2**16 - 1)

        lo = word & 0xFF
        hi = (word >> 8) & 0xFF

        return lo, hi
