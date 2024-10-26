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

#include "PythonSid.h"

namespace sid = reSIDfp;

namespace pyreSIDfp {

    PythonSid::PythonSid(const sid::ChipModel model, const sid::SamplingMethod method,
                                       const double clockFrequency, const double samplingFrequency) :
            delegate(new sid::SID()),
            chipModel(model),
            samplingMethod(method),
            clockFrequency(clockFrequency),
            samplingFrequency(samplingFrequency),
            isMuted() {
        if (clockFrequency < samplingFrequency) {
            throw sid::SIDError("Clock frequency below sampling frequency");
        }
        this->reset();
    }

    void PythonSid::reset() {
        delegate->reset();
        delegate->setChipModel(chipModel);
        delegate->setSamplingParameters(clockFrequency, samplingMethod, samplingFrequency);
    }

    sid::ChipModel PythonSid::getChipModel() const {
        return chipModel;
    }

    void PythonSid::setChipModel(const sid::ChipModel model) {
        this->chipModel = model;
        this->reset();
    }

    sid::SamplingMethod PythonSid::getSamplingMethod() const {
        return samplingMethod;
    }

    void PythonSid::setSamplingMethod(const sid::SamplingMethod method) {
        this->samplingMethod = method;
        this->reset();
    }

    double PythonSid::getClockFrequency() const {
        return this->clockFrequency;
    }

    void PythonSid::setClockFrequency(const double frequency) {
        if (frequency < this->samplingFrequency) {
            throw sid::SIDError("Clock frequency below sampling frequency");
        }
        this->clockFrequency = frequency;
        this->reset();
    }

    double PythonSid::getSamplingFrequency() const {
        return this->samplingFrequency;
    }

    void PythonSid::setSamplingFrequency(const double frequency) {
        if (frequency > this->clockFrequency) {
            throw sid::SIDError("Sampling frequency above clock frequency");
        }
        this->samplingFrequency = frequency;
        this->reset();
    }

    void PythonSid::input(const int value) {
        this->delegate->input(value);
    }

    const unsigned char PythonSid::read(const int offset) {
        return this->delegate->read(offset);
    }

    void PythonSid::write(const int offset, unsigned char value) {
        switch (offset) {
            case 0x04:
                if (this->isMuted[0]) value &= 0x0f;
                break;
            case 0x0b:
                if (this->isMuted[1]) value &= 0x0f;
                break;
            case 0x12:
                if (this->isMuted[2]) value &= 0x0f;
                break;
            case 0x18:
                if (this->isMuted[3]) value |= 0x0f;
                break;
        }

        this->delegate->write(offset, value);
    }

    void PythonSid::mute(const int channel, const bool enable) {
        if (channel < 4) {
            this->isMuted[channel] = enable;
        }
    }

    std::vector<short> PythonSid::clock(const unsigned int cycles) {
        // over-allocate buffer (sampling frequency is always below clock frequency)
        std::vector<short> result(cycles);
        int realSamples = this->delegate->clock(cycles, result.data());
        result.resize(static_cast<std::size_t>(realSamples));
        return result;
    }

    void PythonSid::setFilter6581Curve(const double filterCurve) {
        this->delegate->setFilter6581Curve(filterCurve);
    }

    void PythonSid::setFilter8580Curve(const double filterCurve) {
        this->delegate->setFilter8580Curve(filterCurve);
    }

    void PythonSid::enableFilter(const bool enable) {
        this->delegate->enableFilter(enable);
    }
} // namespace pyreSIDfp
