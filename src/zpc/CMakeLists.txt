add_custom_command(
    COMMENT "Generating Pascal parser"
    MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/3rd/grammars-v4/pascal/pascal.g4
    OUTPUT
        parser/pascalBaseListener.h
        parser/pascalBaseListener.cpp
        parser/pascalBaseVisitor.h
        parser/pascalBaseVisitor.cpp
        parser/pascalLexer.h
        parser/pascalLexer.cpp
        parser/pascalListener.h
        parser/pascalListener.cpp
        parser/pascalParser.h
        parser/pascalParser.cpp
        parser/pascalVisitor.h
        parser/pascalVisitor.cpp
    BYPRODUCTS
        parser/pascalLexer.interp
        parser/pascalLexer.tokens
        parser/pascalParser.interp
        parser/pascalParser.tokens
    COMMAND antlr4 -Dlanguage=Cpp -o ${CMAKE_BINARY_DIR}/src/zpc/parser -visitor ${CMAKE_SOURCE_DIR}/3rd/grammars-v4/pascal/pascal.g4
)

add_executable(zdk-zpc
    main.cpp

    parser/pascalBaseListener.cpp
    parser/pascalBaseVisitor.cpp
    parser/pascalLexer.cpp
    parser/pascalListener.cpp
    parser/pascalParser.cpp
    parser/pascalVisitor.cpp
)
target_include_directories(zdk-zpc PRIVATE include ${CMAKE_SOURCE_DIR}/3rd/antlr4-cpp-runtime/runtime/src ${CMAKE_BINARY_DIR}/src/cc/parser ${CMAKE_BINARY_DIR}/src/cc/include)
target_compile_features(zdk-zpc PRIVATE cxx_std_17)
target_link_libraries(zdk-zpc PRIVATE argparse antlr4_static)
install(TARGETS zdk-zpc RUNTIME DESTINATION bin)
