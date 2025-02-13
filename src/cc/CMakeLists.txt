add_executable(zdk-cc
    main.c
    scan.c
    expr.c
    tree.c
    interp.c
)
target_compile_options(zdk-cc PRIVATE -std=c90 -pedantic-errors)
target_include_directories(zdk-cc PRIVATE include)
