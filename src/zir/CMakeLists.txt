add_library(zir STATIC
    instruction.cpp
    module.cpp
)
target_include_directories(zir PUBLIC include)
