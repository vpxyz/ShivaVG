# USAGE
# copy this file to your project, where ShivaVG is used
# -> your_project/cmake/modules/FindShivaVG.cmake
#
# add to your CMakeLists.txt:
#
#   find_package(ShivaVG REQUIRED)
#   message("SHIVAVG_INCLUDE_DIR = ${SHIVAVG_INCLUDE_DIR}")
#   message("SHIVAVG_LIBRARIES = ${SHIVAVG_LIBRARIES}")

# CMake module to search for the ShivaVG library
# based on https://github.com/noirotm/flvmeta/blob/master/cmake/modules/FindLibYAML.cmake
#
# If cmake finds ShivaVG, it will set the variables
#    SHIVAVG_FOUND
#    SHIVAVG_INCLUDE_DIR
#    SHIVAVG_LIBRARIES

FIND_PATH(SHIVAVG_INCLUDE_DIR NAMES VG/vgu.h VG/openvg.h)
FIND_LIBRARY(SHIVAVG_LIBRARIES NAMES ShivaVG libShivaVG)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ShivaVG DEFAULT_MSG SHIVAVG_LIBRARIES SHIVAVG_INCLUDE_DIR)
MARK_AS_ADVANCED(SHIVAVG_INCLUDE_DIR SHIVAVG_LIBRARIES)
