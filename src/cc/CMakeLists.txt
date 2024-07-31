add_executable(zdk-cc
    main.cpp
    lexer.cpp
)
target_include_directories(zdk-cc PRIVATE include)
target_compile_features(zdk-cc PRIVATE cxx_std_17)
target_link_libraries(zdk-cc PRIVATE argparse)
