add_executable(cc1
    src/main.cpp
)
target_include_directories(cc PRIVATE include)
target_compile_features(cc PRIVATE cxx_std_23)
