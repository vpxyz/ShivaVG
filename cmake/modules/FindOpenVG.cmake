# USAGE
# copy this file to your project, where OpenVG is used
# -> your_project/cmake/modules/FindOpenVG.cmake
#
# add to your CMakeLists.txt:
#
#   find_package(OpenVG REQUIRED)
#   message("OPENVG_INCLUDE_DIR = ${OPENVG_INCLUDE_DIR}")
#   message("OPENVG_LIBRARIES = ${OPENVG_LIBRARIES}")

# CMake module to search for the OpenVG library
# based on https://github.com/noirotm/flvmeta/blob/master/cmake/modules/FindLibYAML.cmake
#
# If cmake finds OpenVG, it will set the variables
#    OPENVG_FOUND
#    OPENVG_INCLUDE_DIR
#    OPENVG_LIBRARIES

FIND_PATH(OPENVG_INCLUDE_DIR NAMES VG/vgu.h VG/openvg.h)
FIND_LIBRARY(OPENVG_LIBRARIES NAMES OpenVG libOpenVG)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ShivaVG DEFAULT_MSG OPENVG_LIBRARIES OPENVG_INCLUDE_DIR)
MARK_AS_ADVANCED(OPENVG_INCLUDE_DIR OPENVG_LIBRARIES)
