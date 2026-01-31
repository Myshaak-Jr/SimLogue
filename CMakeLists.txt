cmake_minimum_required(VERSION 3.20)

project(SimLogue LANGUAGES CXX)

add_executable(simlogue
    circuits/src/circuit/circuit.cpp
    circuits/src/circuit/interpreter/interpreter.cpp
    circuits/src/circuit/parts/ac_voltage_source.cpp
    circuits/src/circuit/parts/capacitor.cpp
    circuits/src/circuit/parts/current_source.cpp
    circuits/src/circuit/parts/inductor.cpp
    circuits/src/circuit/parts/resistor.cpp
    circuits/src/circuit/parts/switch.cpp
    circuits/src/circuit/parts/voltage_source.cpp
    circuits/src/circuit/scope.cpp
    circuits/src/circuit/util.cpp
    circuits/src/main.cpp
    circuits/src/settings.cpp
)

target_include_directories(simlogue PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/circuits/src
    ${CMAKE_CURRENT_SOURCE_DIR}/circuits/libs/include
)

target_compile_features(simlogue PRIVATE cxx_std_23)
target_compile_definitions(simlogue PRIVATE HIGH_PRECISION)
