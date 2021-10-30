cmake_minimum_required(VERSION 3.0)

project(ShivaVG
    VERSION 0.2.1
    LANGUAGES C
    DESCRIPTION "The ShivaVG implementation of OpenVG"
)

# TODO test on Windows/MSVC
# TODO test on Windows/MinGW
# TODO separate targets for examples and install target
# TODO update INSTALL/README etc

include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckTypeSize)

# make cache variables for install destinations
include(GNUInstallDirs)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
  # using Clang
  set(CMAKE_C_FLAGS "-O3 -std=c11 -Wincompatible-pointer-types -Wstrict-prototypes -Wall -Wvla")
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
  # using GCC
set(CMAKE_C_FLAGS "-O3 -std=c11 -pedantic -Werror=incompatible-pointer-types -Werror=strict-prototypes -fno-strict-aliasing -Wall -Wvla")
endif()


# Export json compilation database command, usefull for clang-check
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set GLVND as prefered OpenGL policy
set(OpenGL_GL_PREFERENCE "GLVND")


# Set the library name
set(SHARED_LIBRARY_NAME "OpenVG" CACHE STRING "Shared library name")
set(STATIC_LIBRARY_NAME "OpenVGStatic" CACHE STRING "Static library name")

# Processing the generator options
# TODO: can we have a master option that will turn others in UI?
# TODO: nicer display in UI? http://www.kitware.com/blog/home/post/82
option(BUILD_DEBUG             "Build with debug (default=no)"                             OFF)
option(BUILD_ALL_EXAMPLES      "Initialize build values for all examples (default=no)]"    OFF)
option(BUILD_VGU               "Build VGU example (default=no)]"                           OFF)
option(BUILD_DASH              "Build Dashing example (default=no)]"                       OFF)
option(BUILD_LINEAR            "Build Linear Gradient example (default=no)]"               OFF)
option(BUILD_RADIAL            "Build Radial Gradient example (default=no)]"               OFF)
option(BUILD_GRADIENT          "Build Gradient example (default=no)]"                      OFF)
option(BUILD_INTERPOLATE       "Build Path Interpolation example (default=no)]"            OFF)
option(BUILD_TIGER             "Build Tiger SVG example (default=no)]"                     OFF)
option(BUILD_IMAGE             "Build Images example (default=no)]"                        OFF)
option(BUILD_PATTERN           "Build Pattern Paint example (default=no)]"                 OFF)
option(BUILD_BLEND             "Build Blending example (default=no)]"                      OFF)
option(BUILD_PORTERDUFF        "Build Porter Duff example (default=no)]"                   OFF)
option(BUILD_EB                "Build Electro Body example (default=no)]"                  OFF)
option(BUILD_WARP              "Build Warp image example (default=no)]"                    OFF)
option(BUILD_PATH              "Build Path image example (default=no)]"                    OFF)
option(BUILD_FLOWERS           "Build Flowers example (default=no)]"                       OFF)
option(BUILD_STRESS            "Build Stress test example (default=no)]"                   OFF)
option(BUILD_CLOCK             "Build Clock example (default=no)]"                         OFF)
option(BUILD_SUBWAYMAP         "Build Subwaymap example (default=no)]"                     OFF)
option(BUILD_COMPOSITING       "Build Compositing example (default=no)]"                   OFF)
option(BUILD_COLORMATRIX       "Build Color matrix example (default=no)]"                  OFF)
option(BUILD_GAUSSIANBLUR      "Build Color gaussian blur example (default=no)]"           OFF)
option(BUILD_GAUSSIANBLUR2     "Build Color gaussian blur advanced example (default=no)]"  OFF)
option(BUILD_CONVOLVE          "Build Convolve example (default=no)]"                      OFF)
option(BUILD_SEPARABLECONVOLVE "Build Separable Convolve example (default=no)]"            OFF)
option(BUILD_LOOKUP            "Build Color lookup example (default=no)]"                  OFF)
option(BUILD_LOOKUP2           "Build Color lookup2 example (default=no)]"                 OFF)
option(BUILD_LOOKUP_SINGLE     "Build Color lookup single example (default=no)]"           OFF)
option(BUILD_COLOR             "Build Simple color matrix example (default=no)]"           OFF)
option(BUILD_SRGB              "Build sRGB example (default=no)]"                          OFF)
option(BUILD_IMAGE_MODE        "Build Image mode example (default=no)]"                    OFF)
option(BUILD_TILING_MODE       "Build Tiling mode example (default=no)]"                   OFF)

if (BUILD_DEBUG)
  set(DEBUG ON CACHE BOOL "Compile with debug code")
