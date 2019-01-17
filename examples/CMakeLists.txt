cmake_minimum_required(VERSION 2.8)
# TODO headers as dependencies
# TODO images as dependencies
include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR} ${JPEG_INCLUDE_DIR})

set(EXAMPLE_SRCS test.c)
set(EXAMPLE_HRRS test.h)
set(EXAMPLE_LIBS ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${JPEG_LIBRARIES})
set(EXAMPLE_INCLUDE ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
set(EXAMPLE_tiger_SRC test_tiger_paths.c)
set(EXAMPLE_lodepng_SRC lodepng.c)
set(EXAMPLE_lodepng_HDR lodepng.h)

foreach (example vgu dash linear radial interpolate tiger image pattern blend eb warp path flowers stress clock subwaymap compositing colormatrix gaussianblur)
    string(TOUPPER ${example} EXAMPLE)
    if(BUILD_${EXAMPLE})
        include_directories(${EXAMPLE_${example}_INCLUDES})
        add_executable(test_${example} ${EXAMPLE_SRCS} test_${example}.c ${EXAMPLE_${example}_SRC} ${EXAMPLE_lodepng_SRC})
        target_link_libraries(test_${example} ${EXAMPLE_LIBS} OpenVG ${EXAMPLE_${example}_LIBS})
    endif()
endforeach()
