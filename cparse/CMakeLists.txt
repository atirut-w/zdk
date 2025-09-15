add_library(cparse STATIC
    lexer.cpp
)
target_include_directories(cparse PUBLIC include)
