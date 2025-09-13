# set(CLANG_VALIDATION OFF CACHE BOOL "Validate input with Clang")
# configure_file(config.hpp.in include/config.hpp)

add_custom_command(
    COMMENT "Generating C parser"
    MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/cc/C.g4
    OUTPUT
        parser/CBaseListener.h
        parser/CBaseListener.cpp
        parser/CBaseVisitor.h
        parser/CBaseVisitor.cpp
        parser/CLexer.h
        parser/CLexer.cpp
        parser/CListener.h
        parser/CListener.cpp
        parser/CParser.h
        parser/CParser.cpp
        parser/CVisitor.h
        parser/CVisitor.cpp
    BYPRODUCTS
        parser/CLexer.interp
        parser/CLexer.tokens
        parser/CParser.interp
        parser/CParser.tokens
    COMMAND antlr4 -Dlanguage=Cpp -o ${CMAKE_BINARY_DIR}/cc/parser -visitor ${CMAKE_SOURCE_DIR}/cc/C.g4
)

add_executable(zdk-cc
    main.cpp
    error.cpp
    # analyzer.cpp
    # codegen.cpp
    ast.cpp
    ast_builder.cpp
    # ast_visitor.cpp

    parser/CBaseListener.cpp
    parser/CBaseVisitor.cpp
    parser/CLexer.cpp
    parser/CListener.cpp
    parser/CParser.cpp
    parser/CVisitor.cpp
)
target_include_directories(zdk-cc PRIVATE include ${CMAKE_SOURCE_DIR}/3rd/antlr4-cpp-runtime/runtime/src ${CMAKE_BINARY_DIR}/cc/parser ${CMAKE_BINARY_DIR}/cc/include)
target_link_libraries(zdk-cc PRIVATE argparse antlr4_static)
install(TARGETS zdk-cc RUNTIME DESTINATION bin)
