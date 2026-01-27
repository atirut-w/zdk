add_executable(cc1
    src/compilation_ctx.c
    src/main.c
)
target_compile_options(cc1 PRIVATE -std=c90)
target_include_directories(cc1 PRIVATE include)
