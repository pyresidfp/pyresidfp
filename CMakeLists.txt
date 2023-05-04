cmake_minimum_required(VERSION 3.15...3.26)

string(REGEX REPLACE "^([0-9]+\.[0-9]+(\.[0-9]+)?)(\.[^.]*)*$" "\\1" TRUNCATED_PROJECT_VERSION ${SKBUILD_PROJECT_VERSION})

project(${SKBUILD_PROJECT_NAME} VERSION ${TRUNCATED_PROJECT_VERSION} LANGUAGES CXX)

set(PYBIND11_NEWPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

include(CheckCXXSourceCompiles)
include(CheckIncludeFileCXX)
include(CheckIPOSupported)

enable_testing()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(USE_CLANG TRUE)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(USE_GCC TRUE)
endif()

if(USE_GCC OR USE_CLANG)
  check_include_file_cxx(emmintrin.h HAVE_EMMINTRIN_H)
  check_include_file_cxx(mmintrin.h HAVE_MMINTRIN_H)
  check_include_file_cxx(arm_neon.h HAVE_ARM_NEON_H)
else()
  set(HAVE_EMMINTRIN_H FALSE)
  set(HAVE_MMINTRIN_H FALSE)
  set(HAVE_ARM_NEON_H FALSE)
endif()

check_cxx_source_compiles("
int main(void) { if (__builtin_expect(0, 0)) return 1; return 0; }
" HAVE_BUILTIN_EXPECT)

check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_ERROR)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
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
        src/residfp/FilterModelConfig6581.h
        src/residfp/FilterModelConfig8580.h
        src/residfp/Integrator6581.h
        src/residfp/Integrator8580.h
        src/residfp/OpAmp.h
        src/residfp/Potentiometer.h
        src/residfp/SID.h
        src/residfp/Spline.h
        src/residfp/Voice.h
        src/residfp/WaveformCalculator.h
        src/residfp/WaveformGenerator.h
        src/sidcxx11.h
        src/sidcxx14.h
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
        src/residfp/FilterModelConfig6581.cpp
        src/residfp/FilterModelConfig8580.cpp
        src/residfp/Integrator6581.cpp
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
set_property(TARGET _pyresidfp PROPERTY CXX_STANDARD 14)

if (IPO_SUPPORTED)
  message(STATUS "IPO / LTO enabled")
  set_property(TARGET _pyresidfp PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(STATUS "IPO / LTO not supported: <${IPO_ERROR}>")
endif()

install(TARGETS _pyresidfp DESTINATION .)
