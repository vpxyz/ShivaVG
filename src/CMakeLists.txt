cmake_minimum_required(VERSION 3.0)
# TODO headers as dependencies
# TODO shared/static switch CMake-style
set(libOpenVG_SOURCES
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
set(libOpenVG_HEADERS
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
add_library(OpenVG_objlib OBJECT ${libOpenVG_SOURCES})
set_property(TARGET OpenVG_objlib PROPERTY POSITION_INDEPENDENT_CODE 1)
add_library(OpenVG SHARED $<TARGET_OBJECTS:OpenVG_objlib>)
add_library(OpenVGStatic STATIC $<TARGET_OBJECTS:OpenVG_objlib>)
target_link_libraries(OpenVG ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} -lm)

install(TARGETS OpenVG DESTINATION lib)
