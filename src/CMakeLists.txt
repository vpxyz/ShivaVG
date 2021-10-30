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
add_library(${SHARED_LIBRARY_NAME}_objlib OBJECT ${libShivaVG_SOURCES})
set_property(TARGET ${SHARED_LIBRARY_NAME}_objlib PROPERTY POSITION_INDEPENDENT_CODE 1)

# create library (static)
add_library(ShivaVGStatic STATIC $<TARGET_OBJECTS:${SHARED_LIBRARY_NAME}_objlib>)
set_target_properties(ShivaVGStatic PROPERTIES OUTPUT_NAME ${STATIC_LIBRARY_NAME})

# create library (shared)
add_library(ShivaVGShared SHARED $<TARGET_OBJECTS:${SHARED_LIBRARY_NAME}_objlib>)
target_link_libraries(ShivaVGShared ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} -lm)
set_target_properties(ShivaVGShared PROPERTIES LIBRARY_OUTPUT_NAME ${SHARED_LIBRARY_NAME})

