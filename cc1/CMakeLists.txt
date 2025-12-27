add_executable(cc1
    src/main.c
    src/lexer.c
    src/parser.c
    src/codegen.c
)
target_include_directories(cc1 PRIVATE include)
target_compile_options(cc1 PRIVATE -std=c90)