endif()

# Setting the master option sets the rest
if(BUILD_ALL_EXAMPLES)
    set(BUILD_VGU ON)
    set(BUILD_DASH ON)
    set(BUILD_LINEAR ON)
    set(BUILD_RADIAL ON)
    set(BUILD_GRADIENT ON)
    set(BUILD_INTERPOLATE ON)
    set(BUILD_TIGER ON)
    set(BUILD_IMAGE ON)
    set(BUILD_PATTERN ON)
    set(BUILD_BLEND ON)
    set(BUILD_PORTERDUFF ON)
    set(BUILD_EB ON)
    set(BUILD_WARP ON)
    set(BUILD_PATH ON)
    set(BUILD_FLOWERS ON)
    set(BUILD_STRESS ON)
    set(BUILD_CLOCK ON)
    set(BUILD_SUBWAYMAP ON)
    set(BUILD_COMPOSITING ON)
    set(BUILD_COLORMATRIX ON)
    set(BUILD_GAUSSIANBLUR ON)
    set(BUILD_GAUSSIANBLUR2 ON)
    set(BUILD_CONVOLVE ON)
    set(BUILD_SEPARABLECONVOLVE ON)
    set(BUILD_LOOKUP ON)
    set(BUILD_LOOKUP2 ON)
    set(BUILD_LOOKUP_SINGLE ON)
    set(BUILD_COLOR ON)
    set(BUILD_SRGB ON)
    set(BUILD_IMAGE_MODE ON)
    set(BUILD_TILING_MODE ON)
endif()
set(BUILD_EXAMPLES BUILD_VGU OR BUILD_DASH OR BUILD_LINEAR OR BUILD_RADIAL OR BUILD_GRADIENT OR BUILD_INTERPOLATE OR BUILD_TIGER
  OR BUILD_IMAGE OR BUILD_PATTERN OR BUILD_BLEND OR BUILD_PORTERDUFF OR BUILD_EB OR BUILD_WARP OR BUILD_PATH OR BUILD_FLOWERS
  OR BUILD_STRESS OR BUILD_CLOCK OR BUILD_SUBWAYMAP OR BUILD_COMPOSITING OR BUILD_COLORMATRIX OR BUILD_CONVOLVE OR BUILD_SEPARABLECONVOLVE
  OR BUILD_GAUSSIANBLUR OR BUILD_GAUSSIANBLUR2 OR BUILD_LOOKUP OR BUILD_LOOKUP2 OR BUID_LOOKUP_SINGLE
  OR BUILD_COLOR OR BUILD_SRGB OR BUILD_IMAGE_MODE OR BUILD_TILING_MODE)

# Finding packages
# TODO: EGL/OpenGLES and friends?
# TODO: learn how to specify prefixes
# TODO: try linking actual libraries like configure does
# TODO: detailed report instead of failure like configure did
# TODO: properly look for GLX on XWindow-based systems
find_package(OpenGL REQUIRED)
if (OpenGL_GLX_FOUND)
  # X11 are required by GLX, are available?
  find_package(X11 REQUIRED)
endif()

if (OpenGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIR})
    link_libraries(${OPENGL_LIBRARIES})
endif()

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
endif()

if(BUILD_EXAMPLES)
    # All the examples require GLUT
    find_package(GLUT REQUIRED)
    if(BUILD_IMAGE OR BUILD_PATTERN)
        find_package(JPEG REQUIRED)
    endif()
endif()

# Platform checks
check_include_files(dlfcn.h HAVE_DLFCN_H)
check_include_files(inttypes.h HAVE_INTTYPES_H)
if (JPEG_FOUND)
  set(HAVE_JPEGLIB_H 1)
endif()
check_library_exists(m cos "" HAVE_LIBM) # TODO use link directories instead of "" location parameter
check_include_files(memory.h HAVE_MEMORY_H)
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(stdlib.h HAVE_STDLIB_H)
check_include_files(strings.h HAVE_STRINGS_H)
check_include_files(string.h HAVE_STRING_H)
check_include_files(stdio.h HAVE_STDIO_H)
check_include_files(sys/stat.h HAVE_SYS_STAT_H)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)
check_include_files(unistd.h HAVE_UNISTD_H)
check_type_size("void*" SIZEOF_VOID_P)

# Trying to get the version of the package
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    find_package(Git)
    if(GIT_FOUND)
        execute_process(COMMAND ${GIT_EXECUTABLE} describe WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE PACKAGE_VERSION RESULT_VARIABLE GIT_RESULT OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET) 
        if(GIT_RESULT)
            execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse -q HEAD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE PACKAGE_VERSION RESULT_VARIABLE GIT_RESULT OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
            if(GIT_RESULT)
                set(PACKAGE_VERSION "Unknown(error)")
            endif()
        endif()
    else()
        set(PACKAGE_VERSION $PROJECT_VERSION)
    endif()
