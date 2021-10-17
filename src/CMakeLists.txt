# TODO headers as dependencies
# TODO shared/static switch CMake-style

set(libShivaVG_SOURCES
    shExtensions.c
    shArrays.c
    shVectors.c
    shPath.c
    shImage.c
    shPaint.c
    shGeometry.c
    shPipeline.c
    shParams.c
    shContext.c
    shVgu.c
    shMath.c
    shCommons.c
)

set(libShivaVG_HEADERS
    shDefs.h
    shExtensions.h
    shArrayBase.h
    shArrays.h
    shVectors.h
    shPath.h
    shImage.h
    shPaint.h
    shGeometry.h
    shContext.h
    shMath.h
    shCommons.h
    )

add_definitions(-DHAVE_CONFIG_H)

# include_directories(${OPENGL_INCLUDE_DIR})
# TODO what if OpenGL library has nonstandard location?

# create object
add_library(ShivaVG_objlib OBJECT ${libShivaVG_SOURCES})
set_property(TARGET ShivaVG_objlib PROPERTY POSITION_INDEPENDENT_CODE 1)

# create library (static)
add_library(ShivaVGStatic STATIC $<TARGET_OBJECTS:ShivaVG_objlib>)
set_target_properties(ShivaVGStatic PROPERTIES OUTPUT_NAME ShivaVG)
# FIXME output name is ShivaVGStatic.a

# create library (shared)
add_library(ShivaVGShared SHARED $<TARGET_OBJECTS:ShivaVG_objlib>)
target_link_libraries(ShivaVGShared ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} -lm)
set_target_properties(ShivaVGShared PROPERTIES LIBRARY_OUTPUT_NAME ShivaVG)

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

# header files (include/VG) are installed by ../CMakeLists.txt

# generate and install export file
install(EXPORT ShivaVGTargets
    FILE ShivaVGTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ShivaVG
)



include(CMakePackageConfigHelpers)

set(version $PROJECT_VERSION)

# generate the version file for the config file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ShivaVGConfigVersion.cmake"
    VERSION "${version}"
    COMPATIBILITY AnyNewerVersion
)

# create config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/ShivaVGConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ShivaVG
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# install config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ShivaVGConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ShivaVGConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ShivaVG
)



# TODO verify
# generate pkgconfig file
set(target1 ShivaVG)
configure_file(shivavg.pc.in lib/pkgconfig/shivavg.pc @ONLY)
