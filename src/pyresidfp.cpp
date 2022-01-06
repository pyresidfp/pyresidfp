/*
 * This file is part of pyresidfp, a SID emulation package for Python.
 *
 * Copyright (c) 2018.  Sebastian Klemke <packet@nerdheim.de>
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

               Filter for 6581 chip
               --------------------

               The SID filter is modeled with a two-integrator-loop biquadratic filter,
               which has been confirmed by Bob Yannes to be the actual circuit used in
               the SID chip.

               Measurements show that excellent emulation of the SID filter is achieved,
               except when high resonance is combined with high sustain levels.
               In this case the SID op-amps are performing less than ideally and are
               causing some peculiar behavior of the SID filter. This however seems to
               have more effect on the overall amplitude than on the color of the sound.

               The theory for the filter circuit can be found in "Microelectric Circuits"
               by Adel S. Sedra and Kenneth C. Smith.
               The circuit is modeled based on the explanation found there except that
               an additional inverter is used in the feedback from the bandpass output,
               allowing the summer op-amp to operate in single-ended mode. This yields
               filter outputs with levels independent of Q, which corresponds with the
               results obtained from a real SID.

               We have been able to model the summer and the two integrators of the circuit
               to form components of an IIR filter.
               Vhp is the output of the summer, Vbp is the output of the first integrator,
               and Vlp is the output of the second integrator in the filter circuit.

               According to Bob Yannes, the active stages of the SID filter are not really
               op-amps. Rather, simple NMOS inverters are used. By biasing an inverter
               into its region of quasi-linear operation using a feedback resistor from
               input to output, a MOS inverter can be made to act like an op-amp for
               small signals centered around the switching threshold.

               In 2008, Michael Huth facilitated closer investigation of the SID 6581
               filter circuit by publishing high quality microscope photographs of the die.
               Tommi Lempinen has done an impressive work on re-vectorizing and annotating
               the die photographs, substantially simplifying further analysis of the
               filter circuit.

               The filter schematics below are reverse engineered from these re-vectorized
               and annotated die photographs. While the filter first depicted in reSID 0.9
               is a correct model of the basic filter, the schematics are now completed
               with the audio mixer and output stage, including details on intended
               relative resistor values. Also included are schematics for the NMOS FET
               voltage controlled resistors (VCRs) used to control cutoff frequency, the
               DAC which controls the VCRs, the NMOS op-amps, and the output buffer.)pbdoc"
	       R"pbdoc(



               SID filter / mixer / output
               ---------------------------
               ~~~
                             +---------------------------------------------------+
                             |                                                   |
                             |                        +--1R1-- \--+ D7           |
                             |             +---R1--+  |           |              |
                             |             |       |  o--2R1-- \--o D6           |
                             |   +---------o--<A]--o--o           |     $17      |
                             |   |                    o--4R1-- \--o D5  1=open   | (3.5R1)
                             |   |                    |           |              |
                             |   |                    +--8R1-- \--o D4           | (7.0R1)
                             |   |                                |              |
               $17           |   |                    (CAP2B)     |  (CAP1B)     |
               0=to mixer    |   +--R8--+  +---R8--+      +---C---o      +---C---o
               1=to filter   |          |  |       |      |       |      |       |
                              ------R8--o--o--[A>--o--Rw--o--[A>--o--Rw--o--[A>--o
                   ve (EXT IN)          |          |              |              |
               D3  \ ---------------R8--o          |              | (CAP2A)      | (CAP1A)
                   |   v3               |          | vhp          | vbp          | vlp
               D2  |   \ -----------R8--o    +-----+              |              |
                   |   |   v2           |    |                    |              |
               D1  |   |   \ -------R8--o    |   +----------------+              |
                   |   |   |   v1       |    |   |                               |
               D0  |   |   |   \ ---R8--+    |   |    ---------------------------+
                   |   |   |   |             |   |   |
                   R6  R6  R6  R6            R6  R6  R6
                   |   |   |   | $18         |   |   |  $18
                   |    \  |   | D7: 1=open   \   \   \ D6 - D4: 0=open
                   |   |   |   |             |   |   |
                   +---o---o---o-------------o---o---+                         12V
                               |
                               |               D3 +--/ --1R2--+                 |
                               |   +---R8--+      |           |  +---R2--+      |
                               |   |       |   D2 o--/ --2R2--o  |       |  ||--+
                               +---o--[A>--o------o           o--o--[A>--o--||
                                               D1 o--/ --4R2--o (4.25R2)    ||--+
                                      $18         |           |                 |
                                      0=open   D0 +--/ --8R2--+ (8.75R2)        |

                                                                                vo (AUDIO
                                                                                    OUT)


               v1  - voice 1
               v2  - voice 2
               v3  - voice 3
               ve  - ext in
               vhp - highpass output
               vbp - bandpass output
               vlp - lowpass output
               vo  - audio out
               [A> - single ended inverting op-amp (self-biased NMOS inverter)
               Rn  - "resistors", implemented with custom NMOS FETs
               Rw  - cutoff frequency resistor (VCR)
               C   - capacitor
               ~~~
               Notes:

                   R2  ~  2.0*R1
                   R6  ~  6.0*R1
                   R8  ~  8.0*R1
                   R24 ~ 24.0*R1

               The Rn "resistors" in the circuit are implemented with custom NMOS FETs,
               probably because of space constraints on the SID die. The silicon substrate
               is laid out in a narrow strip or "snake", with a strip length proportional
               to the intended resistance. The polysilicon gate electrode covers the entire
               silicon substrate and is fixed at 12V in order for the NMOS FET to operate
               in triode mode (a.k.a. linear mode or ohmic mode).

               Even in "linear mode", an NMOS FET is only an approximation of a resistor,
               as the apparant resistance increases with increasing drain-to-source
               voltage. If the drain-to-source voltage should approach the gate voltage
               of 12V, the NMOS FET will enter saturation mode (a.k.a. active mode), and
               the NMOS FET will not operate anywhere like a resistor.)pbdoc"
	       R"pbdoc(



               NMOS FET voltage controlled resistor (VCR)
               ------------------------------------------
               ~~~
                              Vw

                              |
                              |
                              R1
                              |
                       +--R1--o
                       |    __|__
                       |    -----
                       |    |   |
               vi -----o----+   +--o----- vo
                       |           |
                       +----R24----+


               vi  - input
               vo  - output
               Rn  - "resistors", implemented with custom NMOS FETs
               Vw  - voltage from 11-bit DAC (frequency cutoff control)
               ~~~
               Notes:

               An approximate value for R24 can be found by using the formula for the
               filter cutoff frequency:

                   FCmin = 1/(2*pi*Rmax*C)

               Assuming that a the setting for minimum cutoff frequency in combination with
               a low level input signal ensures that only negligible current will flow
               through the transistor in the schematics above, values for FCmin and C can
               be substituted in this formula to find Rmax.
               Using C = 470pF and FCmin = 220Hz (measured value), we get:

                   FCmin = 1/(2*pi*Rmax*C)
                   Rmax = 1/(2*pi*FCmin*C) = 1/(2*pi*220*470e-12) ~ 1.5MOhm

               From this it follows that:
                   R24 =  Rmax   ~ 1.5MOhm
                   R1  ~  R24/24 ~  64kOhm
                   R2  ~  2.0*R1 ~ 128kOhm
                   R6  ~  6.0*R1 ~ 384kOhm
                   R8  ~  8.0*R1 ~ 512kOhm

               Note that these are only approximate values for one particular SID chip,
               due to process variations the values can be substantially different in
               other chips.)pbdoc"
	       R"pbdoc(



               Filter frequency cutoff DAC
               ---------------------------

               ~~~
                  12V  10   9   8   7   6   5   4   3   2   1   0   VGND
                    |   |   |   |   |   |   |   |   |   |   |   |     |   Missing
                   2R  2R  2R  2R  2R  2R  2R  2R  2R  2R  2R  2R    2R   termination
                    |   |   |   |   |   |   |   |   |   |   |   |     |
               Vw --o-R-o-R-o-R-o-R-o-R-o-R-o-R-o-R-o-R-o-R-o-R-o-   -+


               Bit on:  12V
               Bit off:  5V (VGND)
               ~~~
               As is the case with all MOS 6581 DACs, the termination to (virtual) ground
               at bit 0 is missing.

               Furthermore, the control of the two VCRs imposes a load on the DAC output
               which varies with the input signals to the VCRs. This can be seen from the
               VCR figure above.)pbdoc"
	       R"pbdoc(



               "Op-amp" (self-biased NMOS inverter)
               ------------------------------------
               ~~~

                                      12V

                                       |
                           +-----------o
                           |           |
                           |    +------o
                           |    |      |
                           |    |  ||--+
                           |    +--||
                           |       ||--+
                       ||--+           |
               vi -----||              o---o----- vo
                       ||--+           |   |
                           |       ||--+   |
                           |-------||      |
                           |       ||--+   |
                       ||--+           |   |
                    +--||              |   |
                    |  ||--+           |   |
                    |      |           |   |
                    |      +-----------o   |
                    |                  |   |
                    |                      |
                    |                 GND  |
                    |                      |
                    +----------------------+


               vi  - input
               vo  - output
               ~~~
               Notes:

               The schematics above are laid out to show that the "op-amp" logically
               consists of two building blocks; a saturated load NMOS inverter (on the
               right hand side of the schematics) with a buffer / bias input stage
               consisting of a variable saturated load NMOS inverter (on the left hand
               side of the schematics).

               Provided a reasonably high input impedance and a reasonably low output
               impedance, the "op-amp" can be modeled as a voltage transfer function
               mapping input voltage to output voltage.)pbdoc"
	       R"pbdoc(



               Output buffer (NMOS voltage follower)
               -------------------------------------
               ~~~

                          12V

                           |
                           |
                       ||--
               vi -----||
                       ||--
                           |
                           |------ vo
                           |     (AUDIO
                          Rext    OUT)
                           |
                           |

                          GND

               vi   - input
               vo   - output
               Rext - external resistor, 1kOhm
               ~~~
               Notes:

               The external resistor Rext is needed to complete the NMOS voltage follower,
               this resistor has a recommended value of 1kOhm.

               Die photographs show that actually, two NMOS transistors are used in the
               voltage follower. However the two transistors are coupled in parallel (all
               terminals are pairwise common), which implies that we can model the two
               transistors as one.

               Filter for 8580 chip
               --------------------
               The 8580 filter stage had been redesigned to be more linear and robust
               against temperature change. It also features real op-amps and a
               revisited resonance model.
               The filter schematics below are reverse engineered from re-vectorized
               and annotated die photographs. Credits to Michael Huth for the microscope
               photographs of the die, Tommi Lempinen for re-vectorizating and annotating
               the images and ttlworks from forum.6502.org for the circuit analysis.

               ~~~

                             +---------------------------------------------------+
                             |    $17      +----Rf-+                             |
                             |             |       |                             |
                             |      D4&!D5 o- \-R3-o                             |
                             |             |       |                    $17      |
                             |     !D4&!D5 o- \-R2-o                             |
                             |             |       |  +---R8-- \--+  !D6&D7      |
                             |      D4&!D5 o- \-R1-o  |           |              |
                             |             |       |  o---RC-- \--o   D6&D7      |
                             |   +---------o--<A]--o--o           |              |
                             |   |                    o---R4-- \--o  D6&!D7      |
                             |   |                    |           |              |
                             |   |                    +---Ri-- \--o !D6&!D7      |
                             |   |                                |              |
               $17           |   |                    (CAP2B)     |  (CAP1B)     |
               0=to mixer    |   +--R8--+  +---R8--+      +---C---o      +---C---o
               1=to filter   |          |  |       |      |       |      |       |
                             +------R8--o--o--[A>--o--Rfc-o--[A>--o--Rfc-o--[A>--o
                   ve (EXT IN)          |          |              |              |
               D3  \ ---------------R8--o          |              | (CAP2A)      | (CAP1A)
                   |   v3               |          | vhp          | vbp          | vlp
               D2  |   \ -----------R8--o    +-----+              |              |
                   |   |   v2           |    |                    |              |
               D1  |   |   \ -------R8--o    |   +----------------+              |
                   |   |   |   v1       |    |   |                               |
               D0  |   |   |   \ ---R8--+    |   |   +---------------------------+
                   |   |   |   |             |   |   |
                   R6  R6  R6  R6            R6  R6  R6
                   |   |   |   | $18         |   |   |  $18
                   |    \  |   | D7: 1=open   \   \   \ D6 - D4: 0=open
                   |   |   |   |             |   |   |
                   +---o---o---o-------------o---o---+
                               |
                               |               D3 +--/ --1R2--+
                               |   +---R8--+      |           |  +---R2--+
                               |   |       |   D2 o--/ --2R2--o  |       |
                               +---o--[A>--o------o           o--o--[A>--o-- vo (AUDIO OUT)
                                               D1 o--/ --4R2--o (4.25R2)
                                      $18         |           |
                                      0=open   D0 +--/ --8R2--+ (8.75R2))pbdoc"
	       R"pbdoc(



               Resonance
               ---------
               For resonance, we have two tiny DACs that controls both the input
               and feedback resistances.

               The "resistors" are switched in as follows by bits in register $17:

               feedback:
               R1: bit4&!bit5
               R2: !bit4&bit5
               R3: bit4&bit5
               Rf: always on

               input:
               R4: bit6&!bit7
               R8: !bit6&bit7
               RC: bit6&bit7
               Ri: !(R4|R8|RC) = !(bit6|bit7) = !bit6&!bit7


               The relative "resistor" values are approximately (using channel length):

               R1 = 15.3*Ri
               R2 =  7.3*Ri
               R3 =  4.7*Ri
               Rf =  1.4*Ri
               R4 =  1.4*Ri
               R8 =  2.0*Ri
               RC =  2.8*Ri


               Approximate values for 1/Q can now be found as follows (assuming an
               ideal op-amp):

               res  feedback  input  -gain (1/Q)
               ---  --------  -----  ----------
                0   Rf        Ri     Rf/Ri      = 1/(Ri*(1/Rf))      = 1/0.71
                1   Rf|R1     Ri     (Rf|R1)/Ri = 1/(Ri*(1/Rf+1/R1)) = 1/0.78
                2   Rf|R2     Ri     (Rf|R2)/Ri = 1/(Ri*(1/Rf+1/R2)) = 1/0.85
                3   Rf|R3     Ri     (Rf|R3)/Ri = 1/(Ri*(1/Rf+1/R3)) = 1/0.92
                4   Rf        R4     Rf/R4      = 1/(R4*(1/Rf))      = 1/1.00
                5   Rf|R1     R4     (Rf|R1)/R4 = 1/(R4*(1/Rf+1/R1)) = 1/1.10
                6   Rf|R2     R4     (Rf|R2)/R4 = 1/(R4*(1/Rf+1/R2)) = 1/1.20
                7   Rf|R3     R4     (Rf|R3)/R4 = 1/(R4*(1/Rf+1/R3)) = 1/1.30
                8   Rf        R8     Rf/R8      = 1/(R8*(1/Rf))      = 1/1.43
                9   Rf|R1     R8     (Rf|R1)/R8 = 1/(R8*(1/Rf+1/R1)) = 1/1.56
                A   Rf|R2     R8     (Rf|R2)/R8 = 1/(R8*(1/Rf+1/R2)) = 1/1.70
                B   Rf|R3     R8     (Rf|R3)/R8 = 1/(R8*(1/Rf+1/R3)) = 1/1.86
                C   Rf        RC     Rf/RC      = 1/(RC*(1/Rf))      = 1/2.00
                D   Rf|R1     RC     (Rf|R1)/RC = 1/(RC*(1/Rf+1/R1)) = 1/2.18
                E   Rf|R2     RC     (Rf|R2)/RC = 1/(RC*(1/Rf+1/R2)) = 1/2.38
                F   Rf|R3     RC     (Rf|R3)/RC = 1/(RC*(1/Rf+1/R3)) = 1/2.60


               These data indicate that the following function for 1/Q has been
               modeled in the MOS 8580:

                  1/Q = 2^(1/2)*2^(-x/8) = 2^(1/2 - x/8) = 2^((4 - x)/8))pbdoc"
	       R"pbdoc(



               Op-amps
               -------
               Unlike the 6581, the 8580 has real OpAmps.

               Temperature compensated differential amplifier:

                              9V

                              |
                    +-------o-o-o-------+
                    |       |   |       |
                    |       R   R       |
                    +--||   |   |   ||--+
                       ||---o   o---||
                    +--||   |   |   ||--+
                    |       |   |       |
                    o-----+ |   |       o--- Va
                    |     | |   |       |
                    +--|| | |   |   ||--+
                       ||-o-+---+---||
                    +--||   |   |   ||--+
                    |       |   |       |
                            |   |
                   GND      |   |      GND
                        ||--+   +--||
               in- -----||         ||------ in+
                        ||----o----||
                              |
                              8 Current sink
                              |

                             GND

               Inverter + non-inverting output amplifier:

               Va ---o---||-------------------o--------------------+
                     |                        |               9V   |
                     |             +----------+----------+     |   |
                     |        9V   |          |     9V   | ||--+   |
                     |         |   |      9V  |      |   +-||      |
                     |         R   |       |  |  ||--+     ||--+   |
                     |         |   |   ||--+  +--||            o---o--- Vout
                     |         o---o---||        ||--+     ||--+
                     |         |       ||--+         o-----||
                     |     ||--+           |     ||--+     ||--+
                     +-----||              o-----||            |
                           ||--+           |     ||--+
                               |           R         |        GND
                                           |
                              GND                   GND
                                          GND)pbdoc"
	       R"pbdoc(



               Virtual ground
               --------------
               A PolySi resitive voltage divider provides the voltage
               for the positive input of the filter op-amps.

                   5V
                        +----------+
                    |   |   |\     |
                    R1  +---|-\    |
               5V   |       |A >---o--- Vref
                    o-------|+/
                |   |       |/
               R10  R4
                |   |
                o---+
                |
               R10
                |

               GND

               Rn = n*R1



               Rfc - freq control DAC resistance ladder
               ----------------------------------------
               The 8580 has 11 bits for frequency control, but 12 bit DACs.
               If those 11 bits would be '0', the impedance of the DACs would be "infinitely high".
               To get around this, there is an 11 input NOR gate below the DACs sensing those 11 bits.
               If all are 0, the NOR gate gives the gate control voltage to the 12 bit DAC LSB.

                   ----------------------------
                       |   |       |   |   |
                     Rb10 Rb9 ... Rb1 Rb0  R0
                       |   |       |   |   |
                   ----------------------------



               Crystal stabilized precision swithced capacitor voltage divider
               ---------------------------------------------------------------
               There is a FET working as a temperature sensor close to the DACs which changes the gate voltage
               of the frequency control DACs according to the temperature of the DACs,
               to reduce the effects of temperature on the filter curve.
               An asynchronous 3 bit binary counter, running at the speed of PHI2, drives two big capacitors
               whose AC resistance is then used as a voltage divider.
               This implicates that frequency difference between PAL and NTSC might shift the filter curve by 4% or such.

                                              |\  OpAmp has a smaller capacitor than the other OPs
                                      Vref ---|+\
                                              |A >---o--- Vdac
                                      o-------|-/    |
                                      |       |/     |
                                      |              |
                     C1               |     C2       |
                 +---||---o---+   +---o-----||-------o
                 |        |   |   |   |              |
                 o----+   |   -----   |              |
                 |    |   |   -----   +----+   +-----+
                 |    -----     |          |   |     |
                 |    -----     |          -----     |
                 |      |       |          -----     |
                 |    +-----------+          |       |
                      | /Q      Q |          +-------+
                GND   +-----------+      FET close to DAC
                      |   clk/8   |      working as temperature sensor
                      +-----------+
            )pbdoc")

            .def(py::init<sid::ChipModel, sid::SamplingMethod, double, double>(), R"pbdoc(
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
                   chipModel (_pyresidfp.ChipModel):   Chip model to emulate
                   method (_pyresidfp.SamplingMethod): Sampling method to use
                   clockFrequency (float):             System clock frequency at Hz
                   samplingFrequency (float):          Desired output sampling rate

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

            .def("input", &::pysid::PythonSid::input, R"pbdoc(
               16-bit input (EXT IN). Write 16-bit sample to audio input. NB! The caller
               is responsible for keeping the value within 16 bits. Note that to mix in
               an external audio signal, the signal should be resampled to 1MHz first to\
               avoid sampling noise.

               Args:
                   value (int): Input level to set
            )pbdoc")

            .def("read", &::pysid::PythonSid::read, R"pbdoc(
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

            .def("write", &::pysid::PythonSid::write, R"pbdoc(
               Write registers.

               Args:
                   offset (int): Chip register to write to
                   value  (char): Value to write
            )pbdoc")

            .def("mute", &::pysid::PythonSid::mute, R"pbdoc(
               SID voice muting.

               Args:
                   channel (int): Channel to modify
                   enable (bool): enable muting
            )pbdoc")

            .def("clock", &::pysid::PythonSid::clock, R"pbdoc(
               Clock SID forward using chosen output sampling algorithm and sample.

               Note:
                   The number of samples is equal to

                   samples = samplingFrequency * cycles / clockFrequency

               Args:
                   cycles (int): Number of clock cycles to forward

               Returns:
                    :obj:`list` of :obj:`int` samples in range -32768 to 32767
            )pbdoc")

            .def("set_filter_6581_curve", &::pysid::PythonSid::setFilter6581Curve, R"pbdoc(
               Set filter curve parameter for 6581 model.

               Args:
                   curvePosition (float): 0 .. 1, where 0 sets center frequency high ("light") and 1 sets it low ("dark"), default is 0.5
            )pbdoc")

            .def("set_filter_8580_curve", &::pysid::PythonSid::setFilter8580Curve, R"pbdoc(
               Set filter curve parameter for 8580 model.

               Args:
                   curvePosition (float):
            )pbdoc")

            .def("enable_filter", &::pysid::PythonSid::enableFilter, R"pbdoc(
               Enable filter emulation.

               Args:
                   enable (bool): False to turn off filter emulation
            )pbdoc");
}
