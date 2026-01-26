add_executable(cc1
    src/lexer.cpp
    src/main.cpp
)
target_include_directories(cc1 PRIVATE include)
target_link_libraries(cc1 PRIVATE langkit)
