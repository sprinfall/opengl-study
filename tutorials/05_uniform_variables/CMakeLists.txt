add_executable(05_uniform_variables 05_uniform_variables.cpp)
target_link_libraries(05_uniform_variables ${LIBS})

install(FILES shader.fs DESTINATION ./05_uniform_variables)
install(FILES shader.vs DESTINATION ./05_uniform_variables)
