# Find Bison and Flex (optional - for regenerating parser/lexer)
find_package(BISON)
find_package(FLEX)

# Define the generated file paths
set(PARSER_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.c)
set(PARSER_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.h)
set(LEXER_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.c)

# If Bison and Flex are available, set up generation targets
if(BISON_FOUND AND FLEX_FOUND)
  # Generate parser
  add_custom_command(
        OUTPUT ${PARSER_OUTPUT} ${PARSER_HEADER}
        COMMAND ${BISON_EXECUTABLE}
                -d
                -o ${PARSER_OUTPUT}
                ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.y
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.y
        COMMENT "Generating parser with Bison"
        VERBATIM
    )

  # Generate lexer
  add_custom_command(
        OUTPUT ${LEXER_OUTPUT}
        COMMAND ${FLEX_EXECUTABLE}
                -o ${LEXER_OUTPUT}
                ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.l
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.l ${PARSER_HEADER}
        COMMENT "Generating lexer with Flex"
        VERBATIM
    )

  # Custom target to regenerate parser and lexer
  add_custom_target(regenerate_parser
        DEPENDS ${PARSER_OUTPUT} ${PARSER_HEADER} ${LEXER_OUTPUT}
        COMMENT "Regenerating parser and lexer"
    )
endif()

# Check if generated files exist
if(NOT EXISTS ${PARSER_OUTPUT})
  if(BISON_FOUND AND FLEX_FOUND)
    message(STATUS "Generated parser files not found, they will be created during build")
  else()
    message(FATAL_ERROR "Generated parser files not found and Bison/Flex are not available. Please install Bison and Flex or check out the generated files from VCS.")
  endif()
endif()

add_executable(cc
    src/main.c
    ${PARSER_OUTPUT}
    ${LEXER_OUTPUT}
)

# Include directory for parser header
target_include_directories(cc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# If Bison and Flex are found, make the executable depend on the generation
if(BISON_FOUND AND FLEX_FOUND)
  add_dependencies(cc regenerate_parser)
endif()
