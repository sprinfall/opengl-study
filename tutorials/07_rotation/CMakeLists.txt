set(TARGET_NAME 07_rotation)

add_executable(${TARGET_NAME}
    07_rotation.cpp
    ../utility.cpp
    ../utility.h)

target_link_libraries(${TARGET_NAME} ${LIBS})

install(FILES shader.fs DESTINATION ./${TARGET_NAME})
install(FILES shader.vs DESTINATION ./${TARGET_NAME})
