find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(LLVM_LIBRARIES
    Core
    Support
    IRReader
)

add_executable(zdk-llc
    main.cpp
    # codegen.cpp
    allocator.cpp
    ir_visitor.cpp
    liveness_analyzer.cpp
)
target_include_directories(zdk-llc PRIVATE include ${CMAKE_BINARY_DIR}/src/llc/include)
target_compile_features(zdk-llc PRIVATE cxx_std_17)
target_link_libraries(zdk-llc PRIVATE argparse ${LLVM_LIBRARIES})
install(TARGETS zdk-llc DESTINATION bin)
