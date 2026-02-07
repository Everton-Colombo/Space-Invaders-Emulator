cmake_minimum_required(VERSION 3.25)
project(space-invaders LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(raylib)

# Disassembler targets:
add_library(disassemble8080 src/disassembler/8080_disassembler.c src/disassembler/8080_disassembler.h)

add_executable(8080_disassembler src/disassembler/main.c)
target_link_libraries(8080_disassembler PUBLIC disassemble8080)
set_target_properties(8080_disassembler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Utils target
add_library(utils src/utils/utils.cpp src/utils/utils.hpp)

# CPU 8080 debugger target:
add_executable(cpu8080_debugger src/emu/cpu8080.cpp src/emu/cpu8080.hpp src/cpu8080_debugger.cpp)
target_link_libraries(cpu8080_debugger PUBLIC disassemble8080 utils)
target_include_directories(cpu8080_debugger PRIVATE src/emu src/disassembler src/utils)
set_target_properties(cpu8080_debugger PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Emulator target:
file(GLOB EMU_CPP_SOURCES "src/emu/*.cpp")
list(FILTER EMU_CPP_SOURCES EXCLUDE REGEX "src/cpu8080_debugger.cpp$")
file(GLOB EMU_CPP_HEADERS "src/emu/*.hpp")
add_executable(siemu src/main.cpp ${EMU_CPP_SOURCES} ${EMU_CPP_HEADERS})
target_link_libraries(siemu PRIVATE disassemble8080 utils)
target_link_libraries(siemu PRIVATE raylib)
target_include_directories(siemu PRIVATE src/emu src/disassembler src/utils)
set_target_properties(siemu PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)