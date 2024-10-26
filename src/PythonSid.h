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


#ifndef PYRESIDFP_PYTHONSIDADAPTOR_H
#define PYRESIDFP_PYTHONSIDADAPTOR_H


#include <vector>
#include <bitset>

#include "SID.h"

namespace pyreSIDfp {
    class PythonSid {
    private:
        std::unique_ptr <reSIDfp::SID> delegate;
        reSIDfp::ChipModel chipModel;
        reSIDfp::SamplingMethod samplingMethod;
        double clockFrequency;
        double samplingFrequency;
        std::bitset<4> isMuted;

    public:
        PythonSid(reSIDfp::ChipModel model, reSIDfp::SamplingMethod method,
                         double clockFrequency, double samplingFrequency);

        void reset();

        reSIDfp::ChipModel getChipModel() const;

        void setChipModel(reSIDfp::ChipModel model);

        reSIDfp::SamplingMethod getSamplingMethod() const;

        void setSamplingMethod(reSIDfp::SamplingMethod method);

        double getClockFrequency() const;

        void setClockFrequency(double frequency);

        double getSamplingFrequency() const;

        void setSamplingFrequency(double frequency);

        void input(int value);

        const unsigned char read(int offset);

        void write(int offset, unsigned char value);

        void mute(int channel, bool enable);

        std::vector<short> clock(unsigned int cycles);

        void setFilter6581Curve(double filterCurve);

        void setFilter8580Curve(double filterCurve);

        void enableFilter(bool enable);
    };
} // namespace pyreSIDfp

#endif //PYRESIDFP_PYTHONSIDADAPTOR_H
