# pyresidfp

Emulates the SID sound-chip in software. The C++ emulation code was copied over from
[libsidplayfp](https://github.com/libsidplayfp/libsidplayfp).

## How to install

Requirements for installation from source:
- compiler for ISO C++20
- Python 3.10+ and header files


### From PyPI

Install the latest version using
```commandline
python -m pip install pyresidfp
```


### From cloned git repository 

Build from source and install using
```commandline
python -m pip install .
```

## Example

For the example, [NumPy](http://www.numpy.org/) and [soundcard](https://github.com/bastibe/SoundCard) python packages
are required. The example is ported from the section *Sample Sound Program*,
[Commodore 64 User's Guide](https://archive.org/embed/Commodore_64_Users_Guide_1982_Commodore), page 80:
```python
from datetime import timedelta
import numpy as np
import soundcard as sc
from pyresidfp import SoundInterfaceDevice, Voice, ControlBits, Tone

# program SID
sid = SoundInterfaceDevice()
sid.Filter_Mode_Vol = 15             # Maximum volume
sid.attack_decay(Voice.ONE, 190)     # 800 ms attack, 15 s decay
sid.sustain_release(Voice.ONE, 248)  # sustain peak, 300 ms release
sid.tone(Voice.ONE, Tone.C4)
sid.control(Voice.ONE, ControlBits.TRIANGLE | ControlBits.GATE)

# sample attack phase
attack_phase = timedelta(seconds=0.32)
raw_samples = sid.clock(attack_phase)

# reprogram SID for release phase and sample
release_phase = timedelta(seconds=0.3)
sid.control(Voice.ONE, ControlBits.TRIANGLE)
raw_samples.extend(sid.clock(release_phase))

# convert audio format and play
samples = np.array(raw_samples, dtype=np.float32) / 2.0**15
spkr = sc.default_speaker()
spkr.play(data=samples, samplerate=int(sid.sampling_frequency), channels=1)
```


## Credits

We like to thank all contributors of `libsidplayfp`, especially:

- Dag Lem: Designed and programmed complete emulation engine.
- Antti S. Lankila: Distortion simulation and calculation of combined waveforms
- Ken Händel: source code conversion to Java
- Leandro Nini: port to c++, merge with reSID 1.0
