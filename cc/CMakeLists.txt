add_executable(cc
    src/main.c
)
target_include_directories(cc PRIVATE include)
target_compile_options(cc PRIVATE -std=c90)
