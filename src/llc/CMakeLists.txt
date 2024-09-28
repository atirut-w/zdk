add_executable(zdk-llc
    "main.cpp"
)
target_include_directories(zdk-llc PRIVATE "include")
target_compile_features(zdk-llc PRIVATE cxx_std_17)
target_link_libraries(zdk-llc PRIVATE argparse)
install(TARGETS zdk-llc DESTINATION bin)
