cmake_minimum_required(VERSION 3.0)
# TODO headers as dependencies
# TODO images as dependencies
include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR} ${JPEG_INCLUDE_DIR})

set(EXAMPLE_SRC test.c)
set(EXAMPLE_HDR test.h)
set(EXAMPLE_LIBS ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${JPEG_LIBRARIES})
set(EXAMPLE_INCLUDE ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
set(EXAMPLE_tiger_SRC test_tiger_paths.c)
set(EXAMPLE_lodepng_SRC lodepng.c)
set(EXAMPLE_lodepng_HDR lodepng.h)

add_library(lodepng OBJECT ${EXAMPLE_lodepng_SRC} ${EXAMPLE_lodepng_HDR})
add_library(test_common OBJECT ${EXAMPLE_SRC} ${EXAMPLE_HDR})

foreach (example vgu dash linear radial interpolate tiger image pattern blend eb warp path flowers stress clock subwaymap compositing colormatrix gaussianblur gaussianblur2 lookup lookup2 lookup_single color convolve separableconvolve image_mode tiling_mode gradient porterduff srgb)
    string(TOUPPER ${example} EXAMPLE)
    if(BUILD_${EXAMPLE})
        include_directories(${EXAMPLE_${example}_INCLUDES})
        add_executable(test_${example} test_${example}.c ${EXAMPLE_${example}_SRC})
        target_link_libraries(test_${example} ${EXAMPLE_LIBS} OpenVG ${EXAMPLE_${example}_LIBS} lodepng test_common)
    endif()
endforeach()
