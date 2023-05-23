"""
        reSIDfp SID (MOS6581 / MOS8580) extension
        -----------------------------------------

        .. currentmodule:: _pyresidfp

        .. autosummary::
           :toctree: _generate

           SID
    """
from __future__ import annotations
import pyresidfp._pyresidfp
import typing

__all__ = ["ChipModel", "SID", "SamplingMethod"]

class ChipModel:
    """
                   Chip models to emulate.


    Members:

      MOS6581 :
                   The original MOS 6581 chip included in C64


      MOS8580 :
                   The revised MOS 8580 chip included in C64C

    """

    def __eq__(self, other: object) -> bool: ...
    def __getstate__(self) -> int: ...
    def __hash__(self) -> int: ...
    def __index__(self) -> int: ...
    def __init__(self, value: int) -> None: ...
    def __int__(self) -> int: ...
    def __ne__(self, other: object) -> bool: ...
    def __repr__(self) -> str: ...
    def __setstate__(self, state: int) -> None: ...
    @property
    def name(self) -> str:
        """
        :type: str
        """
    @property
    def value(self) -> int:
        """
        :type: int
        """
    MOS6581: pyresidfp._pyresidfp.ChipModel  # value = <ChipModel.MOS6581: 1>
    MOS8580: pyresidfp._pyresidfp.ChipModel  # value = <ChipModel.MOS8580: 2>
    __members__: dict  # value = {'MOS6581': <ChipModel.MOS6581: 1>, 'MOS8580': <ChipModel.MOS8580: 2>}
    pass

