add_executable(cc1
    src/main.c
)
target_compile_options(cc1 PRIVATE -std=c90)
target_include_directories(cc1 PRIVATE include)
