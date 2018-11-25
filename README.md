# pyresidfp

Emulates the SID sound-chip in software. The C++ emulation code was copied over from
[libsidplayfp](https://sourceforge.net/projects/sidplay-residfp/).


## How to Build

Clone repository with `--recursive` flag to also clone submodule
[pybind11](https://github.com/pybind/pybind11). Build from source using
```commandline
python setup.py bdist_egg
```
and install using
```commandline
pip install dist/pyresidfp-*.whl
```
Requirements:
- cmake 3.12+
- compiler for ISO C++11
- Python 3 and libpython include files


## Thanks

I like to thank all contributors of `libsidplayfp`, especially:

- Dag Lem: Designed and programmed complete emulation engine.
- Antti S. Lankila: Distortion simulation and calculation of combined waveforms
- Ken Händel: source code conversion to Java
- Leandro Nini: port to c++, merge with reSID 1.0
