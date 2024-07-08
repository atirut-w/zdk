# Debugging the Compiler

!!! note "TODO"
    Flesh out the content

## Breakpoints
This is usually the first choice you'll want to use when debugging the compiler's internals. It is not the best when debugging issues related to codegen, but it is very useful for checking why that piece of code won't run or (rarely) why the codegen is not producing the desired assembly output.

## Codegen Issues
If you are having issues with the compiler's codegen or just need to know how the AST is structured to break the compiler, you can use [ANTLR Lab](https://lab.antlr.org) to get a high-level overview of the input code's AST. This is great when you're trying to debug the codegen stage, which is implemented as a visitor pattern in the compiler, much like the analyzer.
