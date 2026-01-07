add_executable(as
    src/main.c
    src/section.c
)
target_include_directories(as PRIVATE include)
target_link_libraries(as PRIVATE zobj)

install(TARGETS as DESTINATION bin)
