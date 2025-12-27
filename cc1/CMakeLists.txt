add_executable(cc1
    src/main.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/ast/ast.cpp
)

target_include_directories(cc1 PRIVATE include)
target_compile_features(cc1 PRIVATE cxx_std_23)

# Conditionally compile backends based on target
if(ZDK_TARGET STREQUAL "amd64")
    target_sources(cc1 PRIVATE src/codegen/amd64.cpp)
    target_compile_definitions(cc1 PRIVATE ENABLE_AMD64_BACKEND)
    message(STATUS "Building cc1 with AMD64 backend")
elseif(ZDK_TARGET STREQUAL "z80")
    target_sources(cc1 PRIVATE src/codegen/z80.cpp)
    target_compile_definitions(cc1 PRIVATE ENABLE_Z80_BACKEND)
    message(STATUS "Building cc1 with Z80 backend")
else()
    message(FATAL_ERROR "Invalid ZDK_TARGET: ${ZDK_TARGET}. Must be 'amd64' or 'z80'")
endif()
