add_library(cpm OBJECT
    crt0.s
)
add_link_options(-T ${CMAKE_CURRENT_SOURCE_DIR}/cpm.ld)
