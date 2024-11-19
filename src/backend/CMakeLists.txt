add_library(backend STATIC
    asm_printer.cpp
    register_allocator.cpp
)
target_include_directories(backend PUBLIC include)
