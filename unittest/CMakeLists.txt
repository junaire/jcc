add_executable(
	test_lexer
	${PROJECT_SOURCE_DIR}/unittest/test_lexer.cc
	${PROJECT_SOURCE_DIR}/src/lexer.cc
)

target_include_directories(
	test_lexer
	PUBLIC
	${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(
    test_lexer
    ${CONAN_LIBS}
)

add_test(NAME test_lexer COMMAND  ${CMAKE_BINARY_DIR}/bin/test_lexer)
