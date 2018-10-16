add_executable(04_shaders 04_shaders.cpp)
target_link_libraries(04_shaders ${LIBS})

install(FILES shader.fs DESTINATION ./04_shaders)
install(FILES shader.vs DESTINATION ./04_shaders)
