add_executable(06_transform 06_transform.cpp)
target_link_libraries(06_transform ${LIBS})

install(FILES shader.fs DESTINATION ./06_transform)
install(FILES shader.vs DESTINATION ./06_transform)
