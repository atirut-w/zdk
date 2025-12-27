add_executable(cc
    src/main.cpp
)
target_include_directories(cc PRIVATE include)
target_compile_features(cc PRIVATE cxx_std_23)

# Add backend definitions to cc driver
if(ZDK_TARGET STREQUAL "amd64")
    target_compile_definitions(cc PRIVATE ENABLE_AMD64_BACKEND)
elseif(ZDK_TARGET STREQUAL "z80")
    target_compile_definitions(cc PRIVATE ENABLE_Z80_BACKEND)
endif()
