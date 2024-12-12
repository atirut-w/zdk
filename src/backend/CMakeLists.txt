add_library(backend STATIC
    asm_printer.cpp
    allocator.cpp
)
target_include_directories(backend PUBLIC include)
