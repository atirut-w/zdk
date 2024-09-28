add_custom_command(
    COMMENT "Generating LLVM IR parser"
    MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/src/llc/LLVMIR.g4
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
    COMMAND antlr4 -Dlanguage=Cpp -o ${CMAKE_BINARY_DIR}/src/llc/parser -visitor ${CMAKE_SOURCE_DIR}/src/llc/LLVMIR.g4
)

add_executable(zdk-llc
    main.cpp

    parser/LLVMIRBaseListener.cpp
    parser/LLVMIRBaseVisitor.cpp
    parser/LLVMIRLexer.cpp
    parser/LLVMIRListener.cpp
    parser/LLVMIRParser.cpp
    parser/LLVMIRVisitor.cpp
)
target_include_directories(zdk-llc PRIVATE include ${CMAKE_SOURCE_DIR}/3rd/antlr4-cpp-runtime/runtime/src ${CMAKE_BINARY_DIR}/src/llc/parser ${CMAKE_BINARY_DIR}/src/llc/include)
target_compile_features(zdk-llc PRIVATE cxx_std_17)
target_link_libraries(zdk-llc PRIVATE argparse antlr4_static)
install(TARGETS zdk-llc DESTINATION bin)
