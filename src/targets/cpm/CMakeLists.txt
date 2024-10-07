add_library(cpm OBJECT
    crt0.s
)
target_link_options(cpm PUBLIC -T ${CMAKE_CURRENT_SOURCE_DIR}/cpm.ld)