class SID:
    """
    MOS6581/MOS8580 emulation.
    """

    def __init__(
        self,
        chip_model: ChipModel,
        method: SamplingMethod,
        clock_frequency: float,
        sampling_frequency: float,
    ) -> None:
        """
        Creates a new instance of SID and sets sampling parameters.

        Use a clock freqency of 985248Hz for PAL C64, 1022730Hz for NTSC C64.
        The default end of passband frequency is pass_freq = 0.9*sample_freq/2
        for sample frequencies up to ~ 44.1kHz, and 20kHz for higher sample frequencies.

        For resampling, the ratio between the clock frequency and the sample frequency
        is limited as follows: 125*clock_freq/sample_freq < 16384
        E.g. provided a clock frequency of ~ 1MHz, the sample frequency can not be set
        lower than ~ 8kHz. A lower sample frequency would make the resampling code
        overfill its 16k sample ring buffer.

        Args:
            chip_model (_pyresidfp.ChipModel):  Chip model to emulate
            method (_pyresidfp.SamplingMethod): Sampling method to use
            clock_frequency (float):            System clock frequency at Hz
            sampling_frequency (float):         Desired output sampling rate

        Raises:
            RuntimeError

        Examples:
            Construct an emulated MOS 6581 chip with resampling sample method, PAL clock frequency,
            and a sampling frequency of 48kHz:

            >>> sid = SID(ChipModel.MOS6581, SamplingMethod.RESAMPLE, 985248.0, 48000.0)
        """
    def clock(self, cycles: int) -> typing.List[int]:
        """
        Clock SID forward using chosen output sampling algorithm and sample.

        Note:
            The number of samples is equal to

            samples = samplingFrequency * cycles / clockFrequency

        Args:
            cycles (int): Number of clock cycles to forward

        Returns:
             :obj:`list` of :obj:`int` samples in range -32768 to 32767
        """
    def enable_filter(self, enable: bool) -> None:
        """
        Enable filter emulation.

        Args:
            enable (bool): False to turn off filter emulation
        """
    def input(self, value: int) -> None:
        """
        16-bit input (EXT IN). Write 16-bit sample to audio input. NB! The caller
        is responsible for keeping the value within 16 bits. Note that to mix in
        an external audio signal, the signal should be resampled to 1MHz first to\
        avoid sampling noise.

        Args:
            value (int): Input level to set
        """
    def mute(self, channel: int, enable: bool) -> None:
        """
        SID voice muting.

        Args:
            channel (int): Channel to modify
            enable (bool): enable muting
        """
    def read(self, offset: int) -> int:
        """
        Read registers.

        Reading a write only register returns the last char written to any SID register.
        The individual bits in this value start to fade down towards zero after a few cycles.
        All bits reach zero within approximately $2000 - $4000 cycles.
        It has been claimed that this fading happens in an orderly fashion,
        however sampling of write only registers reveals that this is not the case.

        Note:
            This is not correctly modeled.

            The actual use of write only registers has largely been made
            in the belief that all SID registers are readable.
            To support this belief the read would have to be done immediately
            after a write to the same register (remember that an intermediate write
            to another register would yield that value instead).
            With this in mind we return the last value written to any SID register
            for $2000 cycles without modeling the bit fading.

        Args:
            offset (int): SID register to read

        Returns:
            char: Value read from chip
        """
    def reset(self) -> None:
        """
        Resets chip model, voice registers, filters and sampling method.

        Raises:
            RuntimeError
        """
    def set_filter_6581_curve(self, curve_position: float) -> None:
        """
        Set filter curve parameter for 6581 model.

        Args:
            curve_position (float): 0 .. 1, where 0 sets center frequency high ("light") and 1 sets it low ("dark"), default is 0.5
        """
    def set_filter_8580_curve(self, curve_position: float) -> None:
        """
        Set filter curve parameter for 8580 model.

        Args:
            curve_position (float):
        """
    def write(self, offset: int, value: int) -> None:
        """
        Write registers.

        Args:
            offset (int): Chip register to write to
            value  (char): Value to write
        """
    @property
    def chip_model(self) -> ChipModel:
        """
                       _pyresidfp.ChipModel: Chip model to emulate.


        :type: ChipModel
        """
    @chip_model.setter
    def chip_model(self, arg1: ChipModel) -> None:
        """
        _pyresidfp.ChipModel: Chip model to emulate.
        """
    @property
    def clock_frequency(self) -> float:
        """
                       float: Clock frequency of chip to emulate


        :type: float
        """
    @clock_frequency.setter
    def clock_frequency(self, arg1: float) -> None:
        """
        float: Clock frequency of chip to emulate
        """
    @property
    def sampling_frequency(self) -> float:
        """
                       float: Frequency at which to sample output


        :type: float
        """
    @sampling_frequency.setter
    def sampling_frequency(self, arg1: float) -> None:
        """
        float: Frequency at which to sample output
        """
    @property
    def sampling_method(self) -> SamplingMethod:
        """
                       _pyresidfp.SamplingMethod: Sampling method to use


        :type: SamplingMethod
        """
    @sampling_method.setter
    def sampling_method(self, arg1: SamplingMethod) -> None:
        """
        _pyresidfp.SamplingMethod: Sampling method to use
        """
    pass

class SamplingMethod:
    """
                   Method to sample emulated anologue output.


    Members:

      DECIMATE :
                   Single-pass down-sampling


      RESAMPLE :
                   Two-pass resampling

    """

    def __eq__(self, other: object) -> bool: ...
    def __getstate__(self) -> int: ...
    def __hash__(self) -> int: ...
    def __index__(self) -> int: ...
    def __init__(self, value: int) -> None: ...
    def __int__(self) -> int: ...
    def __ne__(self, other: object) -> bool: ...
    def __repr__(self) -> str: ...
    def __setstate__(self, state: int) -> None: ...
    @property
    def name(self) -> str:
        """
        :type: str
        """
    @property
    def value(self) -> int:
        """
        :type: int
        """
    DECIMATE: pyresidfp._pyresidfp.SamplingMethod  # value = <SamplingMethod.DECIMATE: 1>
    RESAMPLE: pyresidfp._pyresidfp.SamplingMethod  # value = <SamplingMethod.RESAMPLE: 2>
    __members__: dict  # value = {'DECIMATE': <SamplingMethod.DECIMATE: 1>, 'RESAMPLE': <SamplingMethod.RESAMPLE: 2>}
    pass

__version__ = 1
