add_executable(zdk-cc
    src/main.cpp
)
target_compile_features(zdk-cc PRIVATE cxx_std_17)
target_link_libraries(zdk-cc PRIVATE argparse)
