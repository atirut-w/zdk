---
applyTo: "cc/**"
---

# General Guidelines
- The main compiler codes and the codegen driver must be target-agnostic.
- Target-specific implementations should be placed in their own codegen files.

# ABI and Calling Conventions
- Consult the ZDK documentation for details on the common ABI and calling conventions used across targets.
