find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Generate parser (Bison) and lexer (Flex) directly into the source directory so
# the generated files can be checked into version control if desired.
BISON_TARGET(parser
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.y
    ${CMAKE_CURRENT_SOURCE_DIR}/src/y.tab.c
    DEFINES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/y.tab.h)
FLEX_TARGET(lexer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.l
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.c)
ADD_FLEX_BISON_DEPENDENCY(lexer parser)

add_executable(cc
    src/main.c
    src/ast.c
    src/symbols.c
    src/sema.c
    src/errorreport.c
    src/codegen.c
    src/target.c
    src/target_z80.c
    ${BISON_parser_OUTPUTS}
    ${FLEX_lexer_OUTPUTS}
)
target_compile_options(cc PRIVATE -std=c90 -pedantic-errors -Werror)
target_include_directories(cc PRIVATE include)

# Convenience target to force regeneration (e.g. `cmake --build . --target regenerate_grammar`).
add_custom_target(regenerate_grammar
    DEPENDS ${BISON_parser_OUTPUTS} ${FLEX_lexer_OUTPUTS}
    COMMENT "Regenerate parser and lexer sources")
