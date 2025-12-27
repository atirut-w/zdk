# GitHub Copilot Instructions for ZDK

## Project Overview

ZDK is a comprehensive development kit for Z80 development. This project aims to provide a complete toolchain and development environment for Z80 processors.

## Project Components

The ZDK project is planned to include the following components:

- **cc**: Compiler driver that orchestrates the compilation process
- **cc1**: The actual compiler implementation that transforms source code to Z80 assembly/machine code
- **libc**: C standard library implementation optimized for Z80 architecture
- Additional Z80 development utilities and tools

## Important Guidelines

### Documentation Updates

**When making significant changes to the codebase, you MUST update the following:**

1. **README.md**: Update the main project documentation if:
   - New components or features are added
   - Build/installation instructions change
   - Project scope or architecture changes

2. **Code Comments**: Add or update comments in code when:
   - Implementing complex algorithms or Z80-specific optimizations
   - Adding new public APIs or interfaces
   - Making architectural decisions that might not be obvious

3. **This File**: Update `.github/copilot-instructions.md` if:
   - New coding conventions are established
   - Project structure changes significantly
   - New development tools or dependencies are added

### Coding Standards

- Follow the existing CMake-based build system
- Use `-pedantic -pedantic-errors -Wall -Wextra` compiler flags as configured
- Write clear, maintainable code with consideration for Z80 target architecture
- Include unit tests using CTest framework for new functionality
- Keep code modular and well-organized

### Z80 Architecture Considerations

- Remember that Z80 is an 8-bit architecture with limited resources
- Consider memory constraints and performance optimization
- Use appropriate data types and structures for embedded systems
- Be mindful of endianness and alignment issues

### Build System

- Use CMake (minimum version 3.22) for build configuration
- Tests are managed through CTest (enabled via `include(CTest)`)
- Standard tasks are configured in `.zed/tasks.json`:
  - Configure: `cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug`
  - Build: `cmake --build build --parallel`
  - Test: `ctest --test-dir build --output-on-failure`

### Development Workflow

1. Make changes in focused, incremental commits
2. Test changes using the CTest framework
3. Update documentation as needed (see "Documentation Updates" above)
4. Ensure code follows the project's coding standards
5. Validate that changes work with Z80 Binutils toolchain when applicable

## Z80 Toolchain

The project uses the Z80 Binutils toolchain (`z80-unknown-none-elf`). When generating Z80 assembly or working with low-level code:

- Use appropriate Z80 instruction mnemonics
- Follow Z80 calling conventions
- Consider register usage patterns common in Z80 development
- Test generated code with the Z80 toolchain when possible

## Notes

- Project is in early development stages
- Architecture and component structure may evolve
- Always consider backward compatibility when possible
- Prioritize correctness and maintainability over premature optimization
