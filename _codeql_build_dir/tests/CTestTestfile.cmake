# CMake generated Testfile for 
# Source directory: /home/runner/work/zdk/zdk/tests
# Build directory: /home/runner/work/zdk/zdk/_codeql_build_dir/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[test_c90_conformance]=] "/home/runner/work/zdk/zdk/tests/test_conformance.sh")
set_tests_properties([=[test_c90_conformance]=] PROPERTIES  WORKING_DIRECTORY "/home/runner/work/zdk/zdk/tests" _BACKTRACE_TRIPLES "/home/runner/work/zdk/zdk/tests/CMakeLists.txt;4;add_test;/home/runner/work/zdk/zdk/tests/CMakeLists.txt;0;")
add_test([=[test_amd64_simple_main]=] "/home/runner/work/zdk/zdk/tests/test_amd64.sh")
set_tests_properties([=[test_amd64_simple_main]=] PROPERTIES  ENVIRONMENT "BUILD_DIR=/home/runner/work/zdk/zdk/_codeql_build_dir" _BACKTRACE_TRIPLES "/home/runner/work/zdk/zdk/tests/CMakeLists.txt;15;add_test;/home/runner/work/zdk/zdk/tests/CMakeLists.txt;0;")
