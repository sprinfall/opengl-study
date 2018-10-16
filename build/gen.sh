cmake -G"Unix Makefiles"\
    -DCMAKE_INSTALL_PREFIX=./tutorials\
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

cp compile_commands.json ..
