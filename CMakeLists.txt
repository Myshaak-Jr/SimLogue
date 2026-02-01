cmake_minimum_required(VERSION 3.20)

project(SimLogue VERSION 0.2.2 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(SIMLOGUE_HIGH_PRECISION "Enable high precision math" ON)
option(SIMLOGUE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)


add_executable(simlogue
    src/main.cpp
    src/settings.cpp
    
    src/circuit/circuit.cpp
    src/circuit/scope.cpp
    src/circuit/util.cpp
    
    src/circuit/interpreter/interpreter.cpp
    
    src/circuit/parts/ac_voltage_source.cpp
    src/circuit/parts/capacitor.cpp
    src/circuit/parts/current_source.cpp
    src/circuit/parts/inductor.cpp
    src/circuit/parts/resistor.cpp
    src/circuit/parts/switch.cpp
    src/circuit/parts/voltage_source.cpp
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/version.h
    @ONLY
)

target_include_directories(simlogue PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/include
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

target_compile_features(simlogue PRIVATE cxx_std_23)


# ---- Definitions ----
if (SIMLOGUE_HIGH_PRECISION)
    target_compile_definitions(simlogue PRIVATE HIGH_PRECISION)
endif()

if(WIN32)
    target_compile_definitions(simlogue PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
endif()


# ---- Warnings / Compiler-specific flags ----
if(MSVC)
    target_compile_options(simlogue PRIVATE /W4 /permissive-)
    if(SIMLOGUE_WARNINGS_AS_ERRORS)
        target_compile_options(simlogue PRIVATE /WX)
    endif()
else()
    target_compile_options(simlogue PRIVATE -Wall -Wextra -Wpedantic)
    if(SIMLOGUE_WARNINGS_AS_ERRORS)
        target_compile_options(simlogue PRIVATE -Werror)
    endif()
endif()

# ---- Debug/Release behavior ----
target_compile_definitions(simlogue PRIVATE
    $<$<CONFIG:Debug>:SIMLOGUE_DEBUG=1>
    $<$<CONFIG:Release>:SIMLOGUE_RELEASE=1>
)