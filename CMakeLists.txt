cmake_minimum_required(VERSION 3.18)
project(pyresidfp VERSION 0.1.0 LANGUAGES CXX)

# Currently, Scikit-build does not support FindPython, so we convert the
# provided hints ourselves.
if(SKBUILD)
  message(STATUS "The project is built using scikit-build")
  set(Python_EXECUTABLE "${PYTHON_EXECUTABLE}")
  set(Python_INCLUDE_DIR "${PYTHON_INCLUDE_DIR}")
  set(DUMMY "${PYTHON_LIBRARY}")
  set(DUMMY "${PYTHON_VERSION_STRING}")  # Not needed, silences a warning
endif()

set(Python_FIND_IMPLEMENTATIONS CPython PyPy)  # PyPy requires 3.18
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

# Scikit-Build does not add your site-packages to the search path automatically,
# so we need to add it _or_ the pybind11 specific directory here.
execute_process(
  COMMAND
    "${Python_EXECUTABLE}" -c
    "import pybind11; print(pybind11.get_cmake_dir())"
  OUTPUT_VARIABLE _tmp_dir
  OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ECHO STDOUT)
list(APPEND CMAKE_PREFIX_PATH "${_tmp_dir}")

# Now we can find pybind11
find_package(pybind11 CONFIG REQUIRED)

include(CheckCXXSourceCompiles)
include(CheckIncludeFileCXX)

enable_testing()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(USE_CLANG TRUE)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(USE_GCC TRUE)
endif()

if(USE_GCC OR USE_CLANG)
  check_include_file_cxx(mmintrin.h HAVE_MMINTRIN_H)
else()
  set(HAVE_MMINTRIN_H FALSE)
endif()

check_cxx_source_compiles("
int main(void) { if (__builtin_expect(0, 0)) return 1; return 0; }
" HAVE_BUILTIN_EXPECT)

set(CMAKE_CXX_STANDARD 11)
set(PACKAGE_VERSION ${PROJECT_VERSION})
set(RESID_INLINE inline)
set(RESID_INLINING 1)
set(RESID_BRANCH_HINTS 1)
set(HAVE_CXX11 1)

configure_file(src/residfp/siddefs-fp.h.in siddefs-fp.h)
configure_file(src/config.h.in config.h)

set(HEADER_FILES
        ${CMAKE_CURRENT_BINARY_DIR}/siddefs-fp.h
        ${CMAKE_CURRENT_BINARY_DIR}/config.h
        src/residfp/resample/Resampler.h
        src/residfp/resample/SincResampler.h
        src/residfp/resample/TwoPassSincResampler.h
        src/residfp/resample/ZeroOrderResampler.h
        src/residfp/array.h
        src/residfp/Dac.h
        src/residfp/EnvelopeGenerator.h
        src/residfp/EnvelopeGenerator.h
        src/residfp/Filter.h
        src/residfp/Filter6581.h
        src/residfp/Filter8580.h
        src/residfp/FilterModelConfig.h
        src/residfp/FilterModelConfig8580.h
        src/residfp/Integrator.h
        src/residfp/Integrator8580.h
        src/residfp/OpAmp.h
        src/residfp/Potentiometer.h
        src/residfp/SID.h
        src/residfp/Spline.h
        src/residfp/Voice.h
        src/residfp/WaveformCalculator.h
        src/residfp/WaveformGenerator.h
        src/sidcxx11.h
        src/PythonSid.h)
set(SOURCE_FILES
        src/residfp/resample/SincResampler.cpp
        src/residfp/Dac.cpp
        src/residfp/EnvelopeGenerator.cpp
        src/residfp/ExternalFilter.cpp
        src/residfp/Filter.cpp
        src/residfp/Filter6581.cpp
        src/residfp/Filter8580.cpp
        src/residfp/FilterModelConfig.cpp
        src/residfp/FilterModelConfig8580.cpp
        src/residfp/Integrator.cpp
        src/residfp/Integrator8580.cpp
        src/residfp/OpAmp.cpp
        src/residfp/SID.cpp
        src/residfp/Spline.cpp
        src/residfp/version.cc
        src/residfp/WaveformCalculator.cpp
        src/residfp/WaveformGenerator.cpp
        src/pyresidfp.cpp
        src/PythonSid.cpp)

pybind11_add_module(_pyresidfp MODULE
        ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(_pyresidfp
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
        src/residfp src/residfp/resample)
target_compile_definitions(_pyresidfp PRIVATE HAVE_CONFIG_H)
target_compile_definitions(_pyresidfp PRIVATE PROJECT_VERSION)
set_property(TARGET _pyresidfp PROPERTY CXX_STANDARD_REQUIRED true)
set_property(TARGET _pyresidfp PROPERTY CXX_STANDARD 11)

install(TARGETS _pyresidfp DESTINATION .)
