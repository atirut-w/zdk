---
applyTo: "cc/**"
---

# General Guidelines
- The main compiler codes and the codegen driver must be target-agnostic.
- Target-specific implementations must be put in target implementations, and not the main codegen driver.

# Code Generation
- Each codegen action must map to a C construct or a subset of C constructs.
- Avoid introducing new abstractions that do not have a direct mapping to C constructs.

# ABI and Calling Conventions
- Consult the ZDK documentation for details on the common ABI and calling conventions used across targets.
