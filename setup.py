#  This file is part of pyresidfp, a SID emulation package for Python.
#
#  Copyright (c) 2018-2022.  Sebastian Klemke <packet@nerdheim.de>
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

import pybind11

from skbuild import setup


with open('README.md', 'r') as fh:
    long_description = fh.read()

pybind_dir = pybind11.get_cmake_dir()

setup(
    name='pyresidfp',
    author='Sebastian Klemke',
    author_email='pypi@nerdheim.de',
    description='Emulates the SID sound-chip',
    long_description=long_description,
    long_description_content_type='text/markdown',
    url='https://github.com/pyresidfp/pyresidfp',
    classifiers=[
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3 :: Only',
        'Programming Language :: C++',
        'License :: OSI Approved :: GNU General Public License v2 or later (GPLv2+)',
        'Operating System :: OS Independent',
        'Topic :: Multimedia :: Sound/Audio :: Sound Synthesis',
        'Topic :: System :: Emulators',
        'Intended Audience :: Developers',
        'Development Status :: 2 - Pre-Alpha'
    ],
    packages=["pyresidfp"],
    package_dir={"": "src"},
    cmake_install_dir="src/pyresidfp",
    cmake_args=['-DCMAKE_PREFIX_PATH=' + pybind_dir] if pybind_dir else None,
    python_requires='>=3.7.0,<4.0.0',
)
