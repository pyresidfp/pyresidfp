/*
 * This file is part of pyresidfp, a SID emulation package for Python.
 *
 * Copyright (c) 2018-2023.  Sebastian Klemke <pypi@nerdheim.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "PythonSid.h"

namespace py = pybind11;
namespace sid = reSIDfp;
namespace pysid = pyreSIDfp;

PYBIND11_MODULE(_pyresidfp, m) {
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const sid::SIDError &e) {
            PyErr_SetString(PyExc_RuntimeError, e.getMessage());
        }
    });

    m.doc() = R"pbdoc(
        reSIDfp SID (MOS6581 / MOS8580) extension
        -----------------------------------------

        .. currentmodule:: _pyresidfp

        .. autosummary::
           :toctree: _generate

           SID
    )pbdoc";

#ifdef PROJECT_VERSION
    m.attr("__version__") = PROJECT_VERSION;
#else
    m.attr("__version__") = "dev";
#endif


    py::enum_<sid::ChipModel>(m, "ChipModel", R"pbdoc(
               Chip models to emulate.
            )pbdoc")

            .value("MOS6581", sid::ChipModel::MOS6581, R"pbdoc(
               The original MOS 6581 chip included in C64
            )pbdoc")

            .value("MOS8580", sid::ChipModel::MOS8580, R"pbdoc(
               The revised MOS 8580 chip included in C64C
            )pbdoc");

    py::enum_<sid::SamplingMethod>(m, "SamplingMethod", R"pbdoc(
               Method to sample emulated anologue output.
            )pbdoc")

            .value("DECIMATE", sid::SamplingMethod::DECIMATE, R"pbdoc(
               Single-pass down-sampling
            )pbdoc")

            .value("RESAMPLE", sid::SamplingMethod::RESAMPLE, R"pbdoc(
               Two-pass resampling
            )pbdoc");

    py::class_<::pysid::PythonSid>(m, "SID", R"pbdoc(
               MOS6581/MOS8580 emulation.
            )pbdoc")

            .def(py::init<sid::ChipModel, sid::SamplingMethod, double, double>(),
                    py::arg("chip_model"), py::arg("method"), py::arg("clock_frequency"), py::arg("sampling_frequency"),
                    R"pbdoc(
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
            )pbdoc")

            .def_property("chip_model", &::pysid::PythonSid::getChipModel, &::pysid::PythonSid::setChipModel, R"pbdoc(
               _pyresidfp.ChipModel: Chip model to emulate.
            )pbdoc")

            .def_property("sampling_method", &::pysid::PythonSid::getSamplingMethod, &::pysid::PythonSid::setSamplingMethod, R"pbdoc(
               _pyresidfp.SamplingMethod: Sampling method to use
            )pbdoc")

            .def_property("clock_frequency", &::pysid::PythonSid::getClockFrequency, &::pysid::PythonSid::setClockFrequency, R"pbdoc(
               float: Clock frequency of chip to emulate
            )pbdoc")

            .def_property("sampling_frequency", &::pysid::PythonSid::getSamplingFrequency, &::pysid::PythonSid::setSamplingFrequency, R"pbdoc(
               float: Frequency at which to sample output
            )pbdoc")

            .def("reset", &::pysid::PythonSid::reset, R"pbdoc(
               Resets chip model, voice registers, filters and sampling method.

               Raises:
                   RuntimeError
            )pbdoc")

            .def("input", &::pysid::PythonSid::input, py::arg("value"), R"pbdoc(
               16-bit input (EXT IN). Write 16-bit sample to audio input. NB! The caller
               is responsible for keeping the value within 16 bits. Note that to mix in
               an external audio signal, the signal should be resampled to 1MHz first to
               avoid sampling noise.

               Args:
                   value (int): Input level to set
            )pbdoc")

            .def("read", &::pysid::PythonSid::read, py::arg("offset"), R"pbdoc(
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
            )pbdoc")

            .def("write", &::pysid::PythonSid::write, py::arg("offset"), py::arg("value"), R"pbdoc(
               Write registers.

               Args:
                   offset (int): Chip register to write to
                   value  (char): Value to write
            )pbdoc")

            .def("mute", &::pysid::PythonSid::mute, py::arg("channel"), py::arg("enable"), R"pbdoc(
               SID voice muting.

               Args:
                   channel (int): Channel to modify
                   enable (bool): enable muting
            )pbdoc")

            .def("clock", &::pysid::PythonSid::clock, py::arg("cycles"), R"pbdoc(
               Clock SID forward using chosen output sampling algorithm and sample.

               Note:
                   The number of samples is equal to

                   samples = samplingFrequency * cycles / clockFrequency

               Args:
                   cycles (int): Number of clock cycles to forward

               Returns:
                    :obj:`list` of :obj:`int` samples in range -32768 to 32767
            )pbdoc")

            .def("set_filter_6581_curve", &::pysid::PythonSid::setFilter6581Curve, py::arg("curve_position"), R"pbdoc(
               Set filter curve parameter for 6581 model.

               Args:
                   curve_position (float): 0 .. 1, where 0 sets center frequency high ("light") and 1 sets it low ("dark"), default is 0.5
            )pbdoc")

            .def("set_filter_8580_curve", &::pysid::PythonSid::setFilter8580Curve, py::arg("curve_position"), R"pbdoc(
               Set filter curve parameter for 8580 model.

               Args:
                   curve_position (float):
            )pbdoc")

            .def("enable_filter", &::pysid::PythonSid::enableFilter, py::arg("enable"), R"pbdoc(
               Enable filter emulation.

               Args:
                   enable (bool): False to turn off filter emulation
            )pbdoc");
}
