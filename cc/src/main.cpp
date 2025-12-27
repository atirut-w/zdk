#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <vector>

void printUsage(const char* progName) {
    std::cerr << "Usage: " << progName << " [options] <input-file>\n";
    std::cerr << "Options:\n";
    std::cerr << "  -o <file>    Write output to <file>\n";
    std::cerr << "  -S           Compile only; do not assemble\n";
    std::cerr << "  -c           Compile and assemble; do not link\n";
    std::cerr << "  -h, --help   Display this help message\n";
}

int runCommand(const std::vector<std::string>& args) {
    std::vector<char*> cArgs;
    for (const auto& arg : args) {
        cArgs.push_back(const_cast<char*>(arg.c_str()));
    }
    cArgs.push_back(nullptr);
    
    pid_t pid = fork();
    if (pid == 0) {
        /* Child process */
        execvp(cArgs[0], cArgs.data());
        /* If execvp returns, there was an error */
        std::cerr << "Error: Failed to execute " << cArgs[0] << "\n";
        exit(1);
    } else if (pid > 0) {
        /* Parent process */
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        return 1;
    } else {
        std::cerr << "Error: Failed to fork process\n";
        return 1;
    }
}

std::string getAssembler() {
    /* Determine assembler based on target */
#ifdef ENABLE_AMD64_BACKEND
    return "as";
#elif defined(ENABLE_Z80_BACKEND)
    /* Try Z80 assembler first, fall back to generic as */
    if (system("which z80-unknown-none-elf-as >/dev/null 2>&1") == 0) {
        return "z80-unknown-none-elf-as";
    }
    return "as";
#else
    return "as";
#endif
}

std::string getLinker() {
    /* Determine linker based on target */
#ifdef ENABLE_AMD64_BACKEND
    return "ld";
#elif defined(ENABLE_Z80_BACKEND)
    /* Try Z80 linker first, fall back to generic ld */
    if (system("which z80-unknown-none-elf-ld >/dev/null 2>&1") == 0) {
        return "z80-unknown-none-elf-ld";
    }
    return "ld";
#else
    return "ld";
#endif
}

int main(int argc, char* argv[]) {
    const char* inputFile = nullptr;
    const char* outputFile = nullptr;
    bool compileOnly = false;
    bool assembleOnly = false;
    
    /* Parse command line arguments */
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "Error: -o requires an argument\n";
                return 1;
            }
        } else if (std::strcmp(argv[i], "-S") == 0) {
            compileOnly = true;
        } else if (std::strcmp(argv[i], "-c") == 0) {
            assembleOnly = true;
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
    
    /* Determine output file names */
    std::string asmFile;
    std::string objFile;
    std::string exeFile;
    
    if (outputFile != nullptr) {
        if (compileOnly) {
            asmFile = outputFile;
        } else if (assembleOnly) {
            objFile = outputFile;
            asmFile = std::string(inputFile) + ".s";
        } else {
            exeFile = outputFile;
            asmFile = std::string(inputFile) + ".s";
            objFile = std::string(inputFile) + ".o";
        }
    } else {
        asmFile = std::string(inputFile) + ".s";
        objFile = std::string(inputFile) + ".o";
        exeFile = "a.out";
    }
    
    /* Run cc1 to compile to assembly (assumes cc1 is in PATH) */
    std::vector<std::string> cc1Args = {"cc1", "-o", asmFile, inputFile};
    int result = runCommand(cc1Args);
    if (result != 0) {
        return result;
    }
    
    if (compileOnly) {
        return 0;
    }
    
    /* Run assembler */
    std::string assembler = getAssembler();
    std::vector<std::string> asArgs = {assembler, "-o", objFile, asmFile};
    result = runCommand(asArgs);
    if (result != 0) {
        std::cerr << "Error: Assembly failed\n";
        return result;
    }
    
    /* Clean up assembly file (intermediate, not final output) */
    /* Note: We've moved past the assembly stage, so .s is no longer needed */
    unlink(asmFile.c_str());
    
    if (assembleOnly) {
        return 0;
    }
    
    /* Run linker */
    std::string linker = getLinker();
    std::vector<std::string> ldArgs = {linker, "-o", exeFile, objFile};
    result = runCommand(ldArgs);
    if (result != 0) {
        std::cerr << "Error: Linking failed\n";
        return result;
    }
    
    /* Clean up object file (intermediate, not final output) */
    /* Note: We've moved past the linking stage, so .o is no longer needed */
    unlink(objFile.c_str());
    
    return 0;
}
