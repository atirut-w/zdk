add_custom_command(
    COMMENT "Generating parser"
    MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/3rd/grammars-v4/llvm-ir/LLVMIR.g4
    OUTPUT
        parser/LLVMIRBaseListener.h
        parser/LLVMIRBaseListener.cpp
        parser/LLVMIRBaseVisitor.h
        parser/LLVMIRBaseVisitor.cpp
        parser/LLVMIRLexer.h
        parser/LLVMIRLexer.cpp
        parser/LLVMIRListener.h
        parser/LLVMIRListener.cpp
        parser/LLVMIRParser.h
        parser/LLVMIRParser.cpp
        parser/LLVMIRVisitor.h
        parser/LLVMIRVisitor.cpp
    BYPRODUCTS
        parser/LLVMIRLexer.interp
        parser/LLVMIRLexer.tokens
        parser/LLVMIRParser.interp
        parser/LLVMIRParser.tokens
    COMMAND antlr4 -Dlanguage=Cpp -o ${CMAKE_BINARY_DIR}/src/cc/parser -visitor ${CMAKE_SOURCE_DIR}/3rd/grammars-v4/llvm-ir/LLVMIR.g4
)

add_executable(zdk-cc
    main.cpp
    codegen.cpp
    analyzer.cpp

    parser/LLVMIRBaseListener.cpp
    parser/LLVMIRBaseVisitor.cpp
    parser/LLVMIRLexer.cpp
    parser/LLVMIRListener.cpp
    parser/LLVMIRParser.cpp
    parser/LLVMIRVisitor.cpp
)
target_include_directories(zdk-cc PRIVATE include ${CMAKE_SOURCE_DIR}/3rd/antlr4-cpp-runtime/runtime/src ${CMAKE_BINARY_DIR}/src/cc/parser)
target_compile_features(zdk-cc PRIVATE cxx_std_17)
target_link_libraries(zdk-cc PRIVATE argparse antlr4_static)
