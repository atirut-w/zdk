find_package(FLEX)
find_package(BISON)

if(FLEX_FOUND)
    flex_target(lexer src/lexer.l ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.c
        DEFINES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/include/lexer.h)
endif()

if(BISON_FOUND)
    bison_target(parser src/parser.y ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.c
        DEFINES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/include/parser.h)
    add_flex_bison_dependency(lexer parser)
endif()

add_executable(cc
    src/main.c
    ${FLEX_lexer_OUTPUTS}
    ${BISON_parser_OUTPUTS}
)
target_compile_options(cc PRIVATE -std=gnu90)
target_include_directories(cc PRIVATE include)
