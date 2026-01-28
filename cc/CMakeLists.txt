add_executable(cc
    src/cc.c
    src/driver_util.c
)
target_compile_options(cc PRIVATE -std=c90)

install(TARGETS cc RUNTIME DESTINATION bin)

install(PROGRAMS mksysroot.sh DESTINATION bin RENAME zdk-mksysroot)
