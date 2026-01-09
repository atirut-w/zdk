add_executable(as
    src/main.c
    src/section.c
    src/symbol.c
    src/symref.c
    src/symtab.c
)
target_include_directories(as PRIVATE include)
target_link_libraries(as PRIVATE zobj)

install(TARGETS as DESTINATION bin)
