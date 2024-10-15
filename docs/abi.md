# Application Binary Interface
The ZDK project specifies an Application Binary Interface (ABI) employed across its compilers, tools, and libraries. This ABI is based on the [cdecl](https://en.wikipedia.org/wiki/X86_calling_conventions#cdecl) calling convention and adheres to conventional C symbol naming rules.

## Function Naming
In contrast to traditional 8-bit compilers, ZDK functions do not have an underscore prefix. This approach simplifies the compilation and linking processes, resulting in cleaner and more intuitive function names.

## Function Parameters
- Parameters are pushed onto the stack from right to left.
- The caller is responsible for cleaning up the stack after the function returns.

## Return Values
- Single-byte values are returned in the `A` register.
- Multi-byte values are returned with the most significant word in the `DE` register pair and the least significant word in the `HL` register pair.

## Register Usage
- Functions can freely use the `A`, `B`, `C`, `D`, `E`, `H`, `L` registers and their pairs without restrictions.
- The caller must save and restore any registers it needs to preserve across function calls.
- If the `IX` register is used (e.g., for indexing arguments or local variables), the callee must preserve it.
