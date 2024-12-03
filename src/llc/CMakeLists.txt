find_package(LLVM REQUIRED CONFIG)
llvm_map_components_to_libnames(LLVM_LIBS
    core
    support
    irreader
)

add_executable(zdk-llc
    main.cpp
)
target_link_libraries(zdk-llc PRIVATE argparse ${LLVM_LIBS} backend)
