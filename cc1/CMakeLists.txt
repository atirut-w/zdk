add_executable(cc1
    src/main.c
    src/diag.c
    src/emit.c
    src/lexer.c
    src/parse.c
    src/strpool.c
    src/sym.c
    src/type.c
    src/z80_cg.c
)
target_compile_options(cc1 PRIVATE -std=c90)
target_include_directories(cc1 PRIVATE include)

install(TARGETS cc1 RUNTIME DESTINATION libexec/${ZDK_TOOLCHAIN_NAME})
install(TARGETS cc1 RUNTIME DESTINATION libexec/zdk)
