add_executable(cc
    src/main.c
    src/lexer.c
    src/parser.c
    src/ast.c
    src/symbols.c
)
target_compile_options(cc PRIVATE -std=c90 -pedantic-errors -Werror)
target_include_directories(cc PRIVATE include)
