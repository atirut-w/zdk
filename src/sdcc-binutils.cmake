set(CMAKE_SYSTEM_NAME Generic)
if(NOT DEFINED CMAKE_SYSTEM_PROCESSOR)
    set(CMAKE_SYSTEM_PROCESSOR z80)
endif()

set(CMAKE_ASM_COMPILER sdcc-sdas${CMAKE_SYSTEM_PROCESSOR})
set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> <FLAGS> <SOURCE>")

set(CMAKE_C_COMPILER sdcc-sdcc)

if(DEFINED CMAKE_SYSTEM_PROCESSOR)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-m${CMAKE_SYSTEM_PROCESSOR}>)
    add_link_options(-m${CMAKE_SYSTEM_PROCESSOR})
endif()
