# Tests for ZDK compiler

# C90 conformance test - runs on all test files
add_test(
    NAME test_c90_conformance
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test_conformance.sh
)
set_tests_properties(test_c90_conformance PROPERTIES
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Backend-specific tests
if(ZDK_TARGET STREQUAL "amd64")
    # Test AMD64 backend
    add_test(
        NAME test_amd64_simple_main
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test_amd64.sh
    )
    set_tests_properties(test_amd64_simple_main PROPERTIES
        ENVIRONMENT "BUILD_DIR=${CMAKE_BINARY_DIR}"
    )
elseif(ZDK_TARGET STREQUAL "z80")
    # Test Z80 backend
    add_test(
        NAME test_z80_simple_main
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test_z80.sh
    )
    set_tests_properties(test_z80_simple_main PROPERTIES
        ENVIRONMENT "BUILD_DIR=${CMAKE_BINARY_DIR}"
    )
endif()
