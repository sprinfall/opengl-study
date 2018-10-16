project(common)

file(GLOB SRC *.h *.cpp)

add_library(common ${SRC})
