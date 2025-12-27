add_executable(cc1
    src/main.c
    src/lexer.c
    src/parser.c
    src/codegen.c
)
target_include_directories(cc1 PRIVATE include)
target_compile_options(cc1 PRIVATE -std=c90)

# Add conformance tests
if(BUILD_TESTING)
    # Find all test files
    file(GLOB CONFORMANCE_TESTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/conformance/*.c")
    
    foreach(TEST_FILE ${CONFORMANCE_TESTS})
        get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
        add_test(
            NAME cc1_conformance_${TEST_NAME}
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/conformance/run_test.sh
                    $<TARGET_FILE:cc1>
                    ${TEST_FILE}
        )
    endforeach()
endif()