else()
    set(PACKAGE_VERSION $PROJECT_VERSION)
endif()

# Names, strings etc
set(PACKAGE ${CMAKE_PROJECT_NAME})
set(PACKAGE_NAME ${PACKAGE})
set(PACKAGE_STRING "${PACKAGE}-${PACKAGE_VERSION}")
set(PACKAGE_TARNAME "Shivavg") # TODO I don't really think we -need- that

# Now generating the config.h and installing master includes
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include/VG)
# TODO this is really ugly, replace this with some intelligent way to detect glext availability
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Report on what's going to be built
message("ShivaVG will be compiled with the following example programs\n"
        "(run (cmake-gui, ccmake or cmake -D<option>) to change selection) :\n"
        "\n"
        "  VGU                       ${BUILD_VGU}\n"
        "  Dashing                   ${BUILD_DASH}\n"
        "  Linear Gradient           ${BUILD_LINEAR}\n"
        "  Radial Gradient           ${BUILD_RADIAL}\n"
        "  Path Interpolation        ${BUILD_INTERPOLATE}\n"
        "  Tiger SVG                 ${BUILD_TIGER}\n"
        "  Images                    ${BUILD_IMAGE}\n"
        "  Pattern paint             ${BUILD_PATTERN}\n"
        "  Blending                  ${BUILD_BLEND}\n"
        "  Porter Duff               ${BUILD_PORTERDUFF}\n"
        "  Electro Body              ${BUILD_EB}\n"
        "  Warp Image                ${BUILD_WARP}\n"
        "  Flowers                   ${BUILD_FLOWERS}\n"
        "  Stress test               ${BUILD_STRESS}\n"
        "  Clock                     ${BUILD_CLOCK}\n"
        "  Subway Map                ${BUILD_SUBWAYMAP}\n"
        "  Compositing               ${BUILD_COMPOSITING}\n"
        "  Color matrix              ${BUILD_COLORMATRIX}\n"
        "  Gaussian blur             ${BUILD_GAUSSIANBLUR}\n"
        "  Advanced Gaussian blur    ${BUILD_GAUSSIANBLUR2}\n"
        "  Convolve                  ${BUILD_CONVOLVE}\n"
        "  Separable Convolve        ${BUILD_CONVOLVE}\n"
        "  Color lookup              ${BUILD_LOOKUP}\n"
        "  Color lookup2             ${BUILD_LOOKUP2}\n"
        "  Color lookup single       ${BUILD_LOOKUP_SINGLE}\n"
        "  Simple color matrix       ${BUILD_COLOR}\n"
        "  sRGB color                ${BUILD_SRGB}\n"
        "  Image mode                ${BUILD_IMAGE_MODE}\n"
        "  Tiling mode               ${BUILD_TILING_MODE}\n"
        )

# Pick up subdirectories
# TODO see if include_directories work like a stack and whether we can avoid duplication
# TODO break subdir execution of somebody tries to run them directly
add_subdirectory(src)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()


# Installation

install(DIRECTORY include/VG DESTINATION include)

# add include directories
target_include_directories(ShivaVGShared
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# install the target and create export-set
install(TARGETS ShivaVGShared ShivaVGStatic
    EXPORT ShivaVGTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# generate and install export file
install(EXPORT ShivaVGTargets
    FILE ${SHARED_LIBRARY_NAME}Targets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${SHARED_LIBRARY_NAME}
)

include(CMakePackageConfigHelpers)

set(version $PROJECT_VERSION)

# generate the version file for the config file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${SHARED_LIBRARY_NAME}ConfigVersion.cmake"
    VERSION "${version}"
    COMPATIBILITY AnyNewerVersion
)

# create config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${SHARED_LIBRARY_NAME}Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${SHARED_LIBRARY_NAME}
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# install config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${SHARED_LIBRARY_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${SHARED_LIBRARY_NAME}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${SHARED_LIBRARY_NAME}
)

# TODO verify
# generate pkgconfig file
set(target1 ${SHARED_LIBRARY_NAME})
string(TOLOWER ${SHARED_LIBRARY_NAME} PKGCONFIG_NAME)
configure_file(shivavg.pc.in ${CMAKE_CURRENT_BINARY_DIR}/${PKGCONFIG_NAME}.pc @ONLY)

# install pkgconfig files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PKGCONFIG_NAME}.pc"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
