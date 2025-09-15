add_library(cparse STATIC
    lexer.cpp
    parser.cpp
)
target_include_directories(cparse PUBLIC include)
