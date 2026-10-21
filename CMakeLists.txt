cmake_minimum_required(VERSION 3.20)

project(CHIP8Emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Keep single-configuration generators (Makefiles/Ninja) consistent with the
# Release output path used by the run scripts.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ compiler ID: ${CMAKE_CXX_COMPILER_ID}")

# Discover every .cpp under src/. CONFIGURE_DEPENDS makes CMake reconfigure
# when source files are added or removed.
file(GLOB_RECURSE CHIP8_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

if(NOT CHIP8_SOURCES)
    message(FATAL_ERROR "No .cpp source files were found under src/.")
endif()

add_executable(chip8 ${CHIP8_SOURCES})

# The project currently uses both project-root-relative includes such as
# ./include/interface.hpp and direct includes such as renderer.hpp.
# Keep both include roots available; source files can later be standardized to
# #include "interface.hpp" and only the include/ directory will be needed.
target_include_directories(chip8 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Put the executable at build/bin/Release on all supported generators.
set_target_properties(chip8 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
)

# Prefer SDL2's CMake package targets when available.
find_package(SDL2 QUIET CONFIG)

if(TARGET SDL2::SDL2)
    # SDL2main must precede SDL2 in the link order when present.
    if(TARGET SDL2::SDL2main)
        target_link_libraries(chip8 PRIVATE SDL2::SDL2main)
    endif()

    target_link_libraries(chip8 PRIVATE SDL2::SDL2)

    # When SDL2 is a shared imported target on Windows, copy its DLL next to
    # chip8.exe so the program can start without manually locating that DLL.
    if(WIN32)
        get_target_property(CHIP8_SDL2_TYPE SDL2::SDL2 TYPE)
        if(CHIP8_SDL2_TYPE STREQUAL "SHARED_LIBRARY")
            add_custom_command(TARGET chip8 POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "$<TARGET_FILE:SDL2::SDL2>"
                    "$<TARGET_FILE_DIR:chip8>"
                VERBATIM
            )
        endif()
    endif()

elseif(TARGET SDL2::SDL2-static)
    if(TARGET SDL2::SDL2main)
        target_link_libraries(chip8 PRIVATE SDL2::SDL2main)
    endif()
    target_link_libraries(chip8 PRIVATE SDL2::SDL2-static)

elseif(SDL2_FOUND AND SDL2_LIBRARIES)
    # Compatibility path for older SDL2 CMake packages that expose variables.
    if(SDL2_INCLUDE_DIRS)
        target_include_directories(chip8 PRIVATE ${SDL2_INCLUDE_DIRS})
    elseif(SDL2_INCLUDE_DIR)
        target_include_directories(chip8 PRIVATE ${SDL2_INCLUDE_DIR})
    endif()
    target_link_libraries(chip8 PRIVATE ${SDL2_LIBRARIES})

else()
    # Fallback used by common Linux distributions and package-manager installs.
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL2_PC QUIET IMPORTED_TARGET sdl2)
    endif()

    if(TARGET PkgConfig::SDL2_PC)
        target_link_libraries(chip8 PRIVATE PkgConfig::SDL2_PC)
    else()
        message(FATAL_ERROR
            "SDL2 development files were not found. Install SDL2 development "
            "files for your OS, or set CMAKE_PREFIX_PATH to the SDL2 installation."
        )
    endif()
endif()

if(MSVC)
    target_compile_options(chip8 PRIVATE /W4)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(chip8 PRIVATE -Wall -Wextra -Wpedantic)
endif()
