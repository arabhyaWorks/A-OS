cmake_minimum_required(VERSION 3.15)
project(A-OS VERSION 0.1.0 LANGUAGES CXX)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find SDL2 (required for rendering and input)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Find SDL2_ttf (required for text rendering)
# Note: On some systems, this might be found differently
# For vcpkg: should work automatically
# For apt: sudo apt-get install libsdl2-ttf-dev
find_package(SDL2_ttf QUIET)
if(NOT SDL2_ttf_FOUND)
    # Try pkg-config as fallback
    find_package(PkgConfig)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL2_TTF QUIET SDL2_ttf)
        if(SDL2_TTF_FOUND)
            include_directories(${SDL2_TTF_INCLUDE_DIRS})
            link_directories(${SDL2_TTF_LIBRARY_DIRS})
        endif()
    endif()
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/os
    ${CMAKE_SOURCE_DIR}/src/hal
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/apps
)

# Source files
set(AOS_SOURCES
    src/main.cpp
    src/os/event_bus.cpp
    src/os/app_manager.cpp
    src/os/os_core.cpp
    src/hal/input_manager.cpp
    src/hal/audio_manager.cpp
    src/ui/renderer.cpp
    src/apps/home_app.cpp
    src/apps/settings_app.cpp
)

# Executable
add_executable(aos ${AOS_SOURCES})

# Link SDL2
target_link_libraries(aos ${SDL2_LIBRARIES})

# Link SDL2_ttf
if(SDL2_ttf_FOUND)
    target_link_libraries(aos SDL2_ttf::SDL2_ttf)
elseif(SDL2_TTF_FOUND)
    target_link_libraries(aos ${SDL2_TTF_LIBRARIES})
else()
    # Try to link directly (last resort)
    target_link_libraries(aos SDL2_ttf)
endif()

# Platform-specific linking
if(UNIX AND NOT APPLE)
    # Linux/Raspberry Pi: might need additional libraries
    target_link_libraries(aos pthread dl)
endif()

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# Installation (for deployment to Raspberry Pi)
install(TARGETS aos DESTINATION /usr/local/bin)
install(DIRECTORY assets DESTINATION /usr/local/share/aos)
