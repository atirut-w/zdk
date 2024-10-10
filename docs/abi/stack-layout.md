# Stack Layout
A function's stack frame under the ZDK ABI is laid out as follows, from the highest address to the lowest address:

| Name               | Notes                                                                     |
| ------------------ | ------------------------------------------------------------------------- |
| Parameters         | For parameter order, see the [calling convention](calling-convention.md). |
| Return Address     |                                                                           |
| Old Locals Pointer | The caller's stack pointer, if the callee allocates space for locals.     |
| Old Stack Pointer  | The previous stack frame's stack pointer.                                 |
| Locals             | Local variables.                                                          |
