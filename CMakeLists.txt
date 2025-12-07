cmake_minimum_required(VERSION 3.14)
project(space-invaders LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Disassembler target:
add_executable(8080_disassembler src/8080_disassembler.c)
set_target_properties(8080_disassembler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Emulator target:
file(GLOB EMU_CPP_SOURCES "src/emu/*.cpp")
file(GLOB EMU_CPP_HEADERS "src/emu/*.hpp")
add_executable(siemu ${EMU_CPP_SOURCES} ${EMU_CPP_HEADERS})

set_target_properties(siemu PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)