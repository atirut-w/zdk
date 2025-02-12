add_executable(zdk-cc
    main.c
)
target_compile_options(zdk-cc PRIVATE -std=c90 -pedantic-errors)
