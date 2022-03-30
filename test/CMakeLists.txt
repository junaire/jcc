add_executable(
	test_lexer
	${PROJECT_SOURCE_DIR}/test/test_lexer.cc
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
