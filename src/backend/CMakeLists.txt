add_library(backend OBJECT
    target_registry.cpp

    targets/z80/target.cpp
    targets/z80/codegen.cpp
)
target_include_directories(backend PUBLIC include)
