add_test(NAME parser_regression_test COMMAND  python3 ${CMAKE_CURRENT_LIST_DIR}/parser_tests/run_tests.py)
add_test(NAME codegen_regression_test COMMAND python3 ${CMAKE_CURRENT_LIST_DIR}/codegen_tests/run_tests.py)
