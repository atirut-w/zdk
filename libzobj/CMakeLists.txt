add_library(zobj STATIC
  src/exec.c
  src/reloc.c
  src/strtab.c
  src/symbol.c
)
target_include_directories(zobj PUBLIC include)
