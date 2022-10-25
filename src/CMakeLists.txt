SET(SOURCES
	ast.cc
	ast_context.cc
	main.cc
	lexer.cc
	parser.cc
	type.cc
	codegen.cc
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
