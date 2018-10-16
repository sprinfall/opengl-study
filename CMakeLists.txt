cmake_minimum_required(VERSION 3.1.0)

if(POLICY CMP0074)
    # find_package() uses <PackageName>_ROOT variables.
    # This policy was introduced in CMake version 3.12.
    cmake_policy(SET CMP0074 NEW)
endif()

project(opengl-study)

# C++ standard requirements.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL)
if(OPENGL_FOUND)
    message(STATUS ${OPENGL_LIBRARIES})
endif()

find_package(GLUT)
if(GLUT_FOUND)
    message(STATUS ${GLUT_LIBRARIES})
endif()

find_package(GLEW)
if(GLEW_FOUND)
    message(STATUS ${GLEW_LIBRARIES})
endif()

# include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/common)

add_subdirectory(common)
add_subdirectory(tutorials)
