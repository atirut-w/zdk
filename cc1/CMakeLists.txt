add_executable(cc1
    src/compilation_ctx.c
    src/diagnostic.c
    src/lexer.c
    src/main.c
    src/queue.c
    src/string_pool.c
    src/token.c
)
target_compile_options(cc1 PRIVATE -std=c90)
target_include_directories(cc1 PRIVATE include)
