set(LIBS common ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES})

set(SRCS
    01_create_window
    02_hello_dot
    03_first_triangle
    )

foreach(target ${SRCS})
    add_executable(${target} ${target}.cpp)
    target_link_libraries(${target} ${LIBS})
endforeach()

add_subdirectory(04_shaders)
add_subdirectory(05_uniform_variables)
add_subdirectory(06_translation)
add_subdirectory(07_rotation)
