set(TARGET_NAME 06_translation)

add_executable(${TARGET_NAME}
    06_translation.cpp
    ../utility.cpp
    ../utility.h)

target_link_libraries(${TARGET_NAME} ${LIBS})

install(FILES shader.fs DESTINATION ./${TARGET_NAME})
install(FILES shader.vs DESTINATION ./${TARGET_NAME})
