/*
 * This file is part of pyresidfp, a SID emulation package for Python.
 *
 * Copyright 2018 Sebastian Klemke <packet@nerdheim.de>
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


#include <boost/python.hpp>

#include "SID.h"
using namespace boost::python;

void sid_error_translator(reSIDfp::SIDError const& x) {
    PyErr_SetString(PyExc_UserWarning, x.getMessage());
}

BOOST_PYTHON_MODULE(_pyresidfp)
{
    register_exception_translator<reSIDfp::SIDError>(sid_error_translator);

    class_<reSIDfp::SID, boost::noncopyable>("SID", "MOS6581/MOS8580 emulation.")
            .add_property("chip_model", &reSIDfp::SID::getChipModel, &reSIDfp::SID::setChipModel,
                    "Chip model to emulate.")
            .def("reset", &reSIDfp::SID::reset,
                    "SID reset.")
            .def("input", &reSIDfp::SID::input,
                    "16-bit input (EXT IN). Write 16-bit sample to audio input. NB! The caller\n"
                    "is responsible for keeping the value within 16 bits. Note that to mix in\n"
                    "an external audio signal, the signal should be resampled to 1MHz first to\n"
                    "avoid sampling noise.")
            .def("read", &reSIDfp::SID::read,
                    "Read registers.\n"
                    "\n"
                    "Reading a write only register returns the last char written to any SID register.\n"
                    "The individual bits in this value start to fade down towards zero after a few cycles.\n"
                    "All bits reach zero within approximately $2000 - $4000 cycles.\n"
                    "It has been claimed that this fading happens in an orderly fashion,\n"
                    "however sampling of write only registers reveals that this is not the case.\n"
                    "NOTE: This is not correctly modeled.\n"
                    "The actual use of write only registers has largely been made\n"
                    "in the belief that all SID registers are readable.\n"
                    "To support this belief the read would have to be done immediately\n"
                    "after a write to the same register (remember that an intermediate write\n"
                    "to another register would yield that value instead).\n"
                    "With this in mind we return the last value written to any SID register\n"
                    "for $2000 cycles without modeling the bit fading.")
            .def("write", &reSIDfp::SID::write,
                    "Write registers.")
            .def("mute", &reSIDfp::SID::mute,
                    "SID voice muting.")
            .def("set_sampling_parameters", &reSIDfp::SID::setSamplingParameters,
                    "Setting of SID sampling parameters.\n"
                    "\n"
                    "Use a clock freqency of 985248Hz for PAL C64, 1022730Hz for NTSC C64.\n"
                    "The default end of passband frequency is pass_freq = 0.9*sample_freq/2\n"
                    "for sample frequencies up to ~ 44.1kHz, and 20kHz for higher sample frequencies.\n"
                    "\n"
                    "For resampling, the ratio between the clock frequency and the sample frequency\n"
                    "is limited as follows: 125*clock_freq/sample_freq < 16384\n"
                    "E.g. provided a clock frequency of ~ 1MHz, the sample frequency can not be set\n"
                    "lower than ~ 8kHz. A lower sample frequency would make the resampling code\n"
                    "overfill its 16k sample ring buffer.\n"
                    "\n"
                    "The end of passband frequency is also limited: pass_freq <= 0.9*sample_freq/2\n"
                    "\n"
                    "E.g. for a 44.1kHz sampling rate the end of passband frequency\n"
                    "is limited to slightly below 20kHz.\n"
                    "This constraint ensures that the FIR table is not overfilled.")
            .def("clock", &reSIDfp::SID::clock,
                    "Clock SID forward using chosen output sampling algorithm.")
            .def("clock_silent", &reSIDfp::SID::clockSilent,
                    "Clock SID forward with no audio production.\n"
                    "\n"
                    "Warning:\n"
                    "You can't mix this method of clocking with the audio-producing\n"
                    "clock() because components that don't affect OSC3/ENV3 are not\n"
                    "emulated.")
            .def("set_filter_6581_curve", &reSIDfp::SID::setFilter6581Curve,
                    "Set filter curve parameter for 6581 model.")
            .def("set_filter_8580_curve", &reSIDfp::SID::setFilter8580Curve,
                    "Set filter curve parameter for 8580 model.")
            .def("enable_filter", &reSIDfp::SID::enableFilter,
                    "Enable filter emulation.")
            ;

    enum_<reSIDfp::ChipModel>("ChipModel")
            .value("MOS6581", reSIDfp::ChipModel::MOS6581)
            .value("MOS8580", reSIDfp::ChipModel::MOS8580)
            ;

    enum_<reSIDfp::SamplingMethod>("SamplingMethod")
            .value("DECIMATE", reSIDfp::SamplingMethod::DECIMATE)
            .value("RESAMPLE", reSIDfp::SamplingMethod::RESAMPLE)
            ;
}
