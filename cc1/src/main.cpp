#include "lexer.hpp"
#include "parser.hpp"
#include "codegen.hpp"
#include <iostream>
#include <fstream>
#include <memory>
#include <cstring>

void printUsage(const char* progName) {
    std::cerr << "Usage: " << progName << " [options] <input-file>\n";
    std::cerr << "Options:\n";
    std::cerr << "  -o <file>    Write output to <file>\n";
    std::cerr << "  -h, --help   Display this help message\n";
}

int main(int argc, char* argv[]) {
    const char* inputFile = nullptr;
    const char* outputFile = nullptr;
    
    /* Parse command line arguments */
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "Error: -o requires an argument\n";
                return 1;
            }
        } else if (std::strcmp(argv[i], "-h") == 0 || 
                   std::strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            return 0;
        } else if (argv[i][0] == '-') {
            std::cerr << "Error: Unknown option: " << argv[i] << "\n";
            printUsage(argv[0]);
            return 1;
        } else {
            if (inputFile == nullptr) {
                inputFile = argv[i];
            } else {
                std::cerr << "Error: Multiple input files specified\n";
                return 1;
            }
        }
    }
    
    if (inputFile == nullptr) {
        std::cerr << "Error: No input file specified\n";
        printUsage(argv[0]);
        return 1;
    }
    
    try {
        /* Open input file */
        std::ifstream input(inputFile);
        if (!input) {
            std::cerr << "Error: Cannot open input file: " << inputFile << "\n";
            return 1;
        }
        
        /* Lexical analysis */
        Lexer lexer(input);
        auto tokens = lexer.tokenize();
        
        /* Parsing */
        Parser parser(tokens);
        auto ast = parser.parse();
        
        /* Code generation */
        std::unique_ptr<CodeGenerator> codegen;
        
#ifdef ENABLE_AMD64_BACKEND
        codegen = std::make_unique<AMD64CodeGenerator>();
#elif defined(ENABLE_Z80_BACKEND)
        codegen = std::make_unique<Z80CodeGenerator>();
#else
#error "No backend enabled. Please define ENABLE_AMD64_BACKEND or ENABLE_Z80_BACKEND"
#endif
        
        /* Open output file or use stdout */
        if (outputFile != nullptr) {
            std::ofstream output(outputFile);
            if (!output) {
                std::cerr << "Error: Cannot open output file: " << outputFile << "\n";
                return 1;
            }
            codegen->generate(*ast, output);
        } else {
            codegen->generate(*ast, std::cout);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
