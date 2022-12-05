SET(SOURCES
	ast.cc
	ast_node.cc
	ast_context.cc
	codegen.cc
	driver.cc
	lexer.cc
	main.cc
	parser.cc
	type.cc
)

add_executable(
	jcc
	${SOURCES}
)
target_include_directories(
	jcc
	PUBLIC
	${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(
    jcc
    ${CONAN_LIBS}
)
