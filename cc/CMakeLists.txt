find_package(LLVM REQUIRED CONFIG)
llvm_map_components_to_libnames(LLVM_LIBS
    Core
)

add_executable(zdk-cc
    main.cpp
    ir_emitter.cpp
)
target_compile_definitions(zdk-cc PRIVATE
    ${LLVM_DEFINITIONS}
)
target_include_directories(zdk-cc PRIVATE
    include
    ${LLVM_INCLUDE_DIRS}
)
target_link_libraries(zdk-cc PRIVATE
    argparse
    cparse
    ${LLVM_LIBS}
)
